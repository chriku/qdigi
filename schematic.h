#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QObject>
#include <QJsonObject>
#include "impulselabel.h"
#include "jumplabel.h"
#include "line.h"
#include "text.h"
#include "via.h"
#include "blocklist.h"


class Schematic : public QObject
{
    Q_OBJECT
public:
    QString name;
    explicit Schematic(QObject *parent = 0);
    QList<Block*> blocks;
    QList<Line*> lines;
    QList<Text*> texts;
    QList<Via*> vias;
    QList<ImpulseLabel*> impulseLabels;
    QList<JumpLabel*> jumpLabels;
    QList<Item*> items;

    bool isBlock(Item* item);
    bool isLine(Item* item);
    bool isText(Item* item);
    bool isVia(Item* item);
    bool isImpulseLabel(Item* item);
    bool isJumpLabel(Item* item);

    QList<QPair<int,int> > getItemsForOutput(QPoint pos, QList<int> *witems);
    QList<int> getNet(QLine in);
    void loadJson(QJsonObject json);
    void loadJson(QByteArray json);
    QJsonObject exportJSON();
    void addBlock(QPoint pos, QString type);
    void check();
    void cleanUp();
    bool onLine(QLine line,QPoint point,bool proper=false);
    QList<QString> getInputs();
    QList<QPair<QString, int> > getInputList();
    QList<QString> getOutputs();
    QList<QPair<QString, int> > getOutputList();
    bool simulate(QList<Schematic *> stack=QList<Schematic*>());

signals:
void purgeSelection();
public slots:
};

#endif // SCHEMATIC_H
