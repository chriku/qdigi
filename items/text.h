#ifndef TEXT_H
#define TEXT_H

#include <QObject>
#include "item.h"

class Text : public Item
{
    Q_OBJECT
public:
    Text();
    QRectF clickRect();
    QPicture draw();
    QString text;
};

#endif // TEXT_H
