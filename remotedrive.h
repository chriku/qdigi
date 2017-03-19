#ifndef REMOTEDRIVE_H
#define REMOTEDRIVE_H

#include <QObject>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class RemoteDrive : public QObject
{
    Q_OBJECT
public:
    explicit RemoteDrive(QString fileName,QObject *parent = 0);
QString name;
QString prefix;
bool save(QUrl where,QByteArray data);
lua_State* L;
int saveFileIdx;
QUrl saveUrl();
signals:

public slots:
};

#endif // REMOTEDRIVE_H
