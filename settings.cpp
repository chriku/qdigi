#include "settings.h"
Settings* settings=NULL;
Settings::Settings(QObject *parent) : QObject(parent),
    saveFile(QSettings::IniFormat,QSettings::UserScope,"Christian Kurz","btd")
{
    m_gridSize=saveFile.value("gridSize",20).toInt();
    m_gridType=(GRID)saveFile.value("gridType",GRID_LINES).toInt();
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

Settings::GRID Settings::gridType()
{
    return m_gridType;
}

void Settings::setGridSize(double size, bool session)
{
    if(!session)
        saveFile.setValue("gridSize",QVariant((int)round(size)));
    m_gridSize=size;
}

void Settings::setGridType(GRID type, bool session)
{
    if(!session)
        saveFile.setValue("gridType",type);
    m_gridType=type;
}
