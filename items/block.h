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
    virtual void refresh()=0;
    explicit Block(QObject *parent = 0);
    virtual void keyPressNorm(QString key)=0;
    virtual void keyPress(int pos)=0;
    virtual Block* clone()=0;
    virtual void onclick(QPointF where)=0;
    virtual void onpress(QPointF where)=0;
    virtual void onrelease(QPointF where)=0;
    virtual void execContext(int idx)=0;
    virtual bool getState(int pin)=0;
    virtual void simulate()=0;

    static QMap<QString,QList<Block*>> usedBlocks;
    bool subItem;
    QString description;
    QList<bool> fake;
    bool useFake;
    QMap<QString,int> contextMenu;
    QStringList alt;
    bool checkable;
    QString category;
    QString name;
    QList<pin_t> pins;
    int width;
    int height;
    ~Block();
    QRectF clickRect();
    QPointF unmap(QPointF p);
    QPicture draw();
    bool valid;
signals:

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
