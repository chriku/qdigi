#ifndef IMPULSEWIDGET_H
#define IMPULSEWIDGET_H

#include <QWidget>
#include <QMap>

class ImpulseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImpulseWidget(QWidget *parent = 0);
    void pushValue(int pin,bool state);
    void nextField();
    QList<QMap<int,bool> > values;
    QMap<int,bool> cur;
    void paintEvent(QPaintEvent *event);
signals:

public slots:
};

#endif // IMPULSEWIDGET_H
