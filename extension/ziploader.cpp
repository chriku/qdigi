#include "ziploader.h"

ZipLoader::ZipLoader(QString path, QObject *parent) : ExtensionLoader(parent)
{

}

bool ZipLoader::valid()
{

}

bool ZipLoader::hasFiles()
{
    return true;
}

QJsonObject ZipLoader::manifest()
{

}

QByteArray ZipLoader::getFile(QString name)
{

}
