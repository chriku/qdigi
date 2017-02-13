#include "settings.h"
#include <QApplication>

Settings* settings=NULL;
Settings::Settings(QObject *parent) : QObject(parent),
    saveFile(QApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat)
{
    m_gridSize=saveFile.value("gridSize",20).toInt();
    m_gridType=(GRID)saveFile.value("gridType",GRID_LINES).toInt();
    m_defaultSimu=saveFile.value("defaultSimu",false).toBool();
    m_applicationDir=saveFile.value("applicationDir",QApplication::applicationDirPath()).toString();
    m_lastFile=saveFile.value("lastFile","").toString();
    m_rasterSize=saveFile.value("rasterSize",5).toInt();
    m_penWidth=saveFile.value("penWidth",0.1).toDouble();
    m_license=saveFile.value("licenseKey","").toString();
    m_background=saveFile.value("background",QColor(Qt::white)).value<QColor>();
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

QColor Settings::background()
{
    return m_background;
}


void Settings::setGridSize(double size, bool session)
{
    if(!session)
        saveFile.setValue("gridSize",QVariant((int)round(size)));
    m_gridSize=size;
}

void Settings::setBackground(QColor back, bool session)
{
    if(!session)
        saveFile.setValue("background",QVariant(back));
    m_background=back;
}

void Settings::setPenWidth(double wid, bool session)
{
    if(!session)
        saveFile.setValue("penWidth",QVariant(wid));
    m_penWidth=wid;
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

void Settings::clear()
{
    saveFile.clear();
    settings->deleteLater();
    settings=NULL;
}

void Settings::addLastChanged(QString what)
{
    QStringList list=saveFile.value("lastChanged").toStringList();
    list.append(what);
    saveFile.setValue("lastChanged",list);
}

QStringList Settings::lastChanged()
{
    return saveFile.value("lastChanged").toStringList();
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
        saveFile.setValue("licenseKey",lic);
    m_license=lic;
}

QString Settings::license()
{
    return m_license;
}
