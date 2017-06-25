#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include <QObject>
#include "luablock.h"
class BlockList : public QObject
{
    Q_OBJECT
public:
    explicit BlockList(QObject *parent = 0);
static QList<Block*> blocks;
static Block* newBlock(QString name);
signals:

public slots:
};

#endif // BLOCKLIST_H
