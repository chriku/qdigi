#ifndef VIA_H
#define VIA_H

#include <QObject>
#include "item.h"

class Via : public Item
{
    Q_OBJECT
public:
    explicit Via(QObject *parent = 0);
    QRectF clickRect();
    QPicture draw(bool values);
    bool state;

signals:

public slots:
};

#endif // VIA_H
