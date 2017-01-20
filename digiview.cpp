#include <QFileDialog>
#include <QMenu>
#include "settings.h"
#include "digiview.h"
#include <QPainter>
#include "blocklist.h"
#include <QDebug>
#include <QIcon>
#include <QMimeData>
#include <QAbstractItemModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

DigiView::DigiView(QWidget *parent) : QWidget(parent)
{
    startPoint=QPoint(-1,-1);
    curPoint=QPoint(-1,-1);
    BlockList list;
    setAcceptDrops(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer.setInterval(10);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void DigiView::paintEvent(QPaintEvent* event)
{
    int grid=Settings::final()->gridSize();
    QPainter painter(this);
    painter.setPen(Qt::gray);
    switch(Settings::final()->gridType())
    {
    case Settings::GRID_LINES:
        for(int x=0;x<width();x+=grid)
            for(int y=0;y<height();y+=grid)
                painter.drawLine(x,y,x+grid,y);
        for(int x=0;x<width();x+=grid)
            for(int y=0;y<height();y+=grid)
                painter.drawLine(x,y,x,y+grid);
        break;
    case Settings::GRID_POINT:
    default:
        for(int x=0;x<width();x+=grid)
            for(int y=0;y<height();y+=grid)
                painter.drawPoint(x,y);
    }
    if(!dragGate.isEmpty())
    {
        Block* block=NULL;
        for(int i=0;i<BlockList::blocks.length();i++)
            if(BlockList::blocks[i]->name==dragGate)
                block=BlockList::blocks[i];
        if(block!=NULL)
        {
            painter.drawPixmap(dragPos.x()*Settings::final()->gridSize(),dragPos.y()*Settings::final()->gridSize(),block->drawBlock());
        }
    }
    for(int i=0;i<blocks.length();i++)
        painter.drawPixmap(blocks[i].pos*Settings::final()->gridSize(),blocks[i].block->drawBlock());
    QPen line(Qt::black);
    line.setWidth(5);
    painter.setPen(line);
    if(curPoint!=QPoint(-1,-1))
        painter.drawLine(startPoint*Settings::final()->gridSize(),curPoint*Settings::final()->gridSize());
    line.setWidth(3);
    for(int i=0;i<lines.length();i++)
    {
        if(lines[i].state)
            line.setColor(Qt::red);
        else
            line.setColor(Qt::black);
        painter.setPen(line);
        painter.drawLine(lines[i].line.p1()*Settings::final()->gridSize(),lines[i].line.p2()*Settings::final()->gridSize());
    }
    for(int i=0;i<vias.length();i++)
        painter.drawEllipse(vias[i]*Settings::final()->gridSize(),3,3);
}
void DigiView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->formats().contains("application/x-qabstractitemmodeldatalist"))
    {
        QByteArray encoded=event->mimeData()->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        QString gate="";
        while (!stream.atEnd())
        {
            int row, col;
            QMap<int,  QVariant> roleDataMap;
            stream >> row >> col >> roleDataMap;
            gate=roleDataMap[0].toString();
            break;
            /* do something with the data */
        }
        dragGate=gate;
        dragPos=toGrid(event->pos())-QPoint(1,1);
        event->acceptProposedAction();
        //qDebug()<<event;
        update();
    }
}

void DigiView::dragMoveEvent(QDragMoveEvent *event)
{
    dragPos=toGrid(event->pos())-QPoint(1,1);
    update();
}

void DigiView::dragLeaveEvent(QDragLeaveEvent *event)
{
    dragGate="";
    update();
}

void DigiView::dropEvent(QDropEvent *event)
{
    dragPos=toGrid(event->pos())-QPoint(1,1);
    block_t blk;
    blk.block=BlockList::newBlock(dragGate);
    blk.pos=dragPos.toPoint();
    dragGate="";
    for(int i=0;i<blocks.length();i++)
        if(QRect(blocks[i].pos.x(),blocks[i].pos.y(),blocks[i].block->width,blocks[i].block->height).intersects(QRect(blk.pos.x(),blk.pos.y(),blk.block->width,blk.block->height)))
            return;
    blocks.append(blk);
    emit changed();
    cleanUp();
    update();
}

