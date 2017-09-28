#ifndef LINE_H
#define LINE_H

#include <QObject>
#include "item.h"
#include <QLine>

class Line : public Item
{
    Q_OBJECT
public:
    Line();
    QRectF clickRect();
    QPicture draw();
    QLine line;
    bool state;
};

#endif // LINE_H
