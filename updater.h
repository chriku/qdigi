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
QNetworkAccessManager manager;
static void registerReg();
signals:

public slots:
};

#endif // UPDATER_H
