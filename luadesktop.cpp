#include "luadesktop.h"
#include <QDesktopServices>
#include <QUrl>

LuaDesktop::LuaDesktop(QObject *parent) : QObject(parent)
{

}

void LuaDesktop::push(lua_State *L)
{
    lua_newtable(L);
    lua_pushcfunction(L,lopenurl);
    lua_setfield(L,-2,"openUrl");
}

int LuaDesktop::lopenurl(lua_State *L)
{
    QUrl url=QUrl(luaL_checkstring(L,1));
    QDesktopServices::openUrl(url);
    return 0;
}
