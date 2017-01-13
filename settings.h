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
    Q_PROPERTY(QString lastFile READ lastFile WRITE setLastFile)
    Q_PROPERTY(QString applicationDir READ applicationDir WRITE setApplicationDir)
    Q_PROPERTY(bool defaultSimu READ defaultSimu WRITE setDefaultSimu)
    Q_PROPERTY(GRID gridType READ gridType WRITE setGridType)
    explicit Settings(QObject *parent = 0);
    double m_gridSize;
    QString m_lastFile;
    bool m_defaultSimu;
    double gridSize();
    QString lastFile();
    QString applicationDir();
    bool defaultSimu();
    static Settings* final();
GRID gridType();
void setApplicationDir(QString dir,bool session=false);
void setLastFile(QString file,bool session=false);
void setGridSize(double size,bool session=false);
void setDefaultSimu(bool on, bool session=false);
void setGridType(GRID type,bool session=false);
QString m_applicationDir;
signals:

public slots:
};

#endif // SETTINGS_H
