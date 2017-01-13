#ifndef DIGIVIEW_H
#define DIGIVIEW_H
#include <QStatusBar>
#include <QWidget>
#include <QPaintEvent>
#include "blocklist.h"
#include <QTimer>

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
    QString fileName;
    void cleanUp();
    explicit DigiView(QWidget *parent = 0);
    QTimer timer;
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    QPointF dragPos;
    QString dragGate;
    QList<block_t> blocks;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QPoint startPoint;
    QPoint curPoint;
    QList<line_t> lines;
    void save(QString where=QString());
    void load(QString where);
    QPoint toGrid(QPoint in);
    void contextMenuEvent(QContextMenuEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool interLine(QLine l1,QLine l2);
    bool onLine(QLine line,QPoint point);
    QStatusBar* bar;
    QList<QPoint> allIntersect(QLine line);
signals:

public slots:
    void timeout();
};

#endif // DIGIVIEW_H
