#include "extension.h"
#include <QDebug>
#include "module.h"
#include <QJsonArray>

QList<Extension*> Extension::extensions;
static int lprint(lua_State*L)
{
    QDebug debug=qDebug();
    for(int i=1;i<=lua_gettop(L);i++)
    {
        debug<<luaL_tolstring(L,i,NULL);
        lua_pop(L,1);
    }
    return 0;
}
Extension::Extension(QString path, QObject *parent) : QObject(parent)
{
    loader=ExtensionLoader::getLoader(path);
    if(loader==NULL)
        return;
    if(!loader->valid())
        return;
    QJsonObject manifest=loader->manifest();
    if(manifest["version"].toInt(-1)!=0)
        return;
    qDebug()<<"Loading extension"<<manifest["name"].toString(path);
    L=luaL_newstate();
    lua_register(L,"print",lprint);
    QJsonArray permissions=manifest["permissions"].toArray();
    for(int i=0;i<permissions.size();i++)
    {
        Module* mod=Module::getModule(permissions[i].toString());
        if(mod!=NULL)
        {
            mod->push(L);
        }
        else
            return;
    }
    if(luaL_loadstring(L,loader->getFile(manifest["main"].toString()).data())!=LUA_OK)
    {
        qDebug()<<lua_tostring(L,-1);
        return;
    }
    lua_call(L,0,0);
}
