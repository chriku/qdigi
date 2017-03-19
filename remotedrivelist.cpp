#include "remotedrivelist.h"
#include <QDir>
#include "settings.h"

QList<RemoteDrive*> RemoteDriveList::drives;

RemoteDriveList::RemoteDriveList(QObject *parent) : QObject(parent)
{
    if(drives.isEmpty())
    {
        QDir dir(Settings::final()->applicationDir());
        dir.cd("drives");
        QStringList files=dir.entryList();
        for(int i=0;i<files.length();i++)
            if(files[i].endsWith(".lua"))
            {
                RemoteDrive*b=new RemoteDrive(dir.absoluteFilePath(files[i]));
                drives.append(b);
            }
    }

}
