#include "luahttp.h"
#include <QtNetwork>

extern QNetworkAccessManager manager;

LuaHttp::LuaHttp(QObject *parent) : QObject(parent)
{

}

void LuaHttp::push(lua_State *L)
{
    lua_newtable(L);
    lua_pushcfunction(L,lget);
    lua_setfield(L,-2,"get");
    lua_pushcfunction(L,lpost);
    lua_setfield(L,-2,"post");
}

int LuaHttp::lget(lua_State *L)
{
    bool hasHeader=false;
    if(lua_istable(L,3))
        hasHeader=true;
    QUrl url(luaL_checkstring(L,1));
    luaL_checktype(L,2,LUA_TTABLE);
    lua_pushvalue(L,2);
    QUrlQuery query=load(L);
    QNetworkRequest req;
    if(hasHeader)
    {
        lua_pushnil(L);
        while(lua_next(L,3))
        {
            QString key=lua_tostring(L,-2);
            QString value=lua_tostring(L,-1);
            req.setRawHeader(key.toUtf8(),value.toUtf8());
            lua_pop(L,1);
        }
        lua_pop(L,1);
    }
    url.setQuery(query);
    req.setUrl(QUrl(url));
    QNetworkReply* rep=manager.get(req);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    QByteArray data=rep->readAll();
    lua_pushstring(L,data.data());
    return 1;
}

int LuaHttp::lpost(lua_State *L)
{
    bool hasHeader=false;
    if(lua_istable(L,3))
        hasHeader=true;
    QUrl url(luaL_checkstring(L,1));
    QByteArray data;
    if(lua_istable(L,2))
    {
        lua_pushvalue(L,2);
        data=load(L).toString().toUtf8();
    }
    else
    {
        luaL_checkstring(L,2);
        const char *d;
        size_t len;
        d=lua_tolstring(L,2,&len);
        data=QByteArray(d,len);
    }
    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    if(hasHeader)
    {
        lua_pushnil(L);
        while(lua_next(L,3))
        {
            QString key=lua_tostring(L,-2);
            QString value=lua_tostring(L,-1);
            req.setRawHeader(key.toUtf8(),value.toUtf8());
            lua_pop(L,1);
        }
        lua_pop(L,1);
    }
    req.setUrl(QUrl(url));
    req.setHeader(QNetworkRequest::ContentLengthHeader,QString::number(data.length()));
    qDebug()<<req.rawHeaderList()<<req.url()<<data;
    QNetworkReply* rep=manager.post(req,data);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    QByteArray odata=rep->readAll();
    lua_pushstring(L,odata.data());
    return 1;
}

QUrlQuery LuaHttp::load(lua_State *L)
{
    QUrlQuery query;
    lua_pushnil(L);
    while(lua_next(L,-2))
    {
        QString key=lua_tostring(L,-2);
        QString value=lua_tostring(L,-1);
        query.addQueryItem(key,value);
        lua_pop(L,1);
    }
    lua_pop(L,1);
    return query;
}
