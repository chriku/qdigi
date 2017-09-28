#ifndef ZIPLOADER_H
#define ZIPLOADER_H

#include <QObject>
#include "extensionloader.h"

class ZipLoader : public ExtensionLoader
{
    Q_OBJECT
public:
    explicit ZipLoader(QString path,QObject *parent = nullptr);
    bool valid();
    bool hasFiles();
    QJsonObject manifest();
    QByteArray getFile(QString name);
signals:

public slots:
};

#endif // ZIPLOADER_H
