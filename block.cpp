#include "block.h"
#include "painter.h"
#include "settings.h"
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QTime>
extern "C" {
#include "luasocket.h"
}

Block::Block(QObject *parent) : QObject(parent) { L = luaL_newstate(); connect(&watcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString)));useFake=false;}

void Block::load(QString fileName) {
    //watcher.addPath(fileName);
    Block::fileName = fileName;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QString data = file.readAll();
    luaL_openlibs(L);
    // luaopen_socket_core(L);
    // lua_setglobal(L,"socket");
    if (luaL_loadstring(L, data.toUtf8().data()) == LUA_OK) {
        mainRef=luaL_ref(L,LUA_REGISTRYINDEX);
        init(this);
    } else
        qDebug() << "ERR2" << lua_tostring(L, -1);
}

QPicture Block::drawBlock(QColor color, bool plain) {
    QPicture ret;
    ret.setBoundingRect(QRect(0,0,(width * Settings::final()->gridSize()) +
                              Settings::final()->gridSize() + 1,
                              (height * Settings::final()->gridSize()) + 1 +
                              Settings::final()->gridSize()));
    QPainter qpainter(&ret);
    qpainter.setPen(Qt::black);
    pushGlobal(L);
    Painter painter(&qpainter);
    lua_pushinteger(L, QTime::currentTime().msecsSinceStartOfDay());
    lua_setglobal(L, "time");


    lua_rawgeti(L,LUA_REGISTRYINDEX,state);
    lua_setglobal(L,"state");



    lua_getglobal(L, "paintEvent");
    qpainter.setRenderHint(QPainter::Antialiasing, true);
    painter.pushMe(L);
    lua_pushstring(L,color.name().toUtf8().data());
    if (lua_pcall(L, 2, 0, 0) == LUA_OK) {
        qpainter.setRenderHint(QPainter::Antialiasing, false);
        lua_getglobal(L,"state");
        lua_rawseti(L,LUA_REGISTRYINDEX,state);
    } else
        qDebug() << "ERR3" << lua_tostring(L, -1);
    qpainter.setPen(Qt::black);
    for (int i = 0; i < pins.length(); i++) {
        QPen pen(pins[i].color);
        if(pen.color()==Qt::transparent)
            pen.setColor(Qt::black);
        pen.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
        qpainter.setPen(pen);
        QPoint dir(Settings::final()->gridSize() / 2.0, 0);
        if (pins[i].direction == 2)
            dir = QPoint(-Settings::final()->gridSize() / 2.0, 0);
        if (pins[i].type == true) {
            double rad = abs(dir.x()) / 4.0;
            qpainter.setBrush(Qt::NoBrush);
            qpainter.drawEllipse(
                        (QPointF(pins[i].m_point * Settings::final()->gridSize())) +
                        ((dir / 4.0) * 3.0),
                        rad, rad);
            dir /= 2.0;
        }
        if(!plain)
        {
            qpainter.drawLine(pins[i].m_point * Settings::final()->gridSize(),
                              (pins[i].m_point * Settings::final()->gridSize()) + dir);
            pen.setColor(Qt::black);
            pen.setStyle(Qt::DotLine);
            pen.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
            bool err=false;
            if (pins[i].direction == 2) {
                if (getState(i))
                    pen.setColor(Qt::red);
            } else {
                if (pins[i].state==2)
                {
                    err=true;
                    pen.setColor(QColor::fromRgbF(1,0.5,0));
                }
                else if (pins[i].state)
                    pen.setColor(Qt::red);
            }
        }
        qpainter.setPen(pen);
        qpainter.drawLine(pins[i].m_point * Settings::final()->gridSize(),
                          (pins[i].m_point * Settings::final()->gridSize()) + dir);
        // qDebug()<<(pins[i].point*10)<<pins[i].direction<<width;
    }





    return ret;
}

Block *Block::clone() {
    Block *nblk = new Block;
    nblk->mainRef=mainRef;
    nblk->L=L;
    init(nblk);
    lua_getglobal(L,"state");
    nblk->state=luaL_ref(L,LUA_REGISTRYINDEX);
    return nblk;
}

void Block::onclick(QPointF where) {
    lua_rawgeti(L,LUA_REGISTRYINDEX,state);
    lua_setglobal(L,"state");

    lua_getglobal(L, "onclick");
    if (!lua_isnil(L, -1)) {
        lua_pushnumber(L, where.x());
        lua_pushnumber(L, where.y());
        if (lua_pcall(L, 2, 0, 0) == LUA_OK) {
            lua_getglobal(L,"state");
            lua_rawseti(L,LUA_REGISTRYINDEX,state);
        } else
            qDebug() << "ERR4" << lua_tostring(L, -1)<<name;
    }
    lua_getglobal(L,"state");
    lua_rawseti(L,LUA_REGISTRYINDEX,state);
}

