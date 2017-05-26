#ifndef DIGIVIEW_H
#define DIGIVIEW_H
#include <QStatusBar>
#include <QWidget>
#include <QPaintEvent>
#include "blocklist.h"
#include "impulsedialog.h"
#include "impulselabel.h"
#include "jumplabel.h"
#include <QTimer>
#include <QPicture>
#include <QTime>
#include <QList>
#include "line.h"
#include "text.h"
#include "via.h"


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
    void simulate();
    ImpulseDialog dialog;
    bool error;
    QList<QPair<QColor,QString> > loadColorProfile();
    QList<int> selection;

    QList<Block*> blocks;
    QList<Line*> lines;
    QList<Text*> texts;
    QList<Via*> vias;
    QList<ImpulseLabel*> impulseLabels;
    QList<JumpLabel*> jumpLabels;
    QList<Item*> items;

    bool isBlock(Item* item);
    bool isLine(Item* item);
    bool isText(Item* item);
    bool isVia(Item* item);
    bool isImpulseLabel(Item* item);
    bool isJumpLabel(Item* item);

    QList<QPair<int,int> > getItemsForOutput(QPoint pos, QList<int> *witems);
    QList<int> getNet(QLine in);
    bool recording;
    void clearSelection();
    void deleteSelection();
    QUrl fileName;
    void cleanUp();
    explicit DigiView(QWidget *parent = 0);
    QTimer timer;
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
    void load(QUrl where);
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
    void addBlock(QPoint pos, QString type);
    QPoint toScreen(QPointF pos);
    void check();
    int lastSel;
signals:
    void changed();
public slots:
    void timeout();
};

#endif // DIGIVIEW_H
