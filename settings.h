#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    QSettings saveFile;
    enum GRID {
        GRID_POINT,
        GRID_LINES
    } m_gridType;
    Q_PROPERTY(double gridSize READ gridSize WRITE setGridSize)
    Q_PROPERTY(GRID gridType READ gridType WRITE setGridType)
    explicit Settings(QObject *parent = 0);
    double m_gridSize;
    double gridSize();
    static Settings* final();
GRID gridType();
void setGridSize(double size,bool session=false);
void setGridType(GRID type,bool session=false);
signals:

public slots:
};

#endif // SETTINGS_H
