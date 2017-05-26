#ifndef JUMPLABEL_H
#define JUMPLABEL_H

#include "item.h"

class JumpLabel : public Item
{
    Q_OBJECT
public:
    explicit JumpLabel(QObject *parent = 0);
    QRectF clickRect();
    QPicture draw();
    bool state;
    QString name;

signals:

public slots:
};


#endif // JUMPLABEL_H
