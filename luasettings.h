#ifndef LUASETTINGS_H
#define LUASETTINGS_H

#include <QObject>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
class LuaSettings : public QObject
{
    Q_OBJECT
public:
    explicit LuaSettings(QObject *parent = 0);
    static void push(lua_State*L);
    static int lset(lua_State*L);
    static int lget(lua_State*L);
signals:

public slots:
};

#endif // LUASETTINGS_H
