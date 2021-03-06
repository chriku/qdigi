#ifndef DRAGVIEW_H
#define DRAGVIEW_H

#include <QTableWidget>
#include "items/block.h"

class DragView : public QTableWidget
{
    Q_OBJECT
public:
    explicit DragView(QWidget *parent = 0);
void dropEvent(QDropEvent *event);
void resizeEvent(QResizeEvent *event);
void startDrag(Qt::DropActions supportedActions);
void makeCols();
QString category;
QList<Block*> blockList;

signals:

public slots:
};

#endif // DRAGVIEW_H
