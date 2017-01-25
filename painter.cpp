#include "painter.h"
#include <QDebug>
#include "settings.h"

Painter::Painter(QPainter *painter, QObject *parent) : QObject(parent)
{
    Painter::painter=painter;
    QFont font("Arial",Settings::final()->gridSize()*0.5);
painter->setFont(font);
}
void Painter::drawSpline(QPainter *painter, QPolygonF spline,bool cyclic)
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

void Painter::pushMe(lua_State *L)
{
    lua_pushlightuserdata(L,painter);
    lua_newtable(L);
    lua_pushcfunction(L,lprint);
    lua_setfield(L,-2,"__tostring");
    lua_newtable(L);
    lua_pushcfunction(L,lsetPen);
    lua_setfield(L,-2,"setPen");
    lua_pushcfunction(L,lsetBrush);
    lua_setfield(L,-2,"setBrush");
    lua_pushcfunction(L,ldrawRect);
    lua_setfield(L,-2,"drawRect");
    lua_pushcfunction(L,ldrawLine);
    lua_setfield(L,-2,"drawLine");
    lua_pushcfunction(L,ldrawEllipse);
    lua_setfield(L,-2,"drawEllipse");
    lua_pushcfunction(L,ldrawSpline);
    lua_setfield(L,-2,"drawSpline");
    lua_pushcfunction(L,ldrawText);
    lua_setfield(L,-2,"drawText");
    lua_pushcfunction(L,lsetFont);
    lua_setfield(L,-2,"setFont");
    lua_setfield(L,-2,"__index");
    lua_setmetatable(L,-2);
}

int Painter::lprint(lua_State *L)
{
    lua_pushstring(L,"painter");
    return 1;
}

int Painter::lsetPen(lua_State *L)
{
    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    QPen pen(Qt::NoPen);
    if(lua_isstring(L,2))
    {
        QColor color(lua_tostring(L,2));
        pen=QPen(color);
        pen.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
    }
    if(lua_isnumber(L,3))
        pen.setWidth(lua_tonumber(L,3)*Settings::final()->gridSize());
    painter->setPen(pen);
    return 0;
}

int Painter::lsetFont(lua_State *L)
{

    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    QFont font("Arial",Settings::final()->gridSize());
    font.setKerning(true);
    if(lua_isstring(L,2))
        if(QString(lua_isstring(L,2)).length()>0)
        font.setFamily(lua_tostring(L,2));
    if(lua_isnumber(L,3))
        font.setPixelSize(lua_tonumber(L,3)*Settings::final()->gridSize());
    painter->setFont(font);
    return 0;
}

int Painter::lsetBrush(lua_State *L)
{
    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    QBrush brush(Qt::NoBrush);
    if(lua_isstring(L,2))
    {
        QColor color(lua_tostring(L,2));
        brush=QBrush(color);
    }
    painter->setBrush(brush);
    return 0;
}

int Painter::ldrawRect(lua_State *L)
{
    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    double grid=Settings::final()->gridSize();
    double x=luaL_checknumber(L,2);
    double y=luaL_checknumber(L,3);
    double w=luaL_checknumber(L,4);
    double h=luaL_checknumber(L,5);
    x*=grid;
    y*=grid;
    w*=grid;
    h*=grid;
    painter->drawRect(QRectF(x,y,w,h));
    return 0;
}
int Painter::ldrawText(lua_State *L)
{
    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    double grid=Settings::final()->gridSize();
    size_t len=0;
    luaL_checkstring(L,2);
    lua_tolstring(L,2,&len);
    QString text(QByteArray(lua_tolstring(L,2,&len),len));
    double x=luaL_checknumber(L,3);
    double y=luaL_checknumber(L,4);
    double w=luaL_checknumber(L,5);
    double h=luaL_checknumber(L,6);
    x*=grid;
    y*=grid;
    w*=grid;
    h*=grid;
    painter->drawText(QRectF(x,y,w,h),Qt::AlignCenter,text);
    return 0;
}

int Painter::ldrawEllipse(lua_State *L)
{
    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    double grid=Settings::final()->gridSize();
    double x=luaL_checknumber(L,2);
    double y=luaL_checknumber(L,3);
    double rx=luaL_checknumber(L,4);
    double ry=rx;
    if(lua_isnumber(L,5))
        ry=luaL_checknumber(L,5);
    x*=grid;
    y*=grid;
    rx*=grid;
    ry*=grid;
    painter->drawEllipse(QPointF(x,y),rx,ry);
    return 0;
}

int Painter::ldrawLine(lua_State *L)
{
    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    double grid=Settings::final()->gridSize();
    double x1=luaL_checknumber(L,2);
    double y1=luaL_checknumber(L,3);
    double x2=luaL_checknumber(L,4);
    double y2=luaL_checknumber(L,5);
    x1*=grid;
    y1*=grid;
    x2*=grid;
    y2*=grid;
    painter->drawLine(x1,y1,x2,y2);
    return 0;
}

int Painter::ldrawSpline(lua_State *L)
{
    luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
    QPainter*painter=(QPainter*)lua_touserdata(L,1);
    double grid=Settings::final()->gridSize();
    lua_pushnil(L);
    QPolygonF poly;
    while(lua_next(L,2))
    {
        lua_geti(L,-1,1);
        double x=lua_tonumber(L,-1);
        lua_pop(L,1);
        lua_geti(L,-1,2);
        double y=lua_tonumber(L,-1);
        lua_pop(L,1);
        lua_pop(L,1);
        x*=grid;
        y*=grid;
        poly.append(QPointF(x,y));
    }
    bool cyclic=false;
    if(lua_isboolean(L,3))
        cyclic=lua_toboolean(L,3);
    lua_pop(L,1);
    drawSpline(painter,poly,cyclic);
    return 0;
}
