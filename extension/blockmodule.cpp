#include "blockmodule.h"
#include "items/extensionblock.h"
#include "blocklist.h"

BlockModule::BlockModule(QObject *parent) : Module(parent)
{

}

QString BlockModule::name()
{
    return "block";
}

static int ladd(lua_State*L)
{
    ExtensionBlock* e=new ExtensionBlock;
    e->L=L;
    lua_pushvalue(e->L,1);
    qDebug()<<lua_typename(e->L,lua_type(L,-1));
    e->mainRef=luaL_ref(e->L,LUA_REGISTRYINDEX);
    qDebug()<<e->mainRef;
    e->init(e);
    qDebug()<<lua_typename(e->L,lua_type(L,-1));
    BlockList::blocks.append(e);
    return 0;
}

void BlockModule::push(lua_State *L)
{
    lua_newtable(L);
    lua_pushcfunction(L,ladd);
    lua_setfield(L,-2,"add");
    lua_setglobal(L,"block");
}
