#ifndef GDRIVE_H
#define GDRIVE_H

#include <QObject>
#include <QDateTime>

class GDrive : public QObject
{
    Q_OBJECT
public:
    explicit GDrive(QObject *parent = 0);
    void loadToken();
    QString selectFile();
    QString access_token;
    QDateTime access_timeout;
    void uploadFile(QString name,QByteArray data);
    QJsonObject post(QString resource,QString data);
    QJsonObject get(QString resource);
    QStringList listFiles();
signals:

public slots:
};

#endif // GDRIVE_H
