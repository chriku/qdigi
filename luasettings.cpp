#include "luasettings.h"
#include "settings.h"

LuaSettings::LuaSettings(QObject *parent) : QObject(parent)
{

}

void LuaSettings::push(lua_State *L)
{
    lua_newtable(L);
    lua_pushcfunction(L,lget);
    lua_setfield(L,-2,"get");
    lua_pushcfunction(L,lset);
    lua_setfield(L,-2,"set");
}

int LuaSettings::lset(lua_State *L)
{
    QString key=luaL_checkstring(L,1);
    QString value=luaL_checkstring(L,2);
    Settings::final()->setLua(key,value);
    return 0;
}

int LuaSettings::lget(lua_State *L)
{
    QString key=luaL_checkstring(L,1);
    QString ret=Settings::final()->getLua(key);
    lua_pushstring(L,ret.toUtf8().data());
    return 1;
}
