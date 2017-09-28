#include "extensionblock.h"
#include "settings.h"
#include "painter.h"
#include <QTime>
#include <QFile>

ExtensionBlock::ExtensionBlock(QObject *parent) : Block(parent)
{
}

QPicture ExtensionBlock::draw()
{
    if(lua_gettop(L)>0)
    {
        qDebug()<<lua_gettop(L);
        qDebug()<<"TYPE"<<lua_typename(L,lua_type(L,lua_gettop(L)));
        if(lua_type(L,lua_gettop(L))==LUA_TSTRING)
            qDebug()<<lua_tostring(L,lua_gettop(L));
        qDebug()<<"TOP ERROR";
    }
    QPicture ret;
    ret.setBoundingRect(QRect(0,0,(width * Settings::final()->gridSize()) +
                              Settings::final()->gridSize() + 1,
                              (height * Settings::final()->gridSize()) + 1 +
                              Settings::final()->gridSize()));
    if(!valid)
        return ret;
    QPainter qpainter(&ret);
    qpainter.setPen(Qt::black);
    pushGlobal(L);
    Painter painter(&qpainter);
    lua_pushinteger(L, QTime::currentTime().msecsSinceStartOfDay());
    lua_setglobal(L, "time");


    lua_rawgeti(L,LUA_REGISTRYINDEX, painti);
    qpainter.setRenderHint(QPainter::Antialiasing, true);
    painter.pushMe(L);
    lua_pushstring(L,color.name().toUtf8().data());
    lua_call(L, 2, 0);
    //if (lua_call(L, 2, 0, 0) == LUA_OK) {*/
    qpainter.setRenderHint(QPainter::Antialiasing, false);
    /*} else
    {
        qDebug() << "ERR3" << lua_tostring(L, -1);
        valid=false;
        return ret;
    }*/
    qpainter.setPen(Qt::black);
    qpainter.drawPicture(0,0,Block::draw());
    lua_gc(L,LUA_GCCOLLECT,0);
    return ret;
}

Block *ExtensionBlock::clone() {
    ExtensionBlock *nblk = new ExtensionBlock;
    nblk->mainRef=mainRef;
    nblk->L=L;
    init(nblk);
    return nblk;
}

void ExtensionBlock::onclick(QPointF where) {
    if(!valid)
        return;

    lua_rawgeti(L,LUA_REGISTRYINDEX,onclicki);
    if (!lua_isnil(L, -1)) {
        lua_pushnumber(L, where.x());
        lua_pushnumber(L, where.y());
        lua_call(L, 2, 0);
    }
    else
        lua_pop(L,1);
}

void ExtensionBlock::onpress(QPointF where) {
    if(!valid)
        return;
    lua_rawgeti(L,LUA_REGISTRYINDEX,onpressi);
    if (!lua_isnil(L, -1)) {
        lua_pushnumber(L, where.x());
        lua_pushnumber(L, where.y());
        lua_call(L, 2, 0);
    }
    else
        lua_pop(L,1);
}
void ExtensionBlock::keyPress(int pos) {
    if(!valid)
        return;
    lua_rawgeti(L,LUA_REGISTRYINDEX,onkeyi);
    if (!lua_isnil(L, -1)) {
        lua_pushinteger(L, pos);
        lua_call(L, 1, 0);
    }
    else
        lua_pop(L,1);
}
void ExtensionBlock::keyPressNorm(QString key) {
    if(!valid)
        return;
    qDebug()<<"2"<<name<<key;
    lua_rawgeti(L,LUA_REGISTRYINDEX,onkeyPressi);
    qDebug()<<"3"<<name<<key;
    if (!lua_isnil(L, -1)) {
        qDebug()<<"4"<<name<<key;
        lua_pushstring(L, key.toUtf8().data());
        qDebug()<<"5"<<name<<key;
        lua_call(L, 1, 0);
    }
    else
    {
        qDebug()<<"Doesn't handle Key Press"<<name;
        lua_pop(L,1);
    }
}

void ExtensionBlock::onrelease(QPointF where) {
    if(!valid)
        return;
    lua_rawgeti(L,LUA_REGISTRYINDEX,onreleasei);
    if (!lua_isnil(L, -1)) {
        lua_pushnumber(L, where.x());
        lua_pushnumber(L, where.y());
        lua_call(L, 2, 0);
    }
    else
        lua_pop(L,1);
}

void ExtensionBlock::execContext(int idx) {
    if(!valid)
        return;
    lua_rawgeti(L,LUA_REGISTRYINDEX,idx);
    if (!lua_isnil(L, -1)) {
        lua_call(L, 0, 0);
    }
    else
        lua_pop(L,1);
}

bool ExtensionBlock::getState(int pin) {
    if(!valid)
        return false;
    if(!useFake)
    {
        pushGlobal(L);
        lua_rawgeti(L,LUA_REGISTRYINDEX,pini);
        if (!lua_isnil(L, -1)) {
            lua_pushinteger(L, pin + 1);
            lua_call(L, 1, 1);
            bool ret=lua_toboolean(L, -1);
            lua_pop(L,1);
            if(pins[pin].type)
                ret=!ret;
            return ret;
        } else
            lua_pop(L,1);
        /*  qDebug() << "Warning: "
                     << "Missing State at" << name;*/
    }
    else
        return fake[pin];
    lua_gc(L,LUA_GCCOLLECT,0);
    return false;
}

void ExtensionBlock::pushGlobal(lua_State *L) {
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

QPointF ExtensionBlock::pointAt(QPolygonF spline, double pos, bool cyclic) {
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

ExtensionBlock::~ExtensionBlock()
{
    luaL_unref(L,LUA_REGISTRYINDEX,state);
    //lua_close(L);
}
void ExtensionBlock::refresh()
{
}

void ExtensionBlock::init(ExtensionBlock *blk)
{
    blk->pins.clear();
    blk->contextMenu.clear();
    blk->alt.clear();
    blk->valid=true;
    blk->checkable=false;
    lua_State*L=blk->L;
    lua_rawgeti(L,LUA_REGISTRYINDEX,blk->mainRef);
    lua_call(L, 0, 1);
    if(!lua_istable(L,-1))
    {
        blk->valid=false;
        qDebug()<<lua_typename(L,lua_type(L,-1));
        exit(1);
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
    lua_getfield(L, -1, "paintEvent");
    blk->painti=luaL_ref(L,LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "getState");
    blk->pini=luaL_ref(L,LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "onpress");
    blk->onpressi=luaL_ref(L,LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "onrelease");
    blk->onreleasei=luaL_ref(L,LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "onclick");
    blk->onclicki=luaL_ref(L,LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "keyPress");
    blk->onkeyPressi=luaL_ref(L,LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "onkey");
    blk->onkeyi=luaL_ref(L,LUA_REGISTRYINDEX);
    lua_pop(L, 1);
}

void ExtensionBlock::simulate()
{

}
