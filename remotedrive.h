#ifndef REMOTEDRIVE_H
#define REMOTEDRIVE_H

#include <QObject>

class RemoteDrive : public QObject
{
    Q_OBJECT
public:
    explicit RemoteDrive(QObject *parent = 0);

signals:

public slots:
};

#endif // REMOTEDRIVE_H