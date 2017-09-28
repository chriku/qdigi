#ifndef EXTENSIONLOADER_H
#define EXTENSIONLOADER_H

#include <QObject>
#include <QJsonObject>

class ExtensionLoader : public QObject
{
    Q_OBJECT
public:
    explicit ExtensionLoader(QObject *parent = nullptr);
    static ExtensionLoader* getLoader(QString path);
    virtual bool valid()=0;
    virtual bool hasFiles()=0;
    virtual QJsonObject manifest()=0;
    virtual QByteArray getFile(QString name)=0;
signals:

public slots:
};

#endif // EXTENSIONLOADER_H