void DigiView::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    startPoint=toGrid(event->pos());
    curPoint=startPoint;
    int idx=-1;
    for(int i=0;i<blocks.length();i++)
        if((blocks[i].pos.x())<curPoint.x())
            if((blocks[i].pos.y())<curPoint.y())
                if((blocks[i].pos.y()+blocks[i].block->height)>=curPoint.y())
                    if((blocks[i].pos.x()+blocks[i].block->width)>=curPoint.x())
                    {
                        idx=i;
                    }
    if(idx>=0)
    {
        double x=event->pos().x()/Settings::final()->gridSize();
        double y=event->pos().y()/Settings::final()->gridSize();
        QPointF p(x,y);
        blocks[idx].block->onpress(p-QPointF(blocks[idx].pos));
    }
    update();
}

void DigiView::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    curPoint=toGrid(event->pos());
    update();
}

void DigiView::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    curPoint=toGrid(event->pos());
    if((startPoint.x()==curPoint.x())||(startPoint.y()==curPoint.y()))
        if(startPoint!=curPoint)
        {
            line_t line;
            line.line=QLine(startPoint,curPoint);
            lines.append(line);
            cleanUp();
            emit changed();
        }
    if(curPoint==startPoint)
    {
        int idx=-1;
        for(int i=0;i<blocks.length();i++)
            if((blocks[i].pos.x())<curPoint.x())
                if((blocks[i].pos.y())<curPoint.y())
                    if((blocks[i].pos.y()+blocks[i].block->height)>=curPoint.y())
                        if((blocks[i].pos.x()+blocks[i].block->width)>=curPoint.x())
                        {
                            idx=i;
                        }
        if(idx>=0)
        {
            double x=event->pos().x()/Settings::final()->gridSize();
            double y=event->pos().y()/Settings::final()->gridSize();
            QPointF p(x,y);
            blocks[idx].block->onclick(p-QPointF(blocks[idx].pos));
            blocks[idx].block->onrelease(p-QPointF(blocks[idx].pos));
        }
    }
    startPoint=QPoint(-1,-1);
    curPoint=QPoint(-1,-1);
    update();
}

void DigiView::save(QString where)
{
    if(where.isEmpty())
        where=fileName;
    if(where.isEmpty())
        where=QFileDialog::getSaveFileName(NULL,"Speichern unter...",QString(),"*.json");
    if(where.isEmpty())
        return;
    fileName=where;
    QJsonObject root;
    QJsonArray l;
    for(int i=0;i<lines.length();i++)
    {
        QJsonObject c;
        c.insert("x1",lines[i].line.x1());
        c.insert("y1",lines[i].line.y1());
        c.insert("x2",lines[i].line.x2());
        c.insert("y2",lines[i].line.y2());
        l.append(c);
    }
    root.insert("lines",l);
    QJsonArray g;
    for(int i=0;i<blocks.length();i++)
    {
        QJsonObject c;
        c.insert("x",blocks[i].pos.x());
        c.insert("y",blocks[i].pos.y());
        c.insert("name",blocks[i].block->name);
        QJsonArray pins;
        for(int j=0;j<blocks[i].block->pins.length();j++)
        {
            QJsonObject obj;
            obj.insert("type",blocks[i].block->pins[j].type);
            pins.append(obj);
        }
        c.insert("pins",pins);
        g.append(c);
    }
    root.insert("blocks",g);
    QJsonArray v;
    for(int i=0;i<vias.length();i++)
    {
        QJsonObject c;
        c.insert("x",vias[i].x());
        c.insert("y",vias[i].y());
        v.append(c);
    }
    root.insert("vias",v);
    QFile file(where);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.write(QJsonDocument(root).toJson());
    file.close();
    Settings::final()->setLastFile(fileName);
}

