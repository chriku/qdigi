#ifndef REMOTEDRIVELIST_H
#define REMOTEDRIVELIST_H

#include <QObject>
#include "remotedrive.h"

class RemoteDriveList : public QObject
{
    Q_OBJECT
public:
    explicit RemoteDriveList(QObject *parent = 0);

signals:

public slots:
};

#endif // REMOTEDRIVELIST_H
