#ifndef DIGIVIEW_H
#define DIGIVIEW_H
#include <QStatusBar>
#include <QWidget>
#include <QPaintEvent>
#include "blocklist.h"
#include <QTimer>
#include <QPicture>



struct text_t {
    QPoint pos;
    QString text;
    double len;
    QColor color;
};

struct line_t {
    line_t()
    {
        state=false;
        color=Qt::black;
    }

    QLine line;
    bool state;
    QColor color;
};

class DigiView : public QWidget
{
    Q_OBJECT
public:
    bool error;
    QList<int> linesAtPoint(QPoint point);
    QList<QPair<QColor,QString> > loadColorProfile();
    QList<int> getNet(QLine in);
    QList<int> selectedBlocks;
    QList<int> selectedLines;
    QList<int> selectedTexts;
    void clearSelection();
    void deleteSelection();
    QString fileName;
    void cleanUp();
    explicit DigiView(QWidget *parent = 0);
    QTimer timer;
    void largeIn(int o);
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    QList<QPoint> vias;
    QPointF dragPos;
    QString dragGate;
    QList<block_t*> blocks;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void zoomReset();
    void zoomIn();
    void zoomOut();
    QPoint startPoint;
    QPoint curPoint;
    bool drag;
    bool dragged;
    int blkIdx;
    int texIdx;
    QPoint startBlock;
    QList<line_t> lines;
    QList<text_t> texts;
    bool save(QString where=QString());
    bool saveGoogle(QString where=QString());
    void load(QString where);
    QPoint toGrid(QPoint in);
    void contextMenuEvent(QContextMenuEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool interLine(QLine l1,QLine l2);
    bool onLine(QLine line,QPoint point,bool proper=false);
    QStatusBar* bar;
    QList<QPoint> allIntersect(QLine line);
    QPicture exportPicture();
    QImage exportImage();
    void loadJson(QByteArray json);
    void resizeNow();
QJsonObject exportJSON();
    void createTable();
    void resizeEvent(QResizeEvent *event);
signals:
    void changed();
public slots:
    void timeout();
};

#endif // DIGIVIEW_H
