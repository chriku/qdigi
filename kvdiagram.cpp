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
    QString vals[16]={"0","0","1","1","1","1","1","0","0","0","0","0","0","0","0","0"};
    //QString vals[16]={"1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1"};
    for(int y=0;y<rowCount;y++)
    {
        QTableWidgetItem*item=new QTableWidgetItem;
        item->setText(vals[y]);
        ui->wahreitstabelle->setItem(y,columnCount,item);
    }
    //on_buttonBox_accepted();
}

void KVDiagram::on_buttonBox_accepted()
{
    QList<int*> values;
    int columnCount=ui->inputs->value();
    int rowCount=pow(2,columnCount);
    for(int i=0;i<rowCount;i++)
    {
        int val=0;
        if(ui->wahreitstabelle->item(i,4)->text()=="0")
            val=0;
        if(ui->wahreitstabelle->item(i,4)->text()=="1")
            val=1;
        if(val==1)
        {
            values.append((int*)malloc(sizeof(int)*columnCount));
            for(int j=0;j<4;j++)
            {
                values.last()[j]=(i>>(3-j))%2==1;
            }
        }
    }
    for(int round=0;round<10;round++)
    {
        qDebug()<<"Round";
        for(int i=0;i<values.length();i++)
            qDebug()<<values[i][0]<<values[i][1]<<values[i][2]<<values[i][3];
        for(int i=0;i<values.length();i++)
            for(int j=i+1;j<values.length();j++)
                {
                    int diff=0;
                    for(int k=0;k<4;k++)
                    {
                        int v1=values[i][k];
                        int v2=values[j][k];
                        /*if(v1==2)
                            continue;
                        if(v2==2)
                            continue;*/
                        if(v1!=v2)
                            diff++;
                    }
                    if(diff==1)
                    {
                        int *same=(int*)malloc(4);
                        for(int k=0;k<4;k++)
                        {
                            same[k]=2;
                            int v1=values[i][k];
                            int v2=values[j][k];
                            same[k]=2;
                            if(v1==2)
                                continue;
                            if(v2==2)
                                continue;
                            if(v1==v2)
                                same[k]=v1;
                        }
                        values[i]=same;
                        values.removeAt(j);
                    }
                    if(diff==0)
                        values.removeAt(j);
                }
    }
    qDebug()<<"Result";
    for(int i=0;i<values.length();i++)
        qDebug()<<values[i][0]<<values[i][1]<<values[i][2]<<values[i][3];
    exit(0);
}
