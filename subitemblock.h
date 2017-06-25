#ifndef SUBITEMBLOCK_H
#define SUBITEMBLOCK_H

#include "block.h"
#include "schematic.h"

class SubitemBlock : public Block
{
    Q_OBJECT
public:
    explicit SubitemBlock(Schematic* data,QObject *parent = 0);
Schematic* m_data;
QPicture draw();
Block*clone();
void onclick(QPointF where);
void onpress(QPointF where);
void onrelease(QPointF where);
void execContext(int idx);
bool getState(int pin);
void keyPressNorm(QString key);
void keyPress(int pos);
QList<QPair<QString,bool> > ppos;
void simulate();
signals:

public slots:
};

#endif // SUBITEMBLOCK_H
