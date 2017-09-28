#ifndef DIGIVIEW_H
#define DIGIVIEW_H
#include <QStatusBar>
#include <QWidget>
#include "mainwindow.h"
#include <QPaintEvent>
#include "blocklist.h"
#include <QTimer>
#include <QPicture>
#include <QTime>
#include <QList>
#include "schematic.h"


/*struct text_t {
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
};*/

class DigiView : public QWidget
{
    Q_OBJECT
public:
    MainWindow*mwp;
    void simulate();
    Schematic* rootSchematic;
    Schematic* curSchematic;
    QList<Schematic*> childSchematics;
    bool error;
    QList<QPair<QColor,QString> > loadColorProfile();
    QList<int> selection;

    bool recording;
    void clearSelection();
    void deleteSelection();
    QUrl fileName;
    explicit DigiView(QWidget *parent = 0);
    QTimer timer;
    QTimer timer2;
    void largeIn(int o);
    void emitKey(QString key);
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    QPointF dragPos;
    QString dragGate;
    bool dragMany;
    QTime lastContext;
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
    int dragIdx;
    QPoint startBlock;
    bool save(QUrl where=QUrl());
    QJsonObject exportFull();
    void load(QUrl where);
    QPoint toGrid(QPoint in);
    void contextMenuEvent(QContextMenuEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool interLine(QLine l1,QLine l2);
    QStatusBar* bar;
    QList<QPoint> allIntersect(QLine line);
    QPicture exportPicture();
    QImage exportImage();
    void resizeNow();
    void createTable();
    void resizeEvent(QResizeEvent *event);
    QPoint toScreen(QPointF pos);
    int lastSel;
    void change();
    QList<QByteArray> undoBuf;
    QList<QByteArray> redoBuf;
    void undo();
    void redo();
signals:
    void changed();
public slots:
    void timeout();
    void purgeSchematic();
};

#endif // DIGIVIEW_H
