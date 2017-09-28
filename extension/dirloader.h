#ifndef DIRLOADER_H
#define DIRLOADER_H

#include <QObject>
#include "extensionloader.h"

class DirLoader : public ExtensionLoader
{
    Q_OBJECT
public:
    explicit DirLoader(QString path,QObject *parent = nullptr);
    bool valid();
    bool hasFiles();
    QJsonObject manifest();
    QByteArray getFile(QString name);
    QString path;
signals:

public slots:
};

#endif // DIRLOADER_H
