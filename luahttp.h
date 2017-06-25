#ifndef LUAHTTP_H
#define LUAHTTP_H

#include <QObject>
#include <QUrlQuery>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
class LuaHttp : public QObject
{
    Q_OBJECT
public:
    explicit LuaHttp(QObject *parent = 0);
    static void push(lua_State*L);
    static int lget(lua_State*L);
    static int lpost(lua_State*L);
    static QUrlQuery load(lua_State*L);
signals:

public slots:
};

#endif // LUAHTTP_H
