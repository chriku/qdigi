#include "settings.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QUrlQuery>
#include "passworddialog.h"
Settings* settings=NULL;
extern QNetworkAccessManager manager;
Settings::Settings(QObject *parent) : QObject(parent)
{
    oauth_id="167107903260-s3lk7i287aidt7qj2l82ml6rsd3b2cot.apps.googleusercontent.com";
    oauth_key="Qt3CQ9UMOkoE8TFkR-b6RZrL";
#ifndef Q_OS_LINUX
    //Windows
    mainPath=QApplication::applicationDirPath();
#else
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    dir.mkdir(".qdigi");
    mainPath=dir.absoluteFilePath(".qdigi");
#endif
    saveFile=new QSettings(QDir(mainPath).absoluteFilePath("settings.ini"),QSettings::IniFormat);
    QString omp=mainPath;
    mainPath=saveFile->value("appDirPath",mainPath).toString();
    if(omp!=mainPath)
        saveFile=new QSettings(QDir(mainPath).absoluteFilePath("settings.ini"),QSettings::IniFormat);
    qDebug()<<saveFile->allKeys();
    m_gridSize=saveFile->value("gridSize",20).toInt();
    m_gridType=(GRID)saveFile->value("gridType",GRID_LINES).toInt();
    m_defaultSimu=saveFile->value("defaultSimu",true).toBool();
    m_applicationDir=saveFile->value("applicationDir",mainPath).toString();
    m_lastFile=saveFile->value("lastFile","").toUrl();
    m_rasterSize=saveFile->value("rasterSize",5).toInt();
    m_penWidth=saveFile->value("penWidth",0.1).toDouble();
    m_background=saveFile->value("background",QColor(Qt::white)).value<QColor>();
    m_refresh_token=saveFile->value("refresh_token","").toString();
    m_simulationTime=saveFile->value("simulationTime",10).toInt();
    m_token=saveFile->value("token").toString();
}

Settings* Settings::final()
{
    if(settings==NULL)
        settings=new Settings;
    return settings;
}

double Settings::gridSize()
{
    return m_gridSize;
}

double Settings::penWidth()
{
    return m_penWidth;
}

QUrl Settings::lastFile()
{
    return m_lastFile;
}

Settings::GRID Settings::gridType()
{
    return m_gridType;
}

bool Settings::defaultSimu()
{
    return m_defaultSimu;
}
int Settings::rasterSize()
{
    return m_rasterSize;
}

QString Settings::applicationDir()
{
    return m_applicationDir;
}
QString Settings::username()
{
    if(m_username.isEmpty())
    {
        QNetworkRequest req;
        req.setUrl(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/login.cgi"));
        req.setRawHeader("token",m_token.toUtf8());
        QNetworkReply* rep=manager.get(req);
        QEventLoop loop;
        connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
        loop.exec();
        if(rep->error()==QNetworkReply::NoError)
        {
            m_username=rep->readAll().replace("\n","");
        }
    }
    return m_username;
}

QColor Settings::background()
{
    return m_background;
}
int Settings::simulationTime()
{
    return m_simulationTime;
}

QString Settings::refresh_token()
{
    m_refresh_token=saveFile->value("refresh_token","").toString();
    return m_refresh_token;
}


void Settings::setGridSize(double size, bool session)
{
    if(!session)
        saveFile->setValue("gridSize",QVariant((int)round(size)));
    m_gridSize=size;
}


void Settings::setRefresh_token(QString token)
{
    saveFile->setValue("refresh_token",token);
    m_refresh_token=token;
    saveFile->sync();
}

void Settings::setBackground(QColor back, bool session)
{
    if(!session)
        saveFile->setValue("background",QVariant(back));
    m_background=back;
}

void Settings::setPenWidth(double wid, bool session)
{
    if(!session)
        saveFile->setValue("penWidth",QVariant(wid));
    m_penWidth=wid;
}

void Settings::setRasterSize(int size, bool session)
{
    if(!session)
        saveFile->setValue("rasterSize",QVariant(size));
    m_rasterSize=size;
}

void Settings::setGridType(GRID type, bool session)
{
    if(!session)
        saveFile->setValue("gridType",type);
    m_gridType=type;
}

void Settings::setSimulationTime(int time, bool session)
{
    if(!session)
        saveFile->setValue("simulationTime",time);
    m_simulationTime=time;
}

void Settings::setDefaultSimu(bool on, bool session)
{
    if(!session)
        saveFile->setValue("defaultSimu",on);
    m_defaultSimu=on;
}

void Settings::setLastFile(QUrl file, bool session)
{
    if(!session)
        saveFile->setValue("lastFile",file);
    m_lastFile=file;
}

void Settings::setApplicationDir(QString dir, bool session)
{
    if(!session)
        saveFile->setValue("applicationDir",dir);
    m_applicationDir=dir;
}

void Settings::clear()
{
    saveFile->clear();
    settings->deleteLater();
    settings=NULL;
}

void Settings::addLastChanged(QUrl what)
{
    QList<QVariant> list=saveFile->value("lastChanged").toList();
    list.append(what);
    saveFile->setValue("lastChanged",list);
}

QList<QUrl> Settings::lastChanged()
{
    QList<QVariant> vlist=saveFile->value("lastChanged").toList();
    QList<QUrl> ulist;
    for(int i=0;i<vlist.length();i++)
        ulist.append(vlist[i].toUrl());
    return ulist;
}

void Settings::setColors(QList<QColor> col, bool session)
{

}

QList<QColor> Settings::colors()
{
    QList<QColor> ret;
    ret.append(Qt::black);
    ret.append(Qt::red);
    ret.append(Qt::green);
    ret.append(Qt::blue);
    return ret;
}

QString Settings::token()
{
    qDebug()<<lastToken.secsTo(QDateTime::currentDateTime());
    if((!m_token.isEmpty())||(lastToken.secsTo(QDateTime::currentDateTime())>60))
    {
        QNetworkRequest req;
        req.setUrl(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/login.cgi"));
        req.setRawHeader("token",m_token.toUtf8());
        QNetworkReply* rep=manager.get(req);
        QEventLoop loop;
        connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
        loop.exec();
        if(rep->error()==QNetworkReply::NoError)
        {
            lastToken=QDateTime::currentDateTime();
            return m_token;
        }
        else
            m_token="";
    }
    while(m_token.isEmpty())
    {
        PasswordDialog pwdd;
        if(pwdd.exec()==QDialog::Accepted)
        {
            QNetworkRequest req;
            req.setUrl(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/login.cgi"));
            QUrlQuery query;
            query.addQueryItem("username",pwdd.username->text());
            query.addQueryItem("password",pwdd.password->text());
            QNetworkReply* rep=manager.post(req,query.toString().toUtf8());
            QEventLoop loop;
            connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
            loop.exec();
            if(rep->error()==QNetworkReply::NoError)
            {
                m_token=rep->rawHeader("token");
                saveFile->setValue("token",m_token);
            }
        }
        else
            return "";
    }
    return m_token;
}

void Settings::setLua(QString key, QString value)
{
    saveFile->beginGroup("lua");
    saveFile->setValue(key,value);
    saveFile->endGroup();
}

QString Settings::getLua(QString key)
{
    saveFile->beginGroup("lua");
    QString value=saveFile->value(key).toString();
    saveFile->endGroup();
    return value;
}

bool Settings::licensePay()
{
    return true;
}

void Settings::resetToken()
{
    qDebug()<<"RESETTING TOKEN";
    m_token="";
}
