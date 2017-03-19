#ifndef LUALIBCOLLECTION_H
#define LUALIBCOLLECTION_H

#include <QObject>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
class LuaLibCollection : public QObject
{
    Q_OBJECT
public:
    explicit LuaLibCollection(QObject *parent = 0);
    static void push(lua_State*L);
    static int lrequire(lua_State*L);
signals:

public slots:
};

#endif // LUALIBCOLLECTION_H
