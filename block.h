#ifndef BLOCK_H
#define BLOCK_H
#include <QFileSystemWatcher>
#include <QPainter>
#include <QObject>
#include <QPicture>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <QPoint>
struct pin_t {
    QPoint point;
    int direction;
    int state;
    bool type;
    QColor color;
    pin_t()
    {
        state=false;
        color=Qt::black;
    }
};

class Block : public QObject
{
    Q_OBJECT
public:
    bool useFake;
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
bool getState(int pin);
static void init(Block* blk);
~Block();
QFileSystemWatcher watcher;

signals:

public slots:
void fileChanged(const QString &path);
};

#endif // BLOCK_H