void DigiView::load(QString where)
{
    lines.clear();
    blocks.clear();
    vias.clear();
    QFile file(where);
    file.open(QFile::ReadOnly);
    QJsonObject root=QJsonDocument::fromJson(file.readAll()).object();
    file.close();
    QJsonArray l=root["lines"].toArray();
    for(int i=0;i<l.size();i++)
    {
        line_t c;
        c.line.setP1(QPoint(l[i].toObject()["x1"].toInt(),l[i].toObject()["y1"].toInt()));
        c.line.setP2(QPoint(l[i].toObject()["x2"].toInt(),l[i].toObject()["y2"].toInt()));
        lines.append(c);
    }
    QJsonArray v=root["vias"].toArray();
    for(int i=0;i<v.size();i++)
    {
        vias.append(QPoint(v[i].toObject()["x"].toInt(),v[i].toObject()["y"].toInt()));
    }
    QJsonArray g=root["blocks"].toArray();
    for(int i=0;i<g.size();i++)
    {
        block_t c;
        c.pos=QPoint(g[i].toObject()["x"].toInt(),g[i].toObject()["y"].toInt());
        c.block=0;
        BlockList list;
        for(int j=0;j<list.blocks.length();j++)
            if(list.blocks[j]->name==g[i].toObject()["name"].toString())
            {
                c.block=list.blocks[j]->clone();
            }
        QJsonArray pins=g[i].toObject()["pins"].toArray();
        for(int j=0;j<pins.size();j++)
        {
            QJsonObject pin=pins[j].toObject();
            if(c.block->pins.length()>j)
                c.block->pins[j].type=pin["type"].toBool();
        }
        if(c.block!=0)
            blocks.append(c);
    }
    for(int i=0;i<blocks.length();i++)
    {
        QJsonObject c;
        c.insert("x",blocks[i].pos.x());
        c.insert("y",blocks[i].pos.y());
        c.insert("name",blocks[i].block->name);
        g.append(c);
    }
    fileName=where;
    Settings::final()->setLastFile(fileName);
    update();
    cleanUp();
}

QPoint DigiView::toGrid(QPoint in)
{
    int x=round(in.x()/Settings::final()->gridSize());
    int y=round(in.y()/Settings::final()->gridSize());
    return QPoint(x,y);
}

void DigiView::timeout()
{
    QList<QList<bool> > done;
    bool ok=true;
    for(int i=0;i<blocks.length();i++)
    {
        done.append(QList<bool>());
        for(int j=0;j<blocks[i].block->pins.length();j++)
        {
            if(blocks[i].block->pins[j].direction==2)
                done[i].append(true);
            else
                done[i].append(false);
        }
    }
    QMap<QPair<int,int>,bool> states;
    for(int i=0;i<blocks.length();i++)
        for(int j=0;j<blocks[i].block->pins.length();j++)
            if(blocks[i].block->pins[j].direction==2)
            {
                QList<QPoint> points;
                points.append(blocks[i].block->pins[j].point+blocks[i].pos);
                bool state=blocks[i].block->getState(j);
                if(blocks[i].block->pins[j].type)
                    state=!state;
                QList<int> linesVisited;
                while(points.length()>0)
                {
                    QPoint p=points.takeFirst();
                    for(int k=0;k<lines.length();k++)
                    {
                        if((lines[k].line.p1()==p)||(lines[k].line.p2()==p))
                            if(!linesVisited.contains(k))
                            {
                                linesVisited.append(k);
                                lines[k].state=state;
                                QList<QPoint> inter=allIntersect(lines[k].line);
                                points.append(inter);
                                if(lines[k].line.p1()==p)
                                    points.removeAll(lines[k].line.p1());
                                else
                                    points.removeAll(lines[k].line.p2());
                            }
                    }
                    for(int k=0;k<blocks.length();k++)
                        for(int l=0;l<blocks[k].block->pins.length();l++)
                            if((blocks[k].block->pins[l].point+blocks[k].pos+QPoint(0,0))==p)
                            {
                                if(blocks[k].block->pins[l].direction==0)
                                {
                                    done[k][l]=true;
                                    states.insert(QPair<int,int>(k,l),state);
                                }
                                if(blocks[k].block->pins[l].direction==2)
                                    if(!((i==k)&&(j==l)))
                                    {
                                        ok=false;
                                        bar->showMessage("Zwei Ausgänge",1000);
                                    }
                            }
                }
            }
    QPair<int,int> offen;
    for(int i=0;i<done.length();i++)
    {
        bool roundOk=true;
        for(int j=0;j<done[i].length();j++)
            if(done[i][j]==false)
            {
                ok=false;
                roundOk=false;
                offen=QPair<int,int>(i,j);
            }
        if(!roundOk)
            bar->showMessage("Offene Eingänge",1000);
    }
    QList<QPair<int,int> > keys=states.keys();
    for(int i=0;i<keys.length();i++)
    {
        int k=keys[i].first;
        int l=keys[i].second;
        bool state=states[keys[i]];
        blocks[k].block->pins[l].state=state;
    }
    if(!ok)
    {
        for(int i=0;i<blocks.length();i++)
            for(int j=0;j<blocks[i].block->pins.length();j++)
                if(blocks[i].block->pins[j].direction==0)
                    blocks[i].block->pins[j].state=false;
        for(int i=0;i<lines.length();i++)
            lines[i].state=false;
        blocks[offen.first].block->pins[offen.second].state=true;
    }
    update();
}

