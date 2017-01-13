#ifndef PAINTER_H
#define PAINTER_H

#include <QObject>
#include <QPainter>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
class Painter : public QObject
{
    Q_OBJECT
public:
    explicit Painter(QPainter*painter,QObject *parent = 0);
    static void drawSpline(QPainter*painter, QPolygonF spline, bool cyclic);
void pushMe(lua_State*L);
QPainter* painter;
static int lprint(lua_State*L);
static int lsetPen(lua_State*L);
static int lsetBrush(lua_State*L);
static int ldrawRect(lua_State*L);
static int ldrawLine(lua_State*L);
static int ldrawEllipse(lua_State*L);
static int ldrawSpline(lua_State*L);
static int ldrawText(lua_State*L);
static int lsetFont(lua_State*L);
signals:

public slots:
};

#endif // PAINTER_H
