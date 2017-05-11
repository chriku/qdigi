#include "text.h"
#include "settings.h"
#include <QPainter>
#include <QDebug>

Text::Text()
{
color=Qt::black;
}

QRectF Text::clickRect()
{
    return QRectF(pos.x(),pos.y(),draw(false).boundingRect().width(),1.0);
}

QPicture Text::draw(bool values)
{
    QPicture picture;
    QPainter painter(&picture);
    QFont font("Arial");
    font.setPixelSize(Settings::final()->gridSize());
    painter.setFont(font);
    QPen pen(Qt::black);
    pen.setWidth(100);
    painter.setPen(pen);
    painter.drawText(0,0,1000,Settings::final()->gridSize(),Qt::AlignLeft|Qt::AlignBottom,text);
    return picture;
}