void DigiView::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    int block=-1;
    bool ok=false;
    QMenu menu;
    QPoint p=toGrid(event->pos());
    QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
    for(int i=0;i<blocks.length();i++)
        if((blocks[i].pos.x()<=p.x())&&((blocks[i].pos.x()+blocks[i].block->width)>=p.x()))
            if((blocks[i].pos.y()<=p.y())&&((blocks[i].pos.y()+blocks[i].block->height)>=p.y()))
                block=i;
    int pin=-1;
    if(block>=0)
    {
        for(int i=0;i<blocks[block].block->pins.length();i++)
        {
            if(blocks[block].block->pins[i].point+blocks[block].pos==p)
                pin=i;
            if(blocks[block].block->pins[i].direction==0)
                if(blocks[block].block->pins[i].point+blocks[block].pos+QPoint(1,0)==p)
                    pin=i;
            if(blocks[block].block->pins[i].direction==2)
                if(blocks[block].block->pins[i].point+blocks[block].pos+QPoint(-1,0)==p)
                    pin=i;
        }
    }
    int line=-1;
    double dist=1.1;
    for(int i=0;i<lines.length();i++)
        if(onLine(lines[i].line,p))
        {
            QLine l(lines[i].line);
            double ndist;
            QPoint np;
            QPoint fp;
            if(QLineF(l.p1(),pf).length()<QLineF(l.p2(),pf).length())
                np=l.p1();
            else
                np=l.p2();
            if(QLineF(l.p1(),pf).length()>QLineF(l.p2(),pf).length())
                fp=l.p1();
            else
                fp=l.p2();
            if(l.y1()==l.y2())
            {
                ndist=double(l.y1())-pf.y();
                double ign;
                if(modf(pf.x(),&ign)<0.5)
                {
                    if(!(fp.x()>np.x()))
                        ndist=1.0;
                }
                else
                {
                    if(!(fp.x()<np.x()))
                        ndist=1.0;
                }
            }else
            {
                ndist=double(l.x1())-pf.x();
                double ign;
                if(modf(pf.y(),&ign)<0.5)
                {
                    qDebug()<<fp<<np;
                    if(!(fp.y()>np.y()))
                        ndist=1.0;
                }
                else
                {
                    if(!(fp.y()<np.y()))
                        ndist=1.0;
                }
            }
            ndist=fabs(ndist);
            double ax;
            double ay;
            qDebug()<<modf(pf.y(),&ay);
            if(ndist<dist)
            {
                dist=ndist;
                line=i;
            }
        }
    int via=-1;
    for(int i=0;i<vias.length();i++)
        if(vias[i]==p)
            via=i;
    int lcnt=0;
    for(int i=0;i<lines.length();i++)
        if(onLine(lines[i].line,p,true))
            lcnt++;
    QAction* delBlockAct=NULL;
    QAction* addViaAct=NULL;
    QAction* delLineAct=NULL;
    QAction* delViaAct=NULL;
    QAction* changePinAct=NULL;
    if(lcnt==2)
    {
        addViaAct=menu.addAction("Knotenpunkt hinzufügen");
        ok=true;
    }
    if(block>=0)
    {
        delBlockAct=menu.addAction("Block Löschen");
        ok=true;
    }
    if(via>=0)
    {
        delViaAct=menu.addAction("Knotenpunkt Löschen");
        ok=true;
    }
    if(line>=0)
    {
        delLineAct=menu.addAction("Linie Löschen");
        ok=true;
    }
    if(pin>=0)
    {
        changePinAct=menu.addAction("Pin");
        ok=true;
    }
    if(ok)
    {
        QAction* act=menu.exec(event->globalPos());
        if(block>=0)
            if(act==delBlockAct)
            {
                blocks.removeAt(block);
                emit changed();
            }
        if(via>=0)
            if(act==delViaAct)
            {
                vias.removeAt(via);
                emit changed();
            }
        if(pin>=0)
            if(act==changePinAct)
            {
                blocks[block].block->pins[pin].type=!blocks[block].block->pins[pin].type;
                emit changed();
            }
        if(lcnt==2)
            if(act==addViaAct)
            {
                vias.append(p);
                emit changed();
            }
        if(line>=0)
            if(act==delLineAct)
            {
                lines.removeAt(line);
                emit changed();
            }
    }
    cleanUp();
    update();
}

