#include "luahttpserver.h"

LuaHttpServer::LuaHttpServer(QObject *parent) : QObject(parent)
{
    server.listen(QHostAddress::Any);
}

void LuaHttpServer::push(lua_State *L)
{
    lua_newtable(L);
    lua_pushcfunction(L,llisten);
    lua_setfield(L,-2,"listen");
}

int LuaHttpServer::llisten(lua_State *L)
{
    luaL_newmetatable(L,"http_server");
    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");
    lua_pushcfunction(L,lwait);
    lua_setfield(L,-2,"wait");
    lua_pushcfunction(L,lport);
    lua_setfield(L,-2,"port");
    lua_pop(L,1);
    void* ptr=lua_newuserdata(L,8);
    LuaHttpServer* srv=new(ptr) LuaHttpServer;
    luaL_setmetatable(L,"http_server");
    return 1;
}

int LuaHttpServer::lwait(lua_State *L)
{
    LuaHttpServer* server=(LuaHttpServer*)luaL_checkudata(L,1,"http_server");
    luaL_checktype(L,2,LUA_TFUNCTION);
    if(!server->server.hasPendingConnections())
    {
        QEventLoop loop;
        connect(&server->server,SIGNAL(newConnection()),&loop,SLOT(quit()));
        loop.exec();
    }
    QTcpSocket* socket=server->server.nextPendingConnection();
    QString data;
    do {
        QEventLoop loop;
        connect(socket,SIGNAL(readyRead()),&loop,SLOT(quit()));
        loop.exec();
        data.append(socket->read(socket->bytesAvailable()));
    } while(!data.contains("\r\n\r\n"));
    lua_pushstring(L,data.toUtf8().data());
    lua_call(L,1,1);
    data=lua_tostring(L,-1);
    socket->write("HTTP/1.1 200 OK\r\n");
    socket->write("Content-type: text/plain\r\n");
    socket->write("\r\n");
    socket->write(data.toUtf8());
    socket->close();
    return 0;
}
int LuaHttpServer::lport(lua_State *L)
{
    LuaHttpServer* server=(LuaHttpServer*)luaL_checkudata(L,1,"http_server");
    lua_pushinteger(L,server->server.serverPort());
    return 1;
}
