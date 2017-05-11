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
#include "item.h"
class Block;
class pin_t;

class Block : public Item
{
    Q_OBJECT
public:
    bool valid;
    bool useFake;
    QMap<QString,int> contextMenu;
    QList<bool> fake;
    QString description;
    QStringList alt;
    explicit Block(QObject *parent = 0);
    void load(QString fileName);
    lua_State*L;
    int state;
    bool checkable;
    void keyPressNorm(QString key);
    void keyPress(int pos);
    int mainRef;
    QList<pin_t> pins;
    void pushGlobal(lua_State*L);
    QPointF pointAt(QPolygonF spline, double pos, bool cyclic);
    QString category;
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
    QFileSystemWatcher* watcher;
    QRectF clickRect();
    //QRectF pinsRect();
    QPointF unmap(QPointF p);
    QPicture draw(bool values);

signals:

public slots:
    void fileChanged(const QString &path);
};
class pin_t {
public:
    QPoint pos(){
        return parent->pos+m_point;
    }

    int direction;
    int state;
    bool type;//Inverted
    QColor color;
    Block* parent;
    pin_t()
    {
        state=false;
        color=Qt::black;
    }
    QPoint m_point;
};
#endif // BLOCK_H