void DigiView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers()&Qt::ControlModifier)
    {
        event->accept();
        double size=Settings::final()->gridSize();
        size+=event->angleDelta().y()/10.0;
        if(size<10)
            size=10;
        if(size>50)
            size=50;
        Settings::final()->setGridSize(size,true);
    }
    update();
}

bool DigiView::interLine(QLine l1, QLine l2)
{
    if(l1==l2)
        return true;
    if(l1.p1()==l2.p2())
        if(l1.p2()==l2.p1())
            return true;
    if(l1.p1()==l1.p2())
        return false;
    if(l2.p1()==l2.p2())
        return false;
    if((l1.x1()==l1.x2())&&(l2.x1()!=l2.x2()))
        return false;
    if((l1.x1()!=l1.x2())&&(l2.x1()==l2.x2()))
        return false;
    if(l1.y1()==l1.y2())//Horizontal
        if((l1.y2()==l2.y2())||(l1.y1()==l2.y1()))//Gleiche Gerade
        {
            if((l1.x1()<=fmax(l2.x1(),l2.x2()))&&(l1.x1()>=fmin(l2.x1(),l2.x2())))
                return true;
            if((l1.x2()<=fmax(l2.x1(),l2.x2()))&&(l1.x2()>=fmin(l2.x1(),l2.x2())))
                return true;
            if((l2.x1()<=fmax(l1.x1(),l1.x2()))&&(l2.x1()>=fmin(l1.x1(),l1.x2())))
                return true;
            if((l2.x2()<=fmax(l1.x1(),l1.x2()))&&(l2.x2()>=fmin(l1.x1(),l1.x2())))
                return true;
        }
    return false;
}

bool DigiView::onLine(QLine line, QPoint point, bool proper)
{
    if(!proper)
    {
        if((fmax(line.x1(),line.x2())>=point.x())&&(fmin(line.x1(),line.x2())<=point.x()))
            if((fmax(line.y1(),line.y2())>=point.y())&&(fmin(line.y1(),line.y2())<=point.y()))
                return true;
    }
    else
    {
        if((fmax(line.x1(),line.x2())>=point.x())&&(fmin(line.x1(),line.x2())<=point.x()))
            if((fmax(line.y1(),line.y2())>=point.y())&&(fmin(line.y1(),line.y2())<=point.y()))
                if(!((point==line.p1())||(point==line.p2())))
                    return true;
    }
    return false;
}

