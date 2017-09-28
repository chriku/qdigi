#ifndef BLOCKMODULE_H
#define BLOCKMODULE_H

#include "module.h"

class BlockModule : public Module
{
    Q_OBJECT
public:
    explicit BlockModule(QObject *parent = nullptr);
    QString name();
    void push(lua_State *L);
signals:

public slots:
};

#endif // BLOCKMODULE_H
