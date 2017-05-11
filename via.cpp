#include "via.h"
#include "settings.h"
#include <QPainter>

Via::Via(QObject *parent) : Item(parent)
{

}

QRectF Via::clickRect()
{
    return QRectF(pos.x()-0.5,pos.y()-0.5,1,1);
}

QPicture Via::draw(bool values)
{
    QPicture picture;
    QPainter painter(&picture);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    double r=(2.0*Settings::final()->penWidth()*Settings::final()->gridSize());
    painter.drawEllipse(QPointF(0,0),r,r);
    return picture;
}
