#include "impulsewidget.h"
#include <QDebug>
#include <QPainter>

ImpulseWidget::ImpulseWidget(QWidget *parent) : QWidget(parent)
{
}

void ImpulseWidget::nextField()
{
    QMap<int,bool> c=cur;
    //cur.clear();
    if(values.length()>0)
        if(values.last()==c)
        {
            return;
        }
    values.append(c);
    update();
}

void ImpulseWidget::pushValue(int pin, bool state)
{
    cur.insert(pin,state);
}

void ImpulseWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::red);
    if(values.length()==0)
        return;
    double ppert=width()/values.length();
    QList<int> keys;
    for(auto map:values)
        for(auto key:map.keys())
        {
            keys.removeAll(key);
            keys.append(key);
        }
    double kl=keys.length();
    if(kl<0.001)
        kl=1.0;
    double pperc=height()/kl;
    for(int i=0;i<values.length();i++)
    {
        QMap<int,bool> time=values[i];
        double px=i*ppert;
        painter.setPen(Qt::lightGray);
        painter.drawLine(px,0,px,height());
        for(int j=0;j<keys.length();j++)
        {
            double py=(j*pperc)+((pperc/3.0)*1.0);
            double h=pperc/3.0;
            if(time.contains(keys[j]))
            {
                painter.setPen(Qt::lightGray);
                painter.drawLine(px,py,px+ppert,py);
                painter.drawLine(px,py+h,px+ppert,py+h);
                py+=h;
                painter.setPen(Qt::black);
                double ch=py;
                if(!time[keys[j]])
                    ch+=h;
                    painter.drawLine(px,ch,px+ppert,ch);
                    if(i>0)
                    {
                        double oh=py;
                        if(!values[i-1][keys[j]])
                            oh+=h;
                        painter.drawLine(px,ch,px,oh);
                    }
            }
        }
    }
}
