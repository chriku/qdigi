#ifndef REMOTEDRIVELIST_H
#define REMOTEDRIVELIST_H

#include <QObject>
#include "remotedrive.h"

class RemoteDriveList : public QObject
{
    Q_OBJECT
public:
    explicit RemoteDriveList(QObject *parent = 0);
    static QList<RemoteDrive*> drives;
signals:

public slots:
};

#endif // REMOTEDRIVELIST_H
