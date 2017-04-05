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
static void registerReg();
QString toPath(QString in);
int authCount=0;
signals:
void showMessage(QString, int);
public slots:
void update();
void authenticationRequired(QNetworkProxy proxy, QAuthenticator* auth);
void startUpdate();
};

#endif // UPDATER_H
