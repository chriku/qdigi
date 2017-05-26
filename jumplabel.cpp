#include "jumplabel.h"
#include <QPainter>
#include "settings.h"

JumpLabel::JumpLabel(QObject *parent) : Item(parent)
{

}

QRectF JumpLabel::clickRect()
{
    QPicture picture;
    QPainter painter(&picture);
    QFont font("Arial");
    font.setPixelSize(Settings::final()->gridSize()*0.75);
    painter.setFont(font);
    QPen pen(Qt::black);
    pen.setWidth(100);
    painter.setPen(pen);
    double w=painter.boundingRect(QRectF(0,0,1000,Settings::final()->gridSize()),Qt::AlignLeft|Qt::AlignVCenter,name).width();
    QRectF ret=QRectF(pos.x(),pos.y(),w/Settings::final()->gridSize(),1.0);
    return ret;
}

QPicture JumpLabel::draw()
{
    QPicture picture;
    QPainter painter(&picture);
    QFont font("Arial");
    font.setPixelSize(Settings::final()->gridSize()*0.75);
    painter.setFont(font);
    QPen pen(Qt::black);
    pen.setWidth(100);
    painter.setPen(pen);
    painter.drawText(0,0,1000,Settings::final()->gridSize(),Qt::AlignLeft|Qt::AlignVCenter,name);
    pen.setColor(Qt::green);
    pen.setWidth(Settings::final()->gridSize()*Settings::final()->penWidth());
    painter.setPen(pen);
    double d=Settings::final()->gridSize()*0.25;
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(0,0),d,d);
    return picture;
}
