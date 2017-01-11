#ifndef BLOCK_H
#define BLOCK_H

#include <QPainter>
#include <QObject>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <QPoint>
struct pin_t {
    QPoint point;
    int direction;
    bool state;
    bool type;
};

class Block : public QObject
{
    Q_OBJECT
public:
    explicit Block(QObject *parent = 0);
    void load(QString fileName);
    lua_State*L;
    QList<pin_t> pins;
    void pushGlobal(lua_State*L);
    void drawSpline(QPainter*painter, QPolygonF spline, bool cyclic);
    QPointF pointAt(QPolygonF spline, double pos, bool cyclic);
QPixmap drawBlock();
QString name;
int width;
int height;
QString fileName;
Block* clone();
void onclick(QPointF where);
void onpress(QPointF where);
void onrelease(QPointF where);
bool getState(int pin);
signals:

public slots:
};

#endif // BLOCK_H
