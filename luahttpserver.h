#ifndef LUAHTTPSERVER_H
#define LUAHTTPSERVER_H

#include <QObject>
#include <QtNetwork>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
class LuaHttpServer : public QObject
{
    Q_OBJECT
public:
    QTcpServer server;
    explicit LuaHttpServer(QObject *parent = 0);
    static void push(lua_State*L);
    static int llisten(lua_State*L);
    static int lwait(lua_State*L);
    static int lport(lua_State*L);
signals:

public slots:
};

#endif // LUAHTTPSERVER_H
