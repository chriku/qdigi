#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>
#include <QSettings>
#include <math.h>
#include <QUrl>
#include <QDateTime>

class Settings : public QObject
{
    Q_OBJECT
public:
    QSettings* saveFile;
    enum GRID {
        GRID_POINT,
        GRID_LINES
    } m_gridType;
    QString oauth_id;
    QString oauth_key;
    QString mainPath;
    Q_PROPERTY(double gridSize READ gridSize WRITE setGridSize)
    Q_PROPERTY(QString refresh_token READ refresh_token WRITE setRefresh_token)
    Q_PROPERTY(double penWidth READ penWidth WRITE setPenWidth)
    Q_PROPERTY(int rasterSize READ rasterSize WRITE setRasterSize)
    Q_PROPERTY(QUrl lastFile READ lastFile WRITE setLastFile)
    Q_PROPERTY(QString token READ token RESET resetToken)
    Q_PROPERTY(QString applicationDir READ applicationDir WRITE setApplicationDir)
    Q_PROPERTY(bool defaultSimu READ defaultSimu WRITE setDefaultSimu)
    Q_PROPERTY(GRID gridType READ gridType WRITE setGridType)
    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_PROPERTY(QList<QColor> colors READ colors WRITE setColors)
    Q_PROPERTY(int simulationTime READ simulationTime WRITE setSimulationTime)
    QList<QUrl> lastChanged();
    int simulationTime();
    void addLastChanged(QUrl what);
    explicit Settings(QObject *parent = 0);
    QString m_refresh_token;
    double m_gridSize;
    double m_penWidth;
    QColor m_background;
    QList<QColor> m_colors;
    QUrl m_lastFile;
    QString m_token;
    bool m_defaultSimu;
    int m_rasterSize;
    double gridSize();
    double penWidth();
    int rasterSize();
    QUrl lastFile();
    QString applicationDir();
    bool defaultSimu();
    static Settings* final();
    QString token();
    QDateTime lastToken;
    void resetToken();
    bool licensePay();
    QColor background();
    QString username();
    QString refresh_token();
    GRID gridType();
    QList<QColor> colors();
    void setRefresh_token(QString token);
    void setBackground(QColor back,bool session=false);
    void setSimulationTime(int time,bool session=false);
    void setColors(QList<QColor> col,bool session=false);
    void setApplicationDir(QString dir,bool session=false);
    void setPenWidth(double wid,bool session=false);
    void setRasterSize(int size,bool session=false);
    void setLastFile(QUrl file,bool session=false);
    void setGridSize(double size,bool session=false);
    void setDefaultSimu(bool on, bool session=false);
    void setGridType(GRID type,bool session=false);
    QString m_applicationDir;
    void clear();
    void setLua(QString key,QString value);
    QString getLua(QString key);
    int m_simulationTime;
    QString m_username;
signals:

public slots:
};

#endif // SETTINGS_H
