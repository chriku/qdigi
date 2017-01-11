#include "block.h"
#include <QFile>
#include "settings.h"
#include <QPixmap>
#include <QPainter>
#include <QDebug>

Block::Block(QObject *parent) : QObject(parent)
{
    L=luaL_newstate();
}

void Block::load(QString fileName)
{
    Block::fileName=fileName;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QString data=file.readAll();
    luaL_openlibs(L);
    if(luaL_loadstring(L,data.toUtf8().data())==LUA_OK)
    {
        if(lua_pcall(L,0,1,0)==LUA_OK)
        {
            lua_getfield(L,-1,"pins");
            lua_pushnil(L);
            while(lua_next(L,-2))
            {
                lua_geti(L,-1,1);
                int x=lua_tointeger(L,-1);
                lua_pop(L,1);
                lua_geti(L,-1,2);
                int y=lua_tointeger(L,-1);
                lua_pop(L,1);
                lua_geti(L,-1,3);
                QString type=lua_tostring(L,-1);
                lua_pop(L,1);
                if(type=="INPUT")
                {
                    pin_t pin;
                    pin.point=QPoint(x,y);
                    pin.direction=0;
                    pin.state=false;
                    pin.type=false;
                    pins.append(pin);
                }
                if(type=="OUTPUT")
                {
                    pin_t pin;
                    pin.point=QPoint(x,y);
                    pin.direction=2;
                    pin.type=false;
                    pins.append(pin);
                }
                lua_pop(L,1);
            }
            lua_pop(L,1);
            lua_getfield(L,-1,"width");
            width=lua_tointeger(L,-1);
            lua_pop(L,1);
            lua_getfield(L,-1,"height");
            height=lua_tointeger(L,-1);
            lua_pop(L,1);
            lua_getfield(L,-1,"name");
            name=QByteArray(lua_tostring(L,-1));
            lua_pop(L,1);
            lua_pop(L,1);
        }
        else
            qDebug()<<"ERR1"<<lua_tostring(L,-1);
    }else
        qDebug()<<"ERR2"<<lua_tostring(L,-1);
}

