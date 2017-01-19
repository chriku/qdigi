#ifndef DRAGVIEW_H
#define DRAGVIEW_H

#include <QTableWidget>

class DragView : public QTableWidget
{
    Q_OBJECT
public:
    explicit DragView(QWidget *parent = 0);
void dropEvent(QDropEvent *event);
void resizeEvent(QResizeEvent *event);
void makeCols();
signals:

public slots:
};

#endif // DRAGVIEW_H
