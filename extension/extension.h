#ifndef EXTENSION_H
#define EXTENSION_H

#include <QObject>
#include "extensionloader.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class Extension : public QObject
{
    Q_OBJECT
public:
    static QList<Extension*> extensions;
    explicit Extension(QString path,QObject *parent = nullptr);
    QString pathName;
    ExtensionLoader*loader;
    lua_State*L;
signals:

public slots:
};

#endif // EXTENSION_H
