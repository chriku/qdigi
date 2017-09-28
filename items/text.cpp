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
    QPicture picture;
    QPainter painter(&picture);
    QFont font("Arial");
    font.setPixelSize(Settings::final()->gridSize()*0.75);
    painter.setFont(font);
    QPen pen(Qt::black);
    pen.setWidth(100);
    painter.setPen(pen);
    double w=painter.boundingRect(QRectF(0,0,1000,Settings::final()->gridSize()),Qt::AlignLeft|Qt::AlignVCenter,text).width();
    QRectF ret=QRectF(pos.x(),pos.y()-0.5,w/Settings::final()->gridSize(),1.0);
    return ret;
}

QPicture Text::draw()
{
    QPicture picture;
    QPainter painter(&picture);
    QFont font("Arial");
    font.setPixelSize(Settings::final()->gridSize()*0.75);
    painter.setFont(font);
    QPen pen(Qt::black);
    pen.setWidth(100);
    painter.setPen(pen);
    painter.drawText(0,-0.5*Settings::final()->gridSize(),1000,Settings::final()->gridSize(),Qt::AlignLeft|Qt::AlignVCenter,text);
    return picture;
}
