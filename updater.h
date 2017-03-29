#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QtNetwork>
#include <QSplashScreen>

class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent = 0);
QSplashScreen screen;
void update();
static void registerReg();
QString toPath(QString in);
signals:

public slots:
void authenticationRequired(QNetworkProxy proxy, QAuthenticator* auth);
};

#endif // UPDATER_H
