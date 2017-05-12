#ifndef IMPULSELABEL_H
#define IMPULSELABEL_H

#include "item.h"

class ImpulseLabel : public Item
{
    Q_OBJECT
public:
    explicit ImpulseLabel(QObject *parent = 0);
    QRectF clickRect();
    QPicture draw();
    bool state;
    QString name;

signals:

public slots:
};

#endif // IMPULSELABEL_H
