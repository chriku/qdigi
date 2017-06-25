#include "block.h"
#include <QDir>
#include "painter.h"
#include "settings.h"
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QTime>
#include "luablock.h"
extern "C" {
#include "luasocket.h"
}

QFileSystemWatcher *watcher=NULL;

Block::Block(QObject *parent) : Item(parent) {
    useFake=false;
    color=Qt::black;
}

Block::~Block()
{

}


QPicture Block::draw() {
    QPicture ret;
    QPainter qpainter(&ret);
    for (int i = 0; i < pins.length(); i++) {
        QPen pen(pins[i].color);
        if(pen.color()==Qt::transparent)
            pen.setColor(Qt::black);
        pen.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
        qpainter.setPen(pen);
        QPoint dir(Settings::final()->gridSize() / 2.0, 0);
        if (pins[i].direction == 2)
            dir = QPoint(-Settings::final()->gridSize() / 2.0, 0);
        if (pins[i].type == true) {
            double rad = abs(dir.x()) / 4.0;
            qpainter.setBrush(Qt::NoBrush);
            qpainter.drawEllipse(
                        (QPointF(pins[i].m_point * Settings::final()->gridSize())) +
                        ((dir / 4.0) * 3.0),
                        rad, rad);
            dir /= 2.0;
        }
        //if(!plain)
        {
            qpainter.drawLine(pins[i].m_point * Settings::final()->gridSize(),
                              (pins[i].m_point * Settings::final()->gridSize()) + dir);
            pen.setColor(Qt::black);
            pen.setStyle(Qt::DotLine);
            pen.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
            bool err=false;
            if (pins[i].direction == 2) {
                if (getState(i))
                    pen.setColor(Qt::red);
            } else {
                if (pins[i].state==2)
                {
                    err=true;
                    pen.setColor(QColor::fromRgbF(1,0.5,0));
                }
                else if (pins[i].state)
                    pen.setColor(Qt::red);
            }
        }
        qpainter.setPen(pen);
        qpainter.drawLine(pins[i].m_point * Settings::final()->gridSize(),
                          (pins[i].m_point * Settings::final()->gridSize()) + dir);
        // qDebug()<<(pins[i].point*10)<<pins[i].direction<<width;
    }




    return ret;
}



QRectF Block::clickRect()
{
    QRectF rect(pos.x(),pos.y()+0.5,width+1,height);
    return rect;
}

QPointF Block::unmap(QPointF p)
{
    return p-pos;
}
