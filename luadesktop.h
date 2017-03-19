#ifndef LUADESKTOP_H
#define LUADESKTOP_H

#include <QObject>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
class LuaDesktop : public QObject
{
    Q_OBJECT
public:
    explicit LuaDesktop(QObject *parent = 0);
    static void push(lua_State*L);
    static int lopenurl(lua_State*L);

signals:

public slots:
};

#endif // LUADESKTOP_H
