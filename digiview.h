#ifndef DIGIVIEW_H
#define DIGIVIEW_H
#include <QStatusBar>
#include <QWidget>
#include <QPaintEvent>
#include "blocklist.h"
#include <QTimer>
#include <QPicture>

struct block_t {
    QPoint pos;
    Block* block;
};

struct line_t {
    line_t()
    {
        state=false;
    }

    QLine line;
    bool state;
};

class DigiView : public QWidget
{
    Q_OBJECT
public:
    QList<int> selectedBlocks;
    QList<int> selectedLines;
    void clearSelection();
    void deleteSelection();
    QString fileName;
    void cleanUp();
    explicit DigiView(QWidget *parent = 0);
    QTimer timer;
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    QList<QPoint> vias;
    QPointF dragPos;
    QString dragGate;
    QList<block_t> blocks;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void zoomReset();
    void zoomIn();
    void zoomOut();
    QPoint startPoint;
    QPoint curPoint;
    QList<line_t> lines;
    bool save(QString where=QString());
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
signals:
    void changed();
public slots:
    void timeout();
};

#endif // DIGIVIEW_H