void Block::onpress(QPointF where) {
    lua_rawgeti(L,LUA_REGISTRYINDEX,state);
    lua_setglobal(L,"state");
    lua_getglobal(L, "onpress");
    if (!lua_isnil(L, -1)) {
        lua_pushnumber(L, where.x());
        lua_pushnumber(L, where.y());
        if (lua_pcall(L, 2, 0, 0) == LUA_OK) {
            lua_getglobal(L,"state");
            lua_rawseti(L,LUA_REGISTRYINDEX,state);
        } else
            qDebug() << "ERR4" << lua_tostring(L, -1)<<name;
    }
    lua_getglobal(L,"state");
    lua_rawseti(L,LUA_REGISTRYINDEX,state);
}
void Block::keyPress(int pos) {
    lua_rawgeti(L,LUA_REGISTRYINDEX,state);
    lua_setglobal(L,"state");
    lua_getglobal(L, "onkey");
    if (!lua_isnil(L, -1)) {
        lua_pushinteger(L, pos);
        if (lua_pcall(L, 1, 0, 0) == LUA_OK) {
            lua_getglobal(L,"state");
            lua_rawseti(L,LUA_REGISTRYINDEX,state);
        } else
            qDebug() << "ERR4" << lua_tostring(L, -1)<<name;
    }
    lua_getglobal(L,"state");
    lua_rawseti(L,LUA_REGISTRYINDEX,state);
}
void Block::keyPressNorm(QString key) {
    qDebug()<<"Sending Key to"<<name;
    lua_rawgeti(L,LUA_REGISTRYINDEX,state);
    qDebug()<<"1"<<name<<key;
    lua_setglobal(L,"state");
    qDebug()<<"2"<<name<<key;
    lua_getglobal(L, "keyPress");
    qDebug()<<"3"<<name<<key;
    if (!lua_isnil(L, -1)) {
        qDebug()<<"4"<<name<<key;
        lua_pushstring(L, key.toUtf8().data());
        qDebug()<<"5"<<name<<key;
        if (lua_pcall(L, 1, 0, 0) == LUA_OK) {
            qDebug()<<"6"<<name<<key;
            qDebug()<<"Successful send"<<key;
            qDebug()<<"7"<<name<<key;
            lua_getglobal(L,"state");
            qDebug()<<"8"<<name<<key;
            lua_rawseti(L,LUA_REGISTRYINDEX,state);
            qDebug()<<"9"<<name<<key;
        } else
            qDebug() << "ERR4" << lua_tostring(L, -1)<<name;
        qDebug()<<"10"<<name<<key;
    }
    else
        qDebug()<<"Doesn't handle Key Press"<<name;
    qDebug()<<"11"<<name<<key;
    lua_getglobal(L,"state");
    lua_rawseti(L,LUA_REGISTRYINDEX,state);
}

void Block::onrelease(QPointF where) {
    lua_rawgeti(L,LUA_REGISTRYINDEX,state);
    lua_setglobal(L,"state");
    lua_getglobal(L, "onrelease");
    if (!lua_isnil(L, -1)) {
        lua_pushnumber(L, where.x());
        lua_pushnumber(L, where.y());
        if (lua_pcall(L, 2, 0, 0) == LUA_OK) {
            lua_getglobal(L,"state");
            lua_rawseti(L,LUA_REGISTRYINDEX,state);
        } else
            qDebug() << "ERR4" << lua_tostring(L, -1)<<name;
    }
    lua_getglobal(L,"state");
    lua_rawseti(L,LUA_REGISTRYINDEX,state);
}

void Block::execContext(int idx) {
    lua_rawgeti(L,LUA_REGISTRYINDEX,state);
    lua_setglobal(L,"state");
    lua_geti(L,LUA_REGISTRYINDEX,idx);
    if (!lua_isnil(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
            lua_getglobal(L,"state");
            lua_rawseti(L,LUA_REGISTRYINDEX,state);
        } else
            qDebug() << "ERR5" << lua_tostring(L, -1)<<name;
    }
    lua_getglobal(L,"state");
    lua_rawseti(L,LUA_REGISTRYINDEX,state);
}

