#include "remotedrive.h"
#include <QDebug>
#include <QUrl>
#include <QInputDialog>

RemoteDrive::RemoteDrive(QString fileName, QObject *parent) : QObject(parent)
{
    return;
    L=luaL_newstate();
    luaL_openlibs(L);
    saveFileIdx=-1;
    if(luaL_loadfile(L,fileName.toUtf8().data())!=LUA_OK)
    {
        qDebug()<<"ERROR "+fileName+" LOAD: "<<lua_tostring(L,-1);
        return;
    }
    if(lua_pcall(L,0,1,0)!=LUA_OK)
    {
        qDebug()<<"ERROR "+fileName+" EXECUTE: "<<lua_tostring(L,-1);
        return;
    }
    if(!lua_istable(L,-1))
    {
        qDebug()<<"ERROR "+fileName+" MISSING RETURN TABLE: "<<lua_tostring(L,-1);
        return;
    }
    lua_pushnil(L);
    while(lua_next(L,-2))
    {
        QString key=lua_tostring(L,-2);
        if(key=="name")
            name=lua_tostring(L,-1);
        if(key=="prefix")
            prefix=lua_tostring(L,-1);
        if(key=="save")
        {
            saveFileIdx=luaL_ref(L,LUA_REGISTRYINDEX);
            lua_pushnil(L);
        }
        lua_pop(L,1);
    }
    lua_pop(L,1);
    qDebug()<<"LOAD"<<fileName;
}

bool RemoteDrive::save(QUrl where, QByteArray data)
{
    return false;
    if(saveFileIdx>=0)
    {
        where.setScheme("");
        lua_geti(L,LUA_REGISTRYINDEX,saveFileIdx);
        lua_pushstring(L,where.toString().toUtf8().data());
        lua_pushlstring(L,data.data(),data.length());
        if(lua_pcall(L,2,1,0)==LUA_OK)
        {
            return lua_toboolean(L,-1);
        }
        else
        {
            qDebug()<<"ERROR "+name+" SAVE ERROR: "<<lua_tostring(L,-1);
            return false;
        }
    }
    else
    {
        qDebug()<<"ERROR "+name+" MISSING SAVE ERROR";
        return false;
    }
}

QUrl RemoteDrive::saveUrl()
{
    return QUrl();
    QString fName=QInputDialog::getText(NULL,"Dateiname","Dateiname");
    QUrl ret("prefix:"+fName);
    ret.setScheme(prefix);
    return ret;
}
