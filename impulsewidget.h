#ifndef IMPULSEWIDGET_H
#define IMPULSEWIDGET_H

#include <QWidget>
#include <QMap>

class ImpulseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImpulseWidget(QWidget *parent = 0);
    void pushValue(QString pin,bool state);
    void nextField();
    QList<QMap<QString,bool> > values;
    QMap<QString,bool> cur;
    void paintEvent(QPaintEvent *event);
signals:

public slots:
};

#endif // IMPULSEWIDGET_H