bool Block::getState(int pin) {
    if(!useFake)
    {
        lua_rawgeti(L,LUA_REGISTRYINDEX,state);
        lua_setglobal(L,"state");
        pushGlobal(L);
        lua_getglobal(L, "getState");
        if (!lua_isnil(L, -1)) {
            lua_pushinteger(L, pin + 1);
            if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
                lua_getglobal(L,"state");
                lua_rawseti(L,LUA_REGISTRYINDEX,state);
                return lua_toboolean(L, -1);
            } else
                qDebug() << "ERR4" << lua_tostring(L, -1)<<name;
        } else
            qDebug() << "Warning: "
                     << "Missing State at" << name;
        lua_getglobal(L,"state");
        lua_rawseti(L,LUA_REGISTRYINDEX,state);
    }
    else
        return fake[pin];
    return false;
}

void Block::pushGlobal(lua_State *L) {
    lua_newtable(L);
    for (int i = 0; i < pins.length(); i++) {
        bool cur = pins[i].state;
        if (pins[i].type)
            cur = !cur;
        if(pins[i].state==2)
            cur=false;
        lua_pushboolean(L, cur);
        lua_seti(L, -2, i + 1);
    }
    lua_setglobal(L, "pins");
}

QPointF Block::pointAt(QPolygonF spline, double pos, bool cyclic) {
    if (!cyclic) {
        while (spline.length() > 1) {
            QPolygonF nspline;
            for (int i = 1; i < spline.length(); i++) {
                nspline.append(QLineF(spline[i - 1], spline[i]).pointAt(pos));
            }
            spline = nspline;
        }
    } else {
        while (spline.length() > 1) {
            QPolygonF nspline;
            for (int i = 0; i < spline.length(); i++) {
                nspline.append(
                            QLineF(spline[i], spline[(i + 1) % spline.length()]).pointAt(pos));
            }
            spline = nspline;
        }
    }
    return spline.first();
}

Block::~Block()
{
    luaL_unref(L,LUA_REGISTRYINDEX,state);
    //lua_close(L);
}

void Block::init(Block *blk)
{
    blk->valid=true;
    blk->checkable=false;
    lua_State*L=blk->L;
    lua_rawgeti(L,LUA_REGISTRYINDEX,blk->mainRef);
    if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
        if(!lua_istable(L,-1))
        {
            blk->valid=false;
            return;
        }
        lua_getfield(L, -1, "pins");
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            lua_geti(L, -1, 1);
            int x = lua_tointeger(L, -1);
            lua_pop(L, 1);
            lua_geti(L, -1, 2);
            int y = lua_tointeger(L, -1);
            lua_pop(L, 1);
            lua_geti(L, -1, 3);
            QString type = lua_tostring(L, -1);
            lua_pop(L, 1);
            if (type == "INPUT") {
                pin_t pin;
                pin.m_point = QPoint(x, y);
                pin.direction = 0;
                pin.state = false;
                pin.type = false;
                blk->pins.append(pin);
            }
            if (type == "OUTPUT") {
                pin_t pin;
                pin.m_point = QPoint(x, y);
                pin.direction = 2;
                pin.type = false;
                blk->pins.append(pin);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
        lua_getfield(L, -1, "context");
        if(lua_istable(L,-1))
        {
            lua_pushnil(L);
            while (lua_next(L, -2)) {
                QString key=lua_tostring(L,-2);
                int pin=luaL_ref(L,LUA_REGISTRYINDEX);
                blk->contextMenu.insert(key,pin);
            }
        }
        lua_pop(L, 1);
        lua_getfield(L, -1, "alt");
        if(lua_istable(L,-1))
        {
            lua_pushnil(L);
            while (lua_next(L, -2)) {
                blk->alt.append(lua_tostring(L,-1));
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);
        lua_getfield(L, -1, "width");
        blk->width = lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, -1, "checkable");
        if(!lua_isnil(L,-1))
        {
            blk->checkable=lua_toboolean(L,-1);
        }
        lua_pop(L, 1);
        lua_getfield(L, -1, "height");
        blk->height = lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, -1, "description");
        if(lua_isstring(L,-1))
            blk->description = QByteArray(lua_tostring(L, -1));
        lua_pop(L, 1);
        lua_getfield(L, -1, "name");
        blk->name = QByteArray(lua_tostring(L, -1));
        lua_pop(L, 1);
        lua_getfield(L, -1, "category");
        blk->category = QByteArray(lua_tostring(L, -1));
        lua_pop(L, 1);
        lua_pop(L, 1);
        lua_getglobal(L,"state");
        blk->state=luaL_ref(L,LUA_REGISTRYINDEX);
    } else
        qDebug() << "ERR1" << lua_tostring(L, -1);
}

void Block::fileChanged(const QString &path)
{
    //load(path);
}
