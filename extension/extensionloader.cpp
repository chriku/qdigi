#include "extensionloader.h"
#include "extension/dirloader.h"
#include "extension/ziploader.h"
#include <QFile>
#include <QFileInfo>

ExtensionLoader::ExtensionLoader(QObject *parent) : QObject(parent)
{

}

ExtensionLoader* ExtensionLoader::getLoader(QString path)
{
    QFileInfo fileinfo(path);
    //if(fileinfo.isDir())
    //    return new DirLoader(path);
    return NULL;
}