void DigiView::cleanUp()
{
    for(int gcnt=0;gcnt<3;gcnt++)
    {
        for(int i=0;i<lines.length();i++)
            for(int j=0;j<lines.length();j++)
                if((i>=0)&&(j>=0))
                if(i!=j)
                {
                    if(onLine(lines[i].line,lines[j].line.p1()))
                        if(onLine(lines[i].line,lines[j].line.p2()))
                        {
                            lines.removeAt(i);
                            if(j>i)
                                j--;
                            lines.removeAt(j);
                            i--;
                        }
                }
        for(int i=0;i<lines.length();i++)
            for(int j=0;j<lines.length();j++)
                if((i>=0)&&(j>=0))
                if(i!=j)
                {
                    bool kill=false;
                    if(lines[i].line.p1()==lines[j].line.p1())
                        if(lines[i].line.p2()!=lines[j].line.p2())
                        {
                            if(lines[i].line.p2().x()==lines[j].line.p2().x())
                            {
                                kill=true;
                            }
                            if(lines[i].line.p2().y()==lines[j].line.p2().y())
                            {
                                kill=true;
                            }
                        }
                    if(lines[i].line.p2()==lines[j].line.p2())
                        if(lines[i].line.p1()!=lines[j].line.p1())
                        {
                            if(lines[i].line.p1().x()==lines[j].line.p1().x())
                            {
                                kill=true;
                            }
                            if(lines[i].line.p1().y()==lines[j].line.p1().y())
                            {
                                kill=true;
                            }
                        }
                    if(lines[i].line.p1()==lines[j].line.p2())
                        if(lines[i].line.p2()!=lines[j].line.p1())
                        {
                            if(lines[i].line.p2().x()==lines[j].line.p1().x())
                            {
                                kill=true;
                            }
                            if(lines[i].line.p2().y()==lines[j].line.p1().y())
                            {
                                kill=true;
                            }
                        }
                    if(lines[i].line.p2()==lines[j].line.p1())
                        if(lines[i].line.p1()!=lines[j].line.p2())
                        {
                            if(lines[i].line.p1().x()==lines[j].line.p2().x())
                            {
                                kill=true;
                            }
                            if(lines[i].line.p1().y()==lines[j].line.p2().y())
                            {
                                kill=true;
                            }
                        }
                    if(kill)
                    {
                        line_t line;
                        double len=0;
                        QList<QPoint> points;
                        points.append(lines[i].line.p1());
                        points.append(lines[i].line.p2());
                        points.append(lines[j].line.p1());
                        points.append(lines[j].line.p2());
                        line.line=QLine(points[0],points[1]);
                        for(int k=0;k<points.length();k++)
                            for(int l=0;l<points.length();l++)
                            {
                                double clen=QLineF(points[k],points[l]).length();
                                if(clen>len)
                                {
                                    line.line=QLine(points[k],points[l]);
                                    len=clen;
                                }
                            }
                        lines.append(line);
                        lines.removeAt(i);
                        if(j>i)
                            j--;
                        i--;
                        lines.removeAt(j);
                        j=lines.length();
                    }
                }
        QList<QPoint> testVias;
        for(int i=0;i<lines.length();i++)
        {
            testVias.append(lines[i].line.p1());
            testVias.append(lines[i].line.p2());
        }
        for(int j=0;j<blocks.length();j++)
            for(int k=0;k<blocks[j].block->pins.length();k++)
                testVias.append(blocks[j].block->pins[k].point+blocks[j].pos);
        testVias.append(vias);
        vias.clear();
        for(int i=0;i<testVias.length();i++)
        {
            int cnt=0;
            int num=0;
            QPoint point=testVias[i];
            for(int j=0;j<lines.length();j++)
                if(i!=j)
                {
                    if(onLine(lines[j].line,point,true))
                        cnt++;
                    if(onLine(lines[j].line,point))
                    {
                        num++;
                        cnt++;
                    }

                }
            for(int j=0;j<blocks.length();j++)
                for(int k=0;k<blocks[j].block->pins.length();k++)
                    if((blocks[j].block->pins[k].point+blocks[j].pos)==point)
                    {
                        num++;
                        cnt++;
                    }
            if(num>=2)
                if(cnt>2)
                {
                    vias.append(point);
                }
        }
        for(int i=0;i<lines.length();i++)
            for(int j=0;j<vias.length();j++)
                if((i>=0)&&(j>=0))
                    if(onLine(lines[i].line,vias[j],true))
                    {
                        line_t line=lines.takeAt(i);
                        i--;
                        line_t l1;
                        l1.state=line.state;
                        l1.line=QLine(line.line.p1(),vias[j]);
                        lines.append(l1);
                        line_t l2;
                        l2.state=line.state;
                        l2.line=QLine(line.line.p2(),vias[j]);
                        lines.append(l2);
                    }
    }
}

