#include "kvdiagram.h"
#include "ui_kvdiagram.h"
#include <QDebug>
#include <QPainter>

KVDiagram::KVDiagram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KVDiagram)
{
    ui->setupUi(this);
    redoTable();
}

KVDiagram::~KVDiagram()
{
    delete ui;
}

void KVDiagram::on_inputs_valueChanged(int arg1)
{
    redoTable();
}

void KVDiagram::redoTable()
{
    int columnCount=ui->inputs->value();
    int rowCount=pow(2,columnCount);
    ui->wahreitstabelle->setColumnCount(columnCount+1);
    ui->wahreitstabelle->setRowCount(rowCount);
    QStringList labels;
    for(int i=0;i<columnCount+1;i++)
    {
        ui->wahreitstabelle->setColumnWidth(i,25);
        labels.append("I"+QString::number(i+1));
    }
    labels.last()="Q";
    ui->wahreitstabelle->setHorizontalHeaderLabels(labels);
    for(int y=0;y<rowCount;y++)
        for(int x=0;x<columnCount;x++)
        {
            int val=(y)>>((columnCount-1)-x);
            val%=2;
            QString text="0";
            if(val)
                text="1";
            QTableWidgetItem*item=new QTableWidgetItem;
            item->setText(text);
            item->setFlags(Qt::NoItemFlags);
            ui->wahreitstabelle->setItem(y,x,item);
        }
    QString vals[16]={"0","1","0","1","0","1","0","1","0","1","0","1","0","1","0","1"};
    for(int y=0;y<rowCount;y++)
    {
        QTableWidgetItem*item=new QTableWidgetItem;
        item->setText(vals[y]);
        ui->wahreitstabelle->setItem(y,columnCount,item);
    }
    on_buttonBox_accepted();
    exit(0);
}

void KVDiagram::on_buttonBox_accepted()
{
    int columnCount=ui->inputs->value();
    int rowCount=pow(2,columnCount);
    QList<QString> cells;
    for(int i=0;i<rowCount;i++)
        cells.append(ui->wahreitstabelle->item(i,columnCount)->text());
    QSize size;
    switch(columnCount)
    {
    case 2:
        size=QSize(1,1);
        break;
    case 3:
        size=QSize(2,1);
        break;
    case 4:
        size=QSize(2,2);
        break;
    }
    QImage img((pow(2,size.width())+1)*100,(pow(2,size.height())+1)*100,QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);
    painter.drawText(0,100,100,100,Qt::AlignCenter,"NICHT D\nC");
    painter.drawText(0,200,100,100,Qt::AlignCenter,"D\nC");
    painter.drawText(0,300,100,100,Qt::AlignCenter,"D\nNICHT C");
    painter.drawText(0,400,100,100,Qt::AlignCenter,"NICHT D\nNICHT C");

    painter.drawText(100,0,100,100,Qt::AlignCenter,"A\nNICHT B");
    painter.drawText(200,0,100,100,Qt::AlignCenter,"A\nB");
    painter.drawText(300,0,100,100,Qt::AlignCenter,"NICHT A\nB");
    painter.drawText(400,0,100,100,Qt::AlignCenter,"NICHT A\nNICHT B");
    QMap<QPair<int,int>,int> pos;
    QImage img2=img;
    QList<QList<QString> > map;
    for(int x=0;x<pow(2,size.width());x++)
    {
        map.append(QList<QString>());
        for(int y=0;y<pow(2,size.width());y++)
        {
            bool a=(x==1)||(x==0);
            bool b=(x==1)||(x==2);
            bool c=(y==1)||(y==0);
            bool d=(y==1)||(y==2);
            int num=a|(b<<1)|(c<<2)|(d<<3);
            QString cur=QString::number(num);
            painter.setPen(Qt::black);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect((x+1)*100,(y+1)*100,100,100);
            painter.drawText((x+1)*100,(y+1)*100,100,100,Qt::AlignCenter,cur);
            map[x].append(cells.takeFirst());
        }
    }
    painter.end();
    img.save("kv1.png");
    img=img2;
    painter.begin(&img);
    for(int x=0;x<pow(2,size.width());x++)
        for(int y=0;y<pow(2,size.width());y++)
        {
            painter.drawText((x+1)*100,(y+1)*100,100,100,Qt::AlignCenter,map[x][y]);
        }
    painter.end();
    img.save("kv2.png");
}