QPixmap Block::drawBlock()
{
    QPixmap ret((width*Settings::final()->gridSize())+Settings::final()->gridSize()+1,(height*Settings::final()->gridSize())+1+Settings::final()->gridSize());
    ret.fill(Qt::transparent);
    QPainter painter(&ret);
    painter.setPen(Qt::black);
    for(int i=0;i<pins.length();i++)
    {
        QPen pen(Qt::black);
        if(pins[i].direction==2)
        {
            if(getState(i))
                pen.setColor(Qt::red);
        }else
        {
            if(pins[i].state)
                pen.setColor(Qt::red);
        }
        pen.setWidth(3);
        painter.setPen(pen);
        QPoint dir(Settings::final()->gridSize()/2.0,0);
        if(pins[i].direction==2)
            dir=QPoint(-Settings::final()->gridSize()/2.0,0);
        if(pins[i].type==true)
        {
            double rad=abs(dir.x())/4.0;
            painter.drawEllipse((pins[i].point*Settings::final()->gridSize())+((dir/4.0)*3.0),rad,rad);
            dir/=2.0;
        }
        painter.drawLine(pins[i].point*Settings::final()->gridSize(),(pins[i].point*Settings::final()->gridSize())+dir);
        //qDebug()<<(pins[i].point*10)<<pins[i].direction<<width;
    }
    painter.setPen(Qt::black);
    pushGlobal(L);
    lua_getglobal(L,"paintEvent");
    if(lua_pcall(L,0,1,0)==LUA_OK)
    {
        int table=lua_gettop(L);
        lua_pushnil(L);
        while(lua_next(L,table))
        {
            int sub=lua_gettop(L);
            lua_pushnil(L);
            QPolygonF spline;
            bool cyclic=false;
            int cnt=0;
            while(lua_next(L,sub))
            {
                if(lua_type(L,-1)==LUA_TTABLE)
                {
                    lua_geti(L,-1,1);
                    double x=lua_tonumber(L,-1);
                    lua_pop(L, 1);
                    lua_geti(L,-1,2);
                    double y=lua_tonumber(L,-1);
                    lua_pop(L, 1);
                    QPointF point(x*Settings::final()->gridSize(),y*Settings::final()->gridSize());
                    spline.append(point);
                }
                else if((lua_type(L,-1)==LUA_TBOOLEAN)&&(cnt==0))
                {
                    cyclic=lua_toboolean(L,-1);
                }
                cnt++;
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
            QPen pen(Qt::black);
            pen.setWidth(3);
            painter.setPen(pen);
            drawSpline(&painter,spline,cyclic);
        }
        lua_pop(L, 1);
    }
    else
        qDebug()<<"ERR3"<<lua_tostring(L,-1);
    return ret;
}

Block* Block::clone()
{
    Block* nblk=new Block;
    nblk->load(Block::fileName);
    return nblk;
}

void Block::onclick(QPointF where)
{
    lua_getglobal(L,"onclick");
    if(!lua_isnil(L,-1))
    {
        lua_pushnumber(L,where.x());
        lua_pushnumber(L,where.y());
        if(lua_pcall(L,2,0,0)==LUA_OK)
        {

        }
        else
            qDebug()<<"ERR4"<<lua_tostring(L,-1);
    }
}

void Block::onpress(QPointF where)
{
    lua_getglobal(L,"onpress");
    if(!lua_isnil(L,-1))
    {
        lua_pushnumber(L,where.x());
        lua_pushnumber(L,where.y());
        if(lua_pcall(L,2,0,0)==LUA_OK)
        {

        }
        else
            qDebug()<<"ERR4"<<lua_tostring(L,-1);
    }
}

void Block::onrelease(QPointF where)
{
    lua_getglobal(L,"onrelease");
    if(!lua_isnil(L,-1))
    {
        lua_pushnumber(L,where.x());
        lua_pushnumber(L,where.y());
        if(lua_pcall(L,2,0,0)==LUA_OK)
        {

        }
        else
            qDebug()<<"ERR4"<<lua_tostring(L,-1);
    }
}

bool Block::getState(int pin)
{
    pushGlobal(L);
    lua_getglobal(L,"getState");
    if(!lua_isnil(L,-1))
    {
        lua_pushinteger(L,pin+1);
        if(lua_pcall(L,1,1,0)==LUA_OK)
        {
            return lua_toboolean(L,-1);
        }
        else
            qDebug()<<"ERR4"<<lua_tostring(L,-1);
    }
    else
        qDebug()<<"Warning: "<<"Missing State at"<<name;
    return false;
}

void Block::pushGlobal(lua_State *L)
{
    lua_newtable(L);
    for(int i=0;i<pins.length();i++)
    {
        bool cur=pins[i].state;
        if(pins[i].type)
            cur=!cur;
        lua_pushboolean(L,cur);
        lua_seti(L,-2,i+1);
    }
    lua_setglobal(L,"pins");
}

void Block::drawSpline(QPainter *painter, QPolygonF spline,bool cyclic)
{
    if(spline.length()==2)
        painter->drawLine(spline[0],spline[1]);
    else
    {
        QList<QPointF> m;
        if(!cyclic)
        {
        m.append(QPointF(0,0));
        for(int i=1;i<spline.length()-1;i++)
        {
            QPointF pn=spline[i-1];
            QPointF p0=spline[i];
            QPointF pp=spline[i+1];
            m.append(((p0-pn)/2.0)+((pp-p0)/2.0));
        }
        m.append(QPointF(0,0));
        }
        else
        {
            for(int i=0;i<spline.length();i++)
            {
                QPointF pn=spline[(i-1+spline.length())%spline.length()];
                QPointF p0=spline[i];
                QPointF pp=spline[(i+1)%spline.length()];
                m.append(((p0-pn)/2.0)+((pp-p0)/2.0));
            }
            m.append(m.first());
            spline.append(spline.first());
            spline.append(spline.first());
            spline.append(spline.first());
            spline.append(spline.first());
        }
        QPolygonF pnt;
        for(double pos=0;pos<(m.length()-1);pos+=0.01)
        {
            double i2=pos;
            double t=modf(pos,&i2);
            int i=i2;
            QPointF p0=spline[i];
            QPointF p1=spline[i+1];
            QPointF m0=m[i];
            QPointF m1=m[i+1];
            QPointF p=(((2*p0)-(2*p1)+m0+m1)*pow(t,3))+(((-3*p0)+(3*p1)-(2*m0)-m1)*pow(t,2))+(m0*t)+p0;
            pnt.append(p);
        }
        painter->drawPolyline(pnt);
    }
    /*for(int i=1;i<spline.length();i++)
        painter->drawLine(spline[i-1],spline[i]);*/
    /*double len=0;
    double lenStep=0.1;
    for(double i=0;i<1;i+=lenStep)
        len+=QLineF(pointAt(spline,i,cyclic),pointAt(spline,i+lenStep,cyclic)).length();
    double step=1.0/len;
    for(double i=0;i<1;i+=step)
    {
        painter->drawLine(pointAt(spline,i,cyclic),pointAt(spline,i+step,cyclic));
    }*/
}

QPointF Block::pointAt(QPolygonF spline, double pos,bool cyclic)
{
    if(!cyclic)
    {
        while(spline.length()>1)
        {
            QPolygonF nspline;
            for(int i=1;i<spline.length();i++)
            {
                nspline.append(QLineF(spline[i-1],spline[i]).pointAt(pos));
            }
            spline=nspline;
        }
    }else
    {
        while(spline.length()>1)
        {
            QPolygonF nspline;
            for(int i=0;i<spline.length();i++)
            {
                nspline.append(QLineF(spline[i],spline[(i+1)%spline.length()]).pointAt(pos));
            }
            spline=nspline;
        }
    }
    return spline.first();
}
