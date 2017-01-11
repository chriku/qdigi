#ifndef DRAGVIEW_H
#define DRAGVIEW_H

#include <QListWidget>

class DragView : public QListWidget
{
    Q_OBJECT
public:
    explicit DragView(QWidget *parent = 0);
void dropEvent(QDropEvent *event);
signals:

public slots:
};

#endif // DRAGVIEW_H
