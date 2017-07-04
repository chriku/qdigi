#include "impulsewidget.h"
#include <QDebug>
#include <QApplication>
#include <QPainter>
#include <QPicture>
#include <QScreen>
#include <QFileDialog>

ImpulseWidget::ImpulseWidget(QWidget *parent) : QWidget(parent)
{
}

void ImpulseWidget::nextField()
{
    QMap<QString,bool> c=cur;
    //cur.clear();
    if(values.length()>0)
    {
        if(c.contains("C")&&values.last().contains("C"))
        {
            if(values.last()["C"]==c["C"])
                return;
            else if(values.length()>0)
                values.last().insert("C",c["C"]);
        }
        else
            if(values.last()==c)
            {
                return;
            }
    }
    qDebug()<<c;
    values.append(c);
    update();
}

void ImpulseWidget::pushValue(QString pin, bool state)
{
    cur.insert(pin,state);
}

void ImpulseWidget::paintEvent(QPaintEvent *event)
{
    QPicture pic=draw();
    QPainter painter(this);
    painter.drawPicture(0,0,pic);
    setMinimumSize(pic.width()+5,pic.height()+5);
}

void ImpulseWidget::exportImage()
{
    QUrl fn=QFileDialog::getSaveFileUrl(NULL,"Exportieren nach",QUrl(),"*.png",NULL);
    if(fn.isLocalFile())
    {
        QString fln=fn.toLocalFile();
        if(!fln.endsWith(".png"))
            fln.append(".png");
        QPicture pic=draw();
        QPixmap pix(pic.width()+5,pic.height()+5);
        pix.fill(Qt::white);
        QPainter painter(&pix);
        painter.drawPicture(0,0,pic);
        pix.save(fln);
    }
}

QPicture ImpulseWidget::draw()
{
    QPicture ret;
    QPainter painter(&ret);
    painter.setPen(Qt::red);
    if(values.length()==0)
        return ret;
    double ppert=(QApplication::screens().first()->physicalDotsPerInch()/25.4)*10.0;
    QList<QString> keys;
    for(auto map:values)
        for(auto key:map.keys())
        {
            keys.removeAll(key);
            keys.append(key);
        }
    if(keys.contains("C"))
    {
        keys.removeAll("C");
        keys.prepend("C");
    }
    double kl=keys.length();
    if(kl<0.001)
        kl=1.0;
    double pperc=(QApplication::screens().first()->physicalDotsPerInch()/25.4)*10.0;
    for(int i=0;i<keys.length();i++)
    {
        double py=(i*pperc)+((pperc/3.0)*1.0);
        double h=pperc/3.0;
        painter.setPen(Qt::red);
        painter.drawText(0,py,100,h,Qt::AlignLeft|Qt::AlignVCenter,keys[i]);
    }
    for(int i=0;i<values.length();i++)
    {
        QMap<QString,bool> time=values[i];
        double px=i*ppert;
        painter.setPen(Qt::lightGray);
        painter.drawLine(px,0,px,pperc*kl);
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
    return ret;
}
