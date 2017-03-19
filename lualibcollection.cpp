#include "lualibcollection.h"
#include "luahttp.h"
#include "luasettings.h"
#include "luadesktop.h"
#include "luahttpserver.h"
#include "settings.h"

LuaLibCollection::LuaLibCollection(QObject *parent) : QObject(parent)
{

}

void LuaLibCollection::push(lua_State *L)
{
    lua_getglobal(L,"package");
    lua_pushstring(L,QDir(QDir(Settings::final()->mainPath).absoluteFilePath("libs")).absoluteFilePath("?.lua").toUtf8().data());
    lua_setfield(L,-2,"path");
    lua_getglobal(L,"require");
    lua_setglobal(L,"orequire");
    lua_pushcfunction(L,lrequire);
    lua_setglobal(L,"require");
}

int LuaLibCollection::lrequire(lua_State *L)
{
    QString name=luaL_checkstring(L,1);
    if(name=="http")
    {
        LuaHttp::push(L);
        return 1;
    }
    if(name=="http.server")
    {
        LuaHttpServer::push(L);
        return 1;
    }
    if(name=="settings")
    {
        LuaSettings::push(L);
        return 1;
    }
    if(name=="desktop")
    {
        LuaDesktop::push(L);
        return 1;
    }
    lua_getglobal(L,"orequire");
    lua_pushvalue(L,1);
    lua_call(L,1,1);
    return 1;
}
