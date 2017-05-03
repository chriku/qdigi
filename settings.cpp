#include "settings.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Settings* settings=NULL;
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
    m_gridSize=saveFile->value("gridSize",20).toInt();
    m_gridType=(GRID)saveFile->value("gridType",GRID_LINES).toInt();
    m_defaultSimu=saveFile->value("defaultSimu",true).toBool();
    m_applicationDir=saveFile->value("applicationDir",mainPath).toString();
    m_lastFile=saveFile->value("lastFile","").toUrl();
    m_rasterSize=saveFile->value("rasterSize",5).toInt();
    m_penWidth=saveFile->value("penWidth",0.1).toDouble();
    m_license=saveFile->value("licenseKey","").toString();
    m_background=saveFile->value("background",QColor(Qt::white)).value<QColor>();
    m_refresh_token=saveFile->value("refresh_token","").toString();
    m_simulationTime=saveFile->value("simulationTime",10).toInt();
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

void Settings::setLicense(QString lic, bool session)
{
    if(!session)
        saveFile->setValue("licenseKey",lic);
    m_license=lic;
}

QString Settings::license()
{
    return m_license;
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
