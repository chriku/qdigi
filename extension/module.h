#ifndef MODULE_H
#define MODULE_H

#include <QObject>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class Module : public QObject
{
    Q_OBJECT
public:
    explicit Module(QObject *parent = nullptr);
    virtual QString name()=0;
    virtual void push(lua_State*L)=0;
    static Module* getModule(QString name);
signals:

public slots:
};

#endif // MODULE_H
