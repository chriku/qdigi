#ifndef EXTENSIONBLOCK_H
#define EXTENSIONBLOCK_H

#include <QObject>
#include "block.h"

class ExtensionBlock : public Block
{
    Q_OBJECT
public:
    explicit ExtensionBlock(QObject *parent = nullptr);
    ~ExtensionBlock();
    lua_State*L;
    int state;
    int painti;
    int pini;
    int onreleasei;
    int onpressi;
    int mainRef;
    void pushGlobal(lua_State*L);
    static void init(ExtensionBlock *blk);

    void refresh();
    QPointF pointAt(QPolygonF spline, double pos, bool cyclic);

    void keyPressNorm(QString key);
    void keyPress(int pos);
    Block* clone();
    void onclick(QPointF where);
    void onpress(QPointF where);
    void onrelease(QPointF where);
    void execContext(int idx);
    bool getState(int pin);
    void simulate();
    QPicture draw();
    int onclicki;
    int onkeyi;
    int onkeyPressi;

signals:

public slots:
};

#endif // EXTENSIONBLOCK_H
