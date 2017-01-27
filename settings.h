#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>
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
    Q_PROPERTY(double penWidth READ penWidth WRITE setPenWidth)
    Q_PROPERTY(int rasterSize READ rasterSize WRITE setRasterSize)
    Q_PROPERTY(QString lastFile READ lastFile WRITE setLastFile)
    Q_PROPERTY(QString license READ license WRITE setLicense)
    Q_PROPERTY(QString applicationDir READ applicationDir WRITE setApplicationDir)
    Q_PROPERTY(bool defaultSimu READ defaultSimu WRITE setDefaultSimu)
    Q_PROPERTY(GRID gridType READ gridType WRITE setGridType)
    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_PROPERTY(QList<QColor> colors READ colors WRITE setColors)
    QStringList lastChanged();
    void addLastChanged(QString what);
    explicit Settings(QObject *parent = 0);
    double m_gridSize;
    double m_penWidth;
    QColor m_background;
    QList<QColor> m_colors;
    QString m_lastFile;
    QString m_license;
    bool m_defaultSimu;
    int m_rasterSize;
    double gridSize();
    double penWidth();
    int rasterSize();
    QString lastFile();
    QString applicationDir();
    bool defaultSimu();
    static Settings* final();
    QString license();
    QColor background();
GRID gridType();
QList<QColor> colors();
void setBackground(QColor back,bool session=false);
void setColors(QList<QColor> col,bool session=false);
void setApplicationDir(QString dir,bool session=false);
void setLicense(QString lic,bool session=false);
void setPenWidth(double wid,bool session=false);
void setRasterSize(int size,bool session=false);
void setLastFile(QString file,bool session=false);
void setGridSize(double size,bool session=false);
void setDefaultSimu(bool on, bool session=false);
void setGridType(GRID type,bool session=false);
QString m_applicationDir;
void clear();
signals:

public slots:
};

#endif // SETTINGS_H
