#include "settings.h"
#include <QApplication>

Settings* settings=NULL;
Settings::Settings(QObject *parent) : QObject(parent),
    saveFile(QSettings::IniFormat,QSettings::UserScope,"Christian Kurz","btd")
{
    m_gridSize=saveFile.value("gridSize",20).toInt();
    m_gridType=(GRID)saveFile.value("gridType",GRID_LINES).toInt();
    m_defaultSimu=saveFile.value("defaultSimu",false).toBool();
    m_applicationDir=saveFile.value("applicationDir",QApplication::applicationDirPath()).toString();
    m_lastFile=saveFile.value("lastFile","").toString();
    m_rasterSize=saveFile.value("rasterSize",5).toInt();
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

QString Settings::lastFile()
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

void Settings::setGridSize(double size, bool session)
{
    if(!session)
        saveFile.setValue("gridSize",QVariant((int)round(size)));
    m_gridSize=size;
}

void Settings::setRasterSize(int size, bool session)
{
    if(!session)
        saveFile.setValue("rasterSize",QVariant(size));
    m_rasterSize=size;
}

void Settings::setGridType(GRID type, bool session)
{
    if(!session)
        saveFile.setValue("gridType",type);
    m_gridType=type;
}

void Settings::setDefaultSimu(bool on, bool session)
{
    if(!session)
        saveFile.setValue("defaultSimu",on);
    m_defaultSimu=on;
}

void Settings::setLastFile(QString file, bool session)
{
    if(!session)
        saveFile.setValue("lastFile",file);
    m_lastFile=file;
}

void Settings::setApplicationDir(QString dir, bool session)
{
    if(!session)
        saveFile.setValue("applicationDir",dir);
    m_applicationDir=dir;
}
