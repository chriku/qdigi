#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QPicture>
#include <QColor>

class Item : public QObject
{
    Q_OBJECT
public:
    Item(QObject* parent=NULL);
    virtual QRectF clickRect()=0;
    virtual QPicture draw()=0;
    QPoint pos;
    QColor color;
};

#endif // ITEM_H
