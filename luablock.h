#ifndef LUABLOCK_H
#define LUABLOCK_H

#include "block.h"

class LuaBlock : public Block
{
    Q_OBJECT
public:
    explicit LuaBlock(QObject *parent = 0);
    void load(QString fileName);
    lua_State*L;
    int state;
    int mainRef;
    void pushGlobal(lua_State*L);
    QPointF pointAt(QPolygonF spline, double pos, bool cyclic);
    QString fileName;
    static void init(Block *blk2);
    QPicture draw();
    Block*clone();
    void onclick(QPointF where);
    void onpress(QPointF where);
    void onrelease(QPointF where);
    void execContext(int idx);
    bool getState(int pin);
    void keyPressNorm(QString key);
    void keyPress(int pos);
    ~LuaBlock();
    void simulate();

signals:

public slots:
};

#endif // LUABLOCK_H
