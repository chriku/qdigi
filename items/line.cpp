#include "line.h"
#include "settings.h"
#include <QPainter>

Line::Line()
{
    state=false;
}

QRectF Line::clickRect()
{
    if(line.p1().x()>line.p2().x())
    {
        QPoint a=line.p2();
        line.setP2(line.p1());
        line.setP1(a);
    }
    if(line.p1().y()>line.p2().y())
    {
        QPoint a=line.p2();
        line.setP2(line.p1());
        line.setP1(a);
    }
    QRectF rect(line.p1(),line.p2());
    rect.normalized();
    if(rect.height()<0.1)
    {
        rect=QRectF(rect.x(),rect.y()-0.5,rect.width(),1.0);
    }
    if(rect.width()<0.1)
    {
        rect=QRectF(rect.x()-0.5,rect.y(),1.0,rect.height());
    }
    //rect=QRectF(rect.x()*Settings::final()->gridSize(),rect.y()*Settings::final()->gridSize(),rect.width()*Settings::final()->gridSize(),rect.height()*Settings::final()->gridSize());
    return rect;
}

QPicture Line::draw()
{
    if(line.p1().x()>line.p2().x())
    {
        QPoint a=line.p2();
        line.setP2(line.p1());
        line.setP1(a);
    }
    if(line.p1().y()>line.p2().y())
    {
        QPoint a=line.p2();
        line.setP2(line.p1());
        line.setP1(a);
    }
    QPicture picture;
    QPainter painter(&picture);
    double s=Settings::final()->gridSize();
    QPen pen(color);
    pen.setWidth(Settings::final()->gridSize()*Settings::final()->penWidth());
    painter.setPen(pen);
    painter.drawLine(QLineF(line.x1()*s,line.y1()*s,line.x2()*s,line.y2()*s));
    pen.setColor(Qt::red);
    pen.setWidth(Settings::final()->gridSize()*Settings::final()->penWidth());
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    if(state)
        painter.drawLine(QLineF(line.x1()*s,line.y1()*s,line.x2()*s,line.y2()*s));
    return picture;
}
