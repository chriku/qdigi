#ifndef BLOCK_H
#define BLOCK_H
#include <QFileSystemWatcher>
#include <QPainter>
#include <QObject>
#include <QPicture>
#include <QDebug>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <QMap>
#include <QPoint>
class Block;
class block_t;
class pin_t;

class Block : public QObject
{
    Q_OBJECT
public:
    bool useFake;
    QMap<QString,int> contextMenu;
    QList<bool> fake;
    QString description;
    QStringList alt;
    explicit Block(QObject *parent = 0);
    void load(QString fileName);
    lua_State*L;
    int state;
    void keyPress(int pos);
    int mainRef;
    QList<pin_t> pins;
    void pushGlobal(lua_State*L);
    QPointF pointAt(QPolygonF spline, double pos, bool cyclic);
    QString category;
QPicture drawBlock(QColor color,bool plain=false);
QString name;
int width;
int height;
QString fileName;
Block* clone();
void onclick(QPointF where);
void onpress(QPointF where);
void onrelease(QPointF where);
void execContext(int idx);
bool getState(int pin);
static void init(Block* blk);
~Block();
QFileSystemWatcher watcher;

signals:

public slots:
void fileChanged(const QString &path);
};
class block_t {
public:
    Block* block;
    QColor color;
    block_t()
    {
        color=Qt::black;
    }
    QPoint m_pos;
    QRectF rect()
    {
        QRectF rect(m_pos.x()+0.5,m_pos.y()+0.5,block->width,block->height);
        return rect;
    }
    QRectF pinsRect()
    {
        QRectF rect(m_pos.x(),m_pos.y(),block->width+1,block->height+1.0);
        return rect;
    }

    QPointF unmap(QPointF p)
    {
        return p-m_pos;
    }
};
class pin_t {
public:
    QPoint pos(){
        return parent->m_pos+m_point;
    }

    int direction;
    int state;
    bool type;
    QColor color;
    block_t* parent;
    pin_t()
    {
        state=false;
        color=Qt::black;
    }
    QPoint m_point;
};
#endif // BLOCK_H
