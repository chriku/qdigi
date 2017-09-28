#include "dirloader.h"
#include <QJsonDocument>
#include <QDir>

DirLoader::DirLoader(QString path, QObject *parent) : ExtensionLoader(parent)
{
    DirLoader::path=path;
}

bool DirLoader::valid()
{
    return true;
}

bool DirLoader::hasFiles()
{
    return true;
}

QJsonObject DirLoader::manifest()
{
    return QJsonDocument::fromJson(getFile("manifest.json")).object();
}

QByteArray DirLoader::getFile(QString name)
{
    QDir dir(path);
    QFile file(dir.absoluteFilePath(name));
    QByteArray data;
    if(file.open(QFile::ReadOnly))
        data=file.readAll();
    file.close();
    return data;
}