QList<QPoint> DigiView::allIntersect(QLine line)
{
    QList<QPoint> ret;
    ret.append(line.p1());
    ret.append(line.p2());
    for(int i=0;i<lines.length();i++)
    {
        if(onLine(lines[i].line,line.p1())||onLine(lines[i].line,line.p2()))
        {
            ret.append(lines[i].line.p1());
            ret.append(lines[i].line.p2());
        }
    }
    for(int i=0;i<lines.length();i++)
    {
        if(onLine(line,lines[i].line.p1()))
            ret.append(lines[i].line.p1());
        if(onLine(line,lines[i].line.p2()))
            ret.append(lines[i].line.p2());
    }
    for(int i=0;i<blocks.length();i++)
        for(int j=0;j<blocks[i].block->pins.length();j++)
            if(onLine(line,blocks[i].block->pins[j].point+blocks[i].pos))
                ret.append(blocks[i].block->pins[j].point+blocks[i].pos);
    return ret;
}

void DigiView::zoomReset()
{
    double size=Settings::final()->gridSize();
    size=20;
    if(size<10)
        size=10;
    if(size>50)
        size=50;
    Settings::final()->setGridSize(size,true);
    update();
}

void DigiView::zoomIn()
{
    double size=Settings::final()->gridSize();
    size+=10;
    if(size<10)
        size=10;
    if(size>50)
        size=50;
    Settings::final()->setGridSize(size,true);
    update();
}

void DigiView::zoomOut()
{
    double size=Settings::final()->gridSize();
    size-=10;
    if(size<10)
        size=10;
    if(size>50)
        size=50;
    Settings::final()->setGridSize(size,true);
    update();
}

QImage DigiView::exportImage()
{
    QPicture pic=exportPicture();
    QImage ret(pic.boundingRect().size(),QImage::Format_ARGB32);
    ret.fill(Qt::transparent);
    QPainter painter(&ret);
    painter.translate(-pic.boundingRect().topLeft());
    painter.drawPicture(0,0,pic);
    painter.end();
    return ret;
}

QPicture DigiView::exportPicture()
{
    QPicture picture;
    QPainter painter(&picture);
    for(int i=0;i<blocks.length();i++)
        painter.drawPixmap(blocks[i].pos*Settings::final()->gridSize(),blocks[i].block->drawBlock());
    QPen line(Qt::black);
    line.setWidth(3);
    for(int i=0;i<lines.length();i++)
    {
        if(lines[i].state)
            line.setColor(Qt::red);
        else
            line.setColor(Qt::black);
        painter.setPen(line);
        painter.drawLine(lines[i].line.p1()*Settings::final()->gridSize(),lines[i].line.p2()*Settings::final()->gridSize());
    }
    for(int i=0;i<vias.length();i++)
        painter.drawEllipse(vias[i]*Settings::final()->gridSize(),3,3);
    return picture;
}
