#include "impulsedialog.h"
#include "remotedrivelist.h"
#include "gdrive.h"
#include <QApplication>
#include <QInputDialog>
#include <QDesktopServices>
#include <QMessageBox>
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
#include <zip.h>
#include <math.h>


DigiView::DigiView(QWidget *parent) : QWidget(parent)
{
    lastSel=-1;
    recording=false;
    setFocus();
    lastContext=QTime::currentTime();
    startPoint=QPoint(-1,-1);
    curPoint=QPoint(-1,-1);
    BlockList list;
    setAcceptDrops(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer.setInterval(Settings::final()->simulationTime());
    setContextMenuPolicy(Qt::DefaultContextMenu);
    resizeNow();
    setMouseTracking(true);
}

void DigiView::paintEvent(QPaintEvent* event)
{
    if(!timer.isActive())
        error=false;
    int grid=Settings::final()->gridSize();
    QPainter painter(this);
    painter.setBrush(Settings::final()->background());
    painter.drawRect(0,0,width(),height());
    if(error)
    {
        painter.setPen(Qt::NoPen);
        QBrush brush(Qt::red);
        brush.setStyle(Qt::Dense7Pattern);
        painter.setBrush(brush);
        painter.drawRect(0,0,width(),height());
    }
    QPen lineSmall(Qt::gray);
    QPen lineLarge(Qt::gray);
    lineLarge.setWidth(3);
    switch(Settings::final()->gridType())
    {
    case Settings::GRID_LINES:
        for(int x=0;x<width();x+=grid)
            for(int y=0;y<height();y+=grid)
            {
                if(((y/grid)%Settings::final()->rasterSize())==0)
                    painter.setPen(lineLarge);
                else
                    painter.setPen(lineSmall);
                painter.drawLine(x,y,x+grid,y);
            }
        for(int x=0;x<width();x+=grid)
            for(int y=0;y<height();y+=grid)
            {
                if(((x/grid)%Settings::final()->rasterSize())==0)
                    painter.setPen(lineLarge);
                else
                    painter.setPen(lineSmall);
                painter.drawLine(x,y,x,y+grid);
            }
        break;
    case Settings::GRID_POINT:
    default:
        painter.setPen(Qt::gray);
        for(int x=0;x<width();x+=grid)
            for(int y=0;y<height();y+=grid)
                painter.drawPoint(x,y);
        for(int x=0;x<width();x+=(grid*Settings::final()->rasterSize()))
            for(int y=0;y<height();y+=(grid*Settings::final()->rasterSize()))
            {
                painter.drawLine(x,y,x+(grid*Settings::final()->rasterSize()),y);
            }
        for(int x=0;x<width();x+=(grid*Settings::final()->rasterSize()))
            for(int y=0;y<height();y+=(grid*Settings::final()->rasterSize()))
            {
                painter.drawLine(x,y,x,y+(grid*Settings::final()->rasterSize()));
            }
    }
    if(!dragGate.isEmpty())
    {
        Block* block=NULL;
        for(int i=0;i<BlockList::blocks.length();i++)
            if(BlockList::blocks[i]->name==dragGate)
                block=BlockList::blocks[i];
        if(block!=NULL)
        {
            painter.drawPicture(dragPos.x()*Settings::final()->gridSize(),dragPos.y()*Settings::final()->gridSize(),block->draw(false));
        }
    }
    for(auto item:items)
    {
        painter.drawPicture(toScreen(item->pos),item->draw(true));
    }
    QPen pen(Qt::green);
    QBrush brush(Qt::green);
    painter.setPen(pen);
    brush.setStyle(Qt::Dense6Pattern);
    brush.setColor(QColor::fromRgbF(0.0,1.0,0.0,0.5));
    painter.setBrush(brush);
    for(int i=0;i<selection.length();i++)
    {
        QRectF rect=items[selection[i]]->clickRect();
        rect.setTop(rect.top()*(Settings::final()->gridSize()));
        rect.setLeft(rect.left()*(Settings::final()->gridSize()));
        rect.setBottom(rect.bottom()*Settings::final()->gridSize());
        rect.setRight(rect.right()*Settings::final()->gridSize());
        painter.drawRect(rect);
    }
    if(!drag)
        if(curPoint!=QPoint(-1,-1))
            if(!(((curPoint.x()!=startPoint.x())&&(curPoint.y()==startPoint.y()))||((curPoint.y()!=startPoint.y())&&(curPoint.x()==startPoint.x()))))
            {
                QRect rect(startPoint*Settings::final()->gridSize(),curPoint*Settings::final()->gridSize());
                rect.normalized();
                painter.drawRect(rect);
            }
    if(lastSel>=0)
    {
        QPen pen(Qt::gray);
        QBrush brush(Qt::gray);
        painter.setPen(pen);
        brush.setStyle(Qt::Dense6Pattern);
        painter.setBrush(brush);
        QRectF rect=blocks[lastSel]->clickRect();
        rect.setTop(rect.top()*(Settings::final()->gridSize()));
        rect.setLeft(rect.left()*(Settings::final()->gridSize()));
        rect.setBottom(rect.bottom()*Settings::final()->gridSize());
        rect.setRight(rect.right()*Settings::final()->gridSize());
        painter.drawRect(rect);
    }
    painter.setBrush(Qt::NoBrush);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    for(int i=0;i<blocks.length();i++)
    {
        for(int j=0;j<blocks[i]->pins.length();j++)
            if(blocks[i]->pins[j].state==2)
            {
                QPen pen(QColor::fromRgbF(1,0.5,0));
                pen.setWidth(0);
                painter.setPen(pen);
                for(int r=5;r<(Settings::final()->gridSize());r+=5)
                    painter.drawEllipse((blocks[i]->pins[j].pos() * Settings::final()->gridSize()),r,r);
            }
    }
    QPen line(Qt::black);
    line.setWidth(3);
    painter.setPen(line);
    if(!drag)
        if(curPoint!=QPoint(-1,-1))
            if(((curPoint.x()!=startPoint.x())&&(curPoint.y()==startPoint.y()))||((curPoint.y()!=startPoint.y())&&(curPoint.x()==startPoint.x())))
                painter.drawLine(startPoint*Settings::final()->gridSize(),curPoint*Settings::final()->gridSize());
    painter.setPen(Qt::NoPen);
    /*int d=(2.0*Settings::final()->penWidth()*Settings::final()->gridSize());
    for(int i=0;i<vias.length();i++)
    {
        painter.setBrush(Qt::black);
        painter.drawEllipse(vias[i]*Settings::final()->gridSize(),d-1,d-1);
        QList<int> linei=linesAtPoint(vias[i]);
        for(int j=0;j<linei.length();j++)
        {
            line_t c=lines[linei[j]];
            QPoint cp;
            QPoint op;
            if(c.line.p1()==vias[i])
            {
                cp=c.line.p1();
                op=c.line.p2();
            }
            else
            {
                cp=c.line.p2();
                op=c.line.p1();
            }
            painter.setBrush(c.color);
            if(cp.x()==op.x())//Senkrecht
            {
                if(cp.y()<op.y())//Unten
                    painter.drawPie((cp.x()*Settings::final()->gridSize())-d,(cp.y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,-16*45,-16*90);
                else
                    painter.drawPie((cp.x()*Settings::final()->gridSize())-d,(cp.y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,16*45,16*90);
            }else//Waaagrecht
            {
                if(cp.x()<op.x())//Rechts
                    painter.drawPie((cp.x()*Settings::final()->gridSize())-d,(cp.y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,-16*45,16*90);
                else
                    painter.drawPie((cp.x()*Settings::final()->gridSize())-d,(cp.y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,16*135,16*90);
            }
        }
        for(int j=0;j<blocks.length();j++)
            for(int k=0;k<blocks[j]->pins.length();k++)
                if(blocks[j]->pins[k].pos()==vias[i])
                {
                    if(blocks[j]->pins[k].direction==0)
                        painter.drawPie((vias[i].x()*Settings::final()->gridSize())-d,(vias[i].y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,-16*45,16*90);
                    else
                        painter.drawPie((vias[i].x()*Settings::final()->gridSize())-d,(vias[i].y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,16*135,16*90);
                }
    }*/
    //resizeNow();
}
void DigiView::dragEnterEvent(QDragEnterEvent *event)
{
    clearSelection();
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
        if(QApplication::mouseButtons()&Qt::LeftButton)
            dragMany=false;
        else
            dragMany=true;
        event->acceptProposedAction();
        //qDebug()<<event;
        update();
    }
}

void DigiView::dragMoveEvent(QDragMoveEvent *event)
{
    clearSelection();
    dragPos=toGrid(event->pos())-QPoint(1,1);
    update();
}

void DigiView::dragLeaveEvent(QDragLeaveEvent *event)
{
    clearSelection();
    dragGate="";
    update();
}

void DigiView::dropEvent(QDropEvent *event)
{
    clearSelection();
    dragPos=toGrid(event->pos())-QPoint(1,1);
    addBlock(dragPos.toPoint(),dragGate);
    dragGate="";
    update();
}

void DigiView::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    if(dragGate!="")
        if(dragMany)
        {
            event->accept();
            dragPos=toGrid(event->pos())-QPoint(1,1);
            addBlock(dragPos.toPoint(),dragGate);
            if(event->button()!=Qt::LeftButton)
                dragGate="";
            update();
            lastContext=QTime::currentTime();
            return;
        }
    dragged=false;
    QWidget::mousePressEvent(event);
    if((event->button()==Qt::LeftButton)&&(event->modifiers()==Qt::NoModifier))
    {
        bool clear=true;
        startPoint=toGrid(event->pos());
        curPoint=startPoint;
        int idx=-1;
        for(int i=0;i<blocks.length();i++)
            if(blocks[i]->clickRect().contains(curPoint))
            {
                idx=i;
            }
        int pin=-1;
        if(idx>=0)
            if(blocks[idx]->checkable)
                lastSel=idx;
        if(idx>=0)
            for(int i=0;i<blocks[idx]->pins.length();i++)
                if(startPoint==blocks[idx]->pins[i].pos())
                    pin=i;
        drag=false;
        dragIdx=-1;
        for(int i=0;i<items.length();i++)
            if(items[i]->clickRect().contains(curPoint))
            {
                dragIdx=i;
            }
        if((pin==-1)&&(idx>=0))
        {
            clear=false;
            startBlock=blocks[idx]->pos;
            drag=true;
        }
        if(idx>=0)
        {
            double x=event->pos().x()/Settings::final()->gridSize();
            double y=event->pos().y()/Settings::final()->gridSize();
            QPointF p(x,y);
            blocks[idx]->onpress(blocks[idx]->unmap(p));
        }
        if(clear)
            clearSelection();
    }
    if((event->button()==Qt::LeftButton)&&(event->modifiers()==Qt::CTRL))
    {
        QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
        for(int i=0;i<items.length();i++)
            if(items[i]->clickRect().contains(pf))
            {
                if(selection.contains(i))
                    selection.removeAll(i);
                else
                    selection.append(i);
            }
    }
    update();
}

void DigiView::mouseMoveEvent(QMouseEvent *event)
{
    if(dragGate!="")
        if(dragMany)
        {
            clearSelection();
            dragPos=toGrid(event->pos())-QPoint(1,1);
            update();
            return;
        }
    if(!event->buttons())
        return;
    QWidget::mouseMoveEvent(event);
    if((event->buttons()==Qt::LeftButton)&&(event->modifiers()==Qt::NoModifier))
    {
        curPoint=toGrid(event->pos());
        if(drag)
        {
            dragged=true;
            if(dragIdx>=0)
                items[dragIdx]->pos+=curPoint-startPoint;
            for(auto i:selection)
                if(i!=dragIdx)
                    items[i]->pos+=curPoint-startPoint;
            startPoint=curPoint;
        }
        else
            clearSelection();
    }
    update();
}

void DigiView::mouseReleaseEvent(QMouseEvent *event)
{
    if(dragGate!="")
        if(dragMany)
        {
        }
    QWidget::mouseReleaseEvent(event);
    if((event->button()==Qt::LeftButton)&&(event->modifiers()==Qt::NoModifier))
    {
        QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
        //clearSelection();
        curPoint=toGrid(event->pos());
        if(!drag)
        {
            if((startPoint.x()==curPoint.x())||(startPoint.y()==curPoint.y()))
                if(startPoint!=curPoint)
                {
                    Line* line=new Line;
                    line->line=QLine(startPoint,curPoint);
                    for(int i=0;i<lines.length();i++)
                        if(onLine(lines[i]->line,startPoint)||onLine(lines[i]->line,curPoint))
                            line->color=lines[i]->color;
                    lines.append(line);
                    items.append(line);
                    cleanUp();
                    emit changed();
                }
            if(curPoint!=startPoint)
                if(!(((curPoint.x()!=startPoint.x())&&(curPoint.y()==startPoint.y()))||((curPoint.y()!=startPoint.y())&&(curPoint.x()==startPoint.x()))))
                {
                    QRectF sel(curPoint,startPoint);
                    for(int i=0;i<items.length();i++)
                        if(items[i]->clickRect().intersects(sel))
                            selection.append(i);
                }
        }
        if(curPoint==startPoint)
        {
            int idx=-1;
            for(int i=0;i<blocks.length();i++)
                if(blocks[i]->clickRect().contains(curPoint))
                {
                    idx=i;
                }
            if(idx>=0)
            {
                double x=event->pos().x()/Settings::final()->gridSize();
                double y=event->pos().y()/Settings::final()->gridSize();
                QPointF p(x,y);
                if(!dragged)
                {
                    blocks[idx]->onclick(blocks[idx]->unmap(p));
                    blocks[idx]->onrelease(blocks[idx]->unmap(p));
                }
            }
        }
        startPoint=QPoint(-1,-1);
        curPoint=QPoint(-1,-1);
    }
    cleanUp();
    update();
}

bool DigiView::save(QUrl where)
{
    if(where.isEmpty())
        where=fileName;
    if(where.isEmpty())
        return false;
    fileName=where;
    QString w=where.toLocalFile();
    if(!w.endsWith(".qdigi"))
        w+=".qdigi";
    QJsonObject root=exportJSON();
    QByteArray data=QJsonDocument(root).toJson(QJsonDocument::Compact);
    if(where.scheme()=="file")
    {
        int err=0;
        QFile file(w);
        file.open(QFile::WriteOnly|QFile::Truncate);
        file.write(data);
        file.close();
        Settings::final()->setLastFile(fileName);
    }
    else
    {
        RemoteDriveList list;
        for(int i=0;i<list.drives.length();i++){
            if(list.drives[i]->prefix==where.scheme())
            {
                if(list.drives[i]->save(where,data))
                {
                    Settings::final()->setLastFile(fileName);
                    return true;
                }
            }
        }
    }
    return true;
}

void DigiView::load(QUrl where)
{
    lines.clear();
    for(int i=0;i<blocks.length();i++)
        blocks[i]->deleteLater();
    blocks.clear();
    vias.clear();
    texts.clear();
    items.clear();
    if(where.scheme()=="file")
    {
        QFile file(where.toLocalFile());
        file.open(QFile::ReadOnly);
        if(!file.isOpen())
        {
            qDebug()<<"Cannot Open";
            return;
        }
        else
            qDebug()<<"Have Open"<<file.fileName();
        QByteArray header=file.read(4);
        qDebug()<<file.size();
        bool isZip=false;
        if(header.at(0)==0x50)
            if(header.at(1)==0x4B)
                if(header.at(2)==0x03)
                    if(header.at(3)==0x04)
                    {
                        isZip=true;
                    }
        file.seek(0);
        fileName=where;
        Settings::final()->setLastFile(fileName.toString());
        if(!isZip)
        {
            QByteArray data=file.readAll();
            loadJson(data);
            qDebug()<<"Data is"<<data;
            file.close();
        }
        else
        {
            int err;
            zip_t *arch=zip_fdopen(file.handle(),0,&err);
            zip_file_t* vfile=zip_fopen(arch,"version.txt",0);
            QByteArray version(128,0);
            int len=zip_fread(vfile,version.data(),version.length());
            version=version.left(len);
            version=version.replace("\r","");
            version=version.replace("\n","");
            zip_fclose(vfile);
            if(version=="0.1")
            {
                QByteArray data;
                zip_file_t* dfile=zip_fopen(arch,"data.json",0);
                int len;
                do {
                    QByteArray buf(1024,0);
                    len=zip_fread(dfile,buf.data(),buf.length());
                    buf=buf.left(len);
                    data+=buf;
                }while(len>0);
                loadJson(data);
            }
            else
            {
                QMessageBox::warning(NULL,"QDigi Fehler","Unbekanntes Dateiformat");
            }
            zip_close(arch);
            file.close();
        }
    }
}

QPoint DigiView::toGrid(QPoint in)
{
    int x=round(in.x()/Settings::final()->gridSize());
    int y=round(in.y()/Settings::final()->gridSize());
    return QPoint(x,y);
}

void DigiView::timeout()
{
    error=false;
    QList<QList<bool> > done;
    bool ok=true;
    for(int i=0;i<blocks.length();i++)
    {
        done.append(QList<bool>());
        for(int j=0;j<blocks[i]->pins.length();j++)
        {
            if(blocks[i]->pins[j].direction==2)
                done[i].append(true);
            else
                done[i].append(false);
        }
    }
    QMap<QPair<int,int>,bool> cache;
    QMap<QPair<int,int>,QPair<bool,QColor> > states;
    for(int i=0;i<blocks.length();i++)
        for(int j=0;j<blocks[i]->pins.length();j++)
            if(blocks[i]->pins[j].direction==2)
                cache.insert(QPair<int,int>(i,j),blocks[i]->getState(j));
    for(int i=0;i<blocks.length();i++)
        for(int j=0;j<blocks[i]->pins.length();j++)
            if(blocks[i]->pins[j].direction==2)
            {
                bool state=cache[QPair<int,int>(i,j)];
                QList<int> list;
                QList<QPair<int,int> > inputs=getItemsForOutput(blocks[i]->pins[j].pos(),&list);
                for(auto item:list)
                {
                    Item* cur=items[item];
                    if(isLine(cur))
                    {
                        Line* line=(Line*)cur;
                        line->state=state;
                    }
                    if(isVia(cur))
                    {
                        Via* via=(Via*)cur;
                        via->state=state;
                    }
                }
                for(auto input:inputs)
                {
                    int k=input.first;
                    int l=input.second;
                    if(blocks[k]->pins[l].direction==0)
                    {
                        done[k][l]=true;
                        states.insert(QPair<int,int>(k,l),QPair<bool,QColor>(state,Qt::black));
                    }
                    if(blocks[k]->pins[l].direction==2)
                    {
                        if(!((i==k)&&(j==l)))
                        {
                            ok=false;
                            bar->showMessage("Zwei Ausgänge",1000);
                        }
                    }
                }
            }
    /*QList<QPair<QPoint,QColor> > points;
                points.append(QPair<QPoint,QColor> (blocks[i]->pins[j].pos(),Qt::black));
                QPoint sp=blocks[i]->pins[j].pos();
                QPair<int,int> idx(i,j);
                bool state=cache[idx];
                if(blocks[i]->pins[j].type)
                    state=!state;
                QList<int> linesVisited;
                while(points.length()>0)
                {
                    QPair<QPoint,QColor> pp=points.takeFirst();
                    QPoint p=pp.first;
                    for(int k=0;k<lines.length();k++)
                    {
                        if((lines[k].line.p1()==p)||(lines[k].line.p2()==p))
                            if(!linesVisited.contains(k))
                            {
                                linesVisited.append(k);
                                lines[k].state=state;
                                points.append(QPair<QPoint,QColor>(lines[k].line.p1(),lines[k].color));
                                points.append(QPair<QPoint,QColor>(lines[k].line.p2(),lines[k].color));
                                if((lines[k].line.p1()==sp)||(lines[k].line.p2()==sp))
                                    blocks[i]->pins[j].color=lines[k].color;
                                QPoint dp=lines[k].line.p2();
                                if(lines[k].line.p1()==p)
                                    dp=lines[k].line.p1();
                                for(int l=0;l<points.length();l++)
                                    if(points[l].first==dp)
                                    {
                                        points.removeAt(l);
                                        l--;
                                    }
                            }
                    }
                    for(int k=0;k<blocks.length();k++)
                        for(int l=0;l<blocks[k]->pins.length();l++)
                            if((blocks[k]->pins[l].pos()+QPoint(0,0))==p)
                            {
                                if(blocks[k]->pins[l].direction==0)
                                {
                                    done[k][l]=true;
                                    states.insert(QPair<int,int>(k,l),QPair<bool,QColor>(state,pp.second));
                                }
                                if(blocks[k]->pins[l].direction==2)
                                {
                                    if(!((i==k)&&(j==l)))
                                    {
                                        ok=false;
                                        bar->showMessage("Zwei Ausgänge",1000);
                                    }
                                }
                            }
                }
            }*/
    QList<QPair<int,int>> offen;
    for(int i=0;i<done.length();i++)
    {
        bool roundOk=true;
        for(int j=0;j<done[i].length();j++)
            if(done[i][j]==false)
            {
                ok=false;
                roundOk=false;
                offen.append(QPair<int,int>(i,j));
            }
        if(!roundOk)
            bar->showMessage("Offene Eingänge",1000);
    }
    QList<QPair<int,int> > keys=states.keys();
    for(int i=0;i<keys.length();i++)
    {
        int k=keys[i].first;
        int l=keys[i].second;
        bool state=states[keys[i]].first;
        QColor col=states[keys[i]].second;
        blocks[k]->pins[l].state=state;
        blocks[k]->pins[l].color=col;
    }
    if(!ok)
    {
        error=true;
        /*for(int i=0;i<blocks.length();i++)
            for(int j=0;j<blocks[i]->pins.length();j++)
                if(blocks[i]->pins[j].direction==0)
                    blocks[i]->pins[j].state=false;
        for(int i=0;i<lines.length();i++)
            lines[i]->state=false;*/
        for(int i=0;i<offen.length();i++)
            blocks[offen[i].first]->pins[offen[i].second].state=2;
    }
    if(recording==true)
    {
        for(int i=0;i<blocks.length();i++)
        {
            if(blocks[i]->name=="OUT")
            {
                dialog.widget->pushValue(i,blocks[i]->pins.first().state);
            }
        }
        for(int i=0;i<blocks[recorder]->pins.length();i++)
        {
            QPoint p=blocks[recorder]->pins[i].pos();
            if(getNet(QLine(p,p)).length()>0)
            {
                dialog.widget->pushValue(-1-i,blocks[recorder]->getState(i));
            }
        }
        dialog.widget->nextField();
    }
    update();
}

void DigiView::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    int lastTime=lastContext.msecsTo(QTime::currentTime());
    if((lastTime<10)&&(lastTime>=0))
        return;
    int block=-1;
    QList<int> clickedItems;
    bool ok=false;
    QMenu menu;
    QPoint p=toGrid(event->pos());
    QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
    for(int i=0;i<blocks.length();i++)
        if(QRectF(blocks[i]->clickRect()).contains(pf))
            block=i;
    int pin=-1;
    if(block>=0)
    {
        for(int i=0;i<blocks[block]->pins.length();i++)
        {
            if(blocks[block]->pins[i].pos()==p)
                pin=i;
            if(blocks[block]->pins[i].direction==0)
                if(blocks[block]->pins[i].pos()+QPoint(1,0)==p)
                    pin=i;
            if(blocks[block]->pins[i].direction==2)
                if(blocks[block]->pins[i].pos()+QPoint(-1,0)==p)
                    pin=i;
        }
    }
    for(int i=0;i<items.length();i++)
    {
        if(items[i]->clickRect().contains(pf))
            clickedItems.append(i);
    }
    QAction* delBlockAct=NULL;
    QAction* impulseBlockAct=NULL;
    QAction* addViaAct=NULL;
    QAction* delLineAct=NULL;
    QAction* delSelAct=NULL;
    QAction* delLineNetAct=NULL;
    QAction* delViaAct=NULL;
    QAction* delTextAct=NULL;
    QAction* changePinAct=NULL;
    QAction* addTextAct=NULL;
    QMap<QAction*,int> blockDefAct;
    QMap<QAction*,QColor> setColorAction;
    QMap<QAction*,QColor> setSelectionColorAction;
    QList<QAction*> altAction;
    QStringList alt;
    int lcnt=0;
    if(pin>=0)
    {
        changePinAct=menu.addAction("Pin");
        ok=true;
    }
    for(auto clicked:clickedItems)
    {
        Item* item=items[clicked];
        if(isLine(item))
            lcnt++;
    }
    Block* blk=NULL;
    Line* line=NULL;
    Via* via=NULL;
    Text* text=NULL;
    bool colorC=false;
    for(auto clicked:clickedItems)
    {
        Item* item=items[clicked];
        if(isBlock(item))
        {
            blk=(Block*)item;
            alt=blk->alt;
            int cnt=0;
            QList<QString> keys=blk->contextMenu.keys();
            for(auto key:keys)
            {
                int val=blk->contextMenu[key];
                QAction* act=menu.addAction(key);
                blockDefAct.insert(act,val);
                cnt++;
            }
            if(cnt>0)
                menu.addSeparator();
            if(blk->name=="Large-IN")
            {
                impulseBlockAct=menu.addAction("Impulsdiagramm erstellen");
            }
            delBlockAct=menu.addAction("Block Löschen");
            if(alt.length()>0)
            {
                QMenu* subMenu=menu.addMenu("Umwandeln in");
                for(int i=0;i<alt.length();i++)
                {
                    altAction.append(subMenu->addAction(alt[i]));
                }
            }
            ok=true;
        }
        if(!isVia(item))
            if(!colorC)
            {
                colorC=true;
                QList<QPair<QColor,QString> > colors=loadColorProfile();
                QMenu* cm=menu.addMenu("Farbe ändern");
                for(int i=0;i<colors.length();i++)
                {
                    QPixmap map(24,24);
                    map.fill(colors[i].first);
                    QAction* act=cm->addAction(QIcon(map),colors[i].second);
                    setColorAction.insert(act,colors[i].first);
                }
                ok=true;
            }
        if(isVia(item))
        {
            via=(Via*)item;
            delViaAct=menu.addAction("Knotenpunkt Löschen");
            ok=true;
        }
        if(isText(item))
        {
            text=(Text*)item;
            delTextAct=menu.addAction("Text Löschen");
            ok=true;
        }
        if(isLine(item))
        {
            line=(Line*)item;
            delLineAct=menu.addAction("Linie Löschen");
            delLineNetAct=menu.addAction("Linien bis Knotenpunk löschen");
            ok=true;
        }
    }
    if(lcnt>=2)
    {
        addViaAct=menu.addAction("Knotenpunkt hinzufügen");
        ok=true;
    }
    if(selection.length()>0)
    {
        delSelAct=menu.addAction("Auswahl löschen");
        QList<QPair<QColor,QString> > colors=loadColorProfile();
        QMenu* cm=menu.addMenu("Auswahlfarbe ändern");
        for(int i=0;i<colors.length();i++)
        {
            QPixmap map(24,24);
            map.fill(colors[i].first);
            QAction* act=cm->addAction(QIcon(map),colors[i].second);
            setSelectionColorAction.insert(act,colors[i].first);
        }
        ok=true;
    }
    addTextAct=menu.addAction("Text hinzufügen");
    ok=true;
    if(pin>=0)
    {
        ok=true;
    }
    if(ok)
    {
        QAction* act=menu.exec(event->globalPos());
        if(act!=NULL)
        {
            if(blk!=NULL)
            {
                if(blockDefAct.contains(act))
                {
                    int func=blockDefAct[act];
                    blk->execContext(func);
                    return;
                }
                if(act==delBlockAct)
                {
                    clearSelection();
                    lastSel=-1;
                    blk->deleteLater();
                    blocks.removeAll(blk);
                    items.removeAll(blk);
                    emit changed();
                    return;
                }
                if(blk->name=="Large-IN")
                    if(act==impulseBlockAct)
                    {
                        dialog.show();
                        recording=true;
                        recorder=block;
                        return;
                    }
                for(int i=0;i<alt.length();i++)
                    if(altAction[i]==act)
                    {
                        clearSelection();
                        items.removeAll(blk);
                        blocks.removeAll(blk);
                        addBlock(blk->pos,alt[i]);
                        Block* nblk=blocks.last();
                        for(int i=0;i<nblk->pins.length();i++)
                            nblk->pins[i].parent=nblk;
                        for(int i=0;i<nblk->pins.length();i++)
                            nblk->pins[i].type=blk->pins[i].type;
                        blk->deleteLater();
                        emit changed();
                        clearSelection();
                        qDebug()<<blocks;
                        update();
                        return;
                    }
            }
            if(selection.length()>0)
            {
                if(act==delSelAct)
                {
                    lastSel=-1;
                    deleteSelection();
                    clearSelection();
                    return;
                }
                if(setSelectionColorAction.contains(act))
                {
                    QColor c=setSelectionColorAction[act];
                    for(auto item:selection)
                        items[item]->color=c;
                    emit changed();
                    clearSelection();
                    return;
                }
            }
            if(act==addTextAct)
            {
                clearSelection();
                bool ok;
                QString message=QInputDialog::getText(NULL,"QDigi","Text Einfügen",QLineEdit::Normal,QString(),&ok);
                if(ok)
                {
                    Text* text=new Text;
                    text->pos=p;
                    text->text=message;
                    texts.append(text);
                    items.append(text);
                }
                deleteSelection();
                return;
            }
            if(via!=NULL)
                if(act==delViaAct)
                {
                    clearSelection();
                    vias.removeAll(via);
                    items.removeAll(via);
                    emit changed();
                    return;
                }
            if(via==NULL)
            {
                if(setColorAction.contains(act))
                {
                    clearSelection();
                    QColor c=setColorAction[act];
                    for(auto idx:clickedItems)
                        items[idx]->color=c;
                    emit changed();
                    clearSelection();
                    return;
                }
            }
            if(pin>=0)
                if(act==changePinAct)
                {
                    clearSelection();
                    blk->pins[pin].type=!blk->pins[pin].type;
                    emit changed();
                    return;
                }
            if(text!=NULL)
                if(act==delTextAct)
                {
                    clearSelection();
                    texts.removeAll(text);
                    items.removeAll(text);
                    emit changed();
                    return;
                }
            if(lcnt==2)
                if(act==addViaAct)
                {
                    clearSelection();
                    Via* via=new Via;
                    vias.append(via);
                    items.append(via);
                    emit changed();
                    return;
                }
            if(line!=NULL)
                if(act==delLineAct)
                {
                    clearSelection();
                    lines.removeAll(line);
                    items.removeAll(line);
                    emit changed();
                    return;
                }
            if(line!=NULL)
                if(act==delLineNetAct)
                {
                    clearSelection();
                    QList<QPoint> points;
                    points.append(line->line.p1());
                    points.append(line->line.p2());
                    lines.removeAll(line);
                    items.removeAll(line);
                    while(points.length()>0)
                    {
                        int cnt=0;
                        int del=-1;
                        QPoint point=points.takeFirst();
                        for(int i=0;i<lines.length();i++)
                        {
                            if(onLine(lines[i]->line,point,true))
                                cnt=100;
                            if(line->line.p1()==point)
                            {
                                cnt++;
                                del=i;
                            }
                            else if(line->line.p2()==point)
                            {
                                del=i;
                                cnt++;
                            }
                        }
                        for(int i=0;i<vias.length();i++)
                            if(vias[i]->pos==point)
                                cnt=100;
                        if(cnt==1)
                        {
                            points.append(lines[del]->line.p1());
                            points.append(lines[del]->line.p2());
                            lines.removeAt(del);
                        }
                    }
                    emit changed();
                    return;
                }
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
                    if((i<lines.length())&&(j<lines.length()))
                        if(i!=j)
                        {
                            if(onLine(lines[i]->line,lines[j]->line.p1()))
                                if(onLine(lines[i]->line,lines[j]->line.p2()))
                                {
                                    items.removeAll((Item*)lines[i]);
                                    lines.removeAt(i);
                                    if(j>i)
                                        j--;
                                    items.removeAll((Item*)lines[j]);
                                    lines.removeAt(j);
                                    i--;
                                }
                        }
        for(int i=0;i<lines.length();i++)
            for(int j=0;j<lines.length();j++)
                if((i>=0)&&(j>=0))
                    if((i<lines.length())&&(j<lines.length()))
                        if(i!=j)
                        {
                            bool kill=false;
                            QPoint kp;
                            if(lines[i]->line.p1()==lines[j]->line.p1())
                                if(lines[i]->line.p2()!=lines[j]->line.p2())
                                {
                                    if(lines[i]->line.p2().x()==lines[j]->line.p2().x())
                                    {
                                        kill=true;
                                        kp=lines[i]->line.p1();
                                    }
                                    if(lines[i]->line.p2().y()==lines[j]->line.p2().y())
                                    {
                                        kp=lines[i]->line.p1();
                                        kill=true;
                                    }
                                }
                            if(lines[i]->line.p2()==lines[j]->line.p2())
                                if(lines[i]->line.p1()!=lines[j]->line.p1())
                                {
                                    if(lines[i]->line.p1().x()==lines[j]->line.p1().x())
                                    {
                                        kp=lines[i]->line.p2();
                                        kill=true;
                                    }
                                    if(lines[i]->line.p1().y()==lines[j]->line.p1().y())
                                    {
                                        kp=lines[i]->line.p2();
                                        kill=true;
                                    }
                                }
                            if(lines[i]->line.p1()==lines[j]->line.p2())
                                if(lines[i]->line.p2()!=lines[j]->line.p1())
                                {
                                    if(lines[i]->line.p2().x()==lines[j]->line.p1().x())
                                    {
                                        kp=lines[i]->line.p1();
                                        kill=true;
                                    }
                                    if(lines[i]->line.p2().y()==lines[j]->line.p1().y())
                                    {
                                        kp=lines[i]->line.p1();
                                        kill=true;
                                    }
                                }
                            if(lines[i]->line.p2()==lines[j]->line.p1())
                                if(lines[i]->line.p1()!=lines[j]->line.p2())
                                {
                                    if(lines[i]->line.p1().x()==lines[j]->line.p2().x())
                                    {
                                        kp=lines[i]->line.p2();
                                        kill=true;
                                    }
                                    if(lines[i]->line.p1().y()==lines[j]->line.p2().y())
                                    {
                                        kp=lines[i]->line.p2();
                                        kill=true;
                                    }
                                }
                            if(kill)
                            {
                                for(int i=0;i<vias.length();i++)
                                    if(vias[i]->pos==kp)
                                        kill=false;
                            }
                            if(kill)
                            {
                                Line* line=new Line;
                                double len=0;
                                QList<QPoint> points;
                                points.append(lines[i]->line.p1());
                                points.append(lines[i]->line.p2());
                                points.append(lines[j]->line.p1());
                                points.append(lines[j]->line.p2());
                                line->line=QLine(points[0],points[1]);
                                for(int k=0;k<points.length();k++)
                                    for(int l=0;l<points.length();l++)
                                    {
                                        double clen=QLineF(points[k],points[l]).length();
                                        if(clen>len)
                                        {
                                            line->line=QLine(points[k],points[l]);
                                            len=clen;
                                        }
                                    }
                                line->color=lines[i]->color;
                                lines.append(line);
                                items.append(line);
                                items.removeAll(lines[i]);
                                lines.removeAt(i);
                                if(j>i)
                                    j--;
                                i--;
                                items.removeAll(lines[j]);
                                lines.removeAt(j);
                                j=lines.length();
                            }
                        }
        QList<QPoint> testVias;
        for(int i=0;i<lines.length();i++)
        {
            testVias.append(lines[i]->line.p1());
            testVias.append(lines[i]->line.p2());
        }
        for(int j=0;j<blocks.length();j++)
            for(int k=0;k<blocks[j]->pins.length();k++)
                testVias.append(blocks[j]->pins[k].pos());
        for(int i=0;i<vias.length();i++)
            testVias.append(vias[i]->pos);
        for(int i=0;i<vias.length();i++)
        {
            items.removeAll(vias[i]);
            vias[i]->deleteLater();
        }
        vias.clear();
        for(int i=0;i<testVias.length();i++)
        {
            int cnt=0;
            int num=0;
            QPoint point=testVias[i];
            for(int j=0;j<lines.length();j++)
                if(i!=j)
                {
                    if(onLine(lines[j]->line,point,true))
                        cnt++;
                    if(onLine(lines[j]->line,point))
                    {
                        num++;
                        cnt++;
                    }

                }
            for(int j=0;j<blocks.length();j++)
                for(int k=0;k<blocks[j]->pins.length();k++)
                    if((blocks[j]->pins[k].pos())==point)
                    {
                        num++;
                        cnt++;
                    }
            if(num>=2)
                if(cnt>2)
                {
                    bool c=false;
                    for(int i=0;i<vias.length();i++)
                        if(vias[i]->pos==point)
                            c=true;
                    if(!c)
                    {
                        Via* v=new Via;
                        v->pos=point;
                        vias.append(v);
                        items.append(v);
                    }
                }
        }
        for(int i=0;i<lines.length();i++)
            for(int j=0;j<vias.length();j++)
                if((i>=0)&&(j>=0))
                    if((i<lines.length())&&(j<vias.length()))
                        if(onLine(lines[i]->line,vias[j]->pos,true))
                        {
                            Line* line=lines.takeAt(i);
                            items.removeAll(line);
                            i--;
                            Line* l1=new Line;
                            l1->state=line->state;
                            l1->line=QLine(line->line.p1(),vias[j]->pos);
                            l1->color=line->color;
                            lines.append(l1);
                            items.append(l1);
                            Line *l2=new Line;
                            l2->color=line->color;
                            l2->state=line->state;
                            l2->line=QLine(line->line.p2(),vias[j]->pos);
                            lines.append(l2);
                            items.append(l2);
                        }
    }
    resizeNow();
}

QList<QPoint> DigiView::allIntersect(QLine line)
{
    QList<QPoint> ret;
    ret.append(line.p1());
    ret.append(line.p2());
    for(int i=0;i<lines.length();i++)
    {
        if(onLine(lines[i]->line,line.p1())||onLine(lines[i]->line,line.p2()))
        {
            ret.append(lines[i]->line.p1());
            ret.append(lines[i]->line.p2());
        }
    }
    for(int i=0;i<lines.length();i++)
    {
        if(onLine(line,lines[i]->line.p1()))
            ret.append(lines[i]->line.p1());
        if(onLine(line,lines[i]->line.p2()))
            ret.append(lines[i]->line.p2());
    }
    for(int i=0;i<blocks.length();i++)
        for(int j=0;j<blocks[i]->pins.length();j++)
            if(onLine(line,blocks[i]->pins[j].pos()))
                ret.append(blocks[i]->pins[j].pos());
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
    double grid=Settings::final()->gridSize();
    Settings::final()->setGridSize(100);
    QPicture pic=exportPicture();
    QImage ret((pic.boundingRect().size())+QSize(200,200),QImage::Format_ARGB32);
    ret.fill(Qt::transparent);
    QPainter painter(&ret);
    painter.setWorldTransform(QTransform().translate(-pic.boundingRect().topLeft().x(),-pic.boundingRect().topLeft().y()));
    //painter.scale(10,10);
    painter.drawPicture(100,100,pic);
    painter.end();
    Settings::final()->setGridSize(grid);
    return ret;
}

QPicture DigiView::exportPicture()
{
    QPicture picture;
    QPainter painter(&picture);
    for(int i=0;i<items.length();i++)
        painter.drawPicture(items[i]->pos*Settings::final()->gridSize(),blocks[i]->draw(false));
    painter.end();
    return picture;
}

void DigiView::loadJson(QByteArray json)
{
    lastSel=-1;
    QJsonObject root=QJsonDocument::fromJson(json).object();
    QJsonArray l=root["lines"].toArray();
    for(int i=0;i<l.size();i++)
    {
        Line* c=new Line;
        c->line.setP1(QPoint(l[i].toObject()["x1"].toInt(),l[i].toObject()["y1"].toInt()));
        c->line.setP2(QPoint(l[i].toObject()["x2"].toInt(),l[i].toObject()["y2"].toInt()));
        c->color=QColor(l[i].toObject()["color"].toString());
        lines.append(c);
        items.append(c);
    }
    QJsonArray t=root["texts"].toArray();
    for(int i=0;i<t.size();i++)
    {
        Text* c=new Text;
        c->pos=QPoint(t[i].toObject()["x"].toInt(),t[i].toObject()["y"].toInt());
        c->text=t[i].toObject()["text"].toString();
        c->color=QColor(t[i].toObject()["color"].toString());
        texts.append(c);
        items.append(c);
    }
    QJsonArray v=root["vias"].toArray();
    for(int i=0;i<v.size();i++)
    {
        Via* c=new Via;
        c->pos=QPoint(v[i].toObject()["x"].toInt(),v[i].toObject()["y"].toInt());
        vias.append(c);
        items.append(c);
    }
    QJsonArray g=root["blocks"].toArray();
    for(int i=0;i<g.size();i++)
    {
        Block *c=BlockList::newBlock(g[i].toObject()["name"].toString());
        QJsonArray pins=g[i].toObject()["pins"].toArray();
        if(c!=0)
        {
            for(int j=0;j<pins.size();j++)
            {
                QJsonObject pin=pins[j].toObject();
                if(c->pins.length()>j)
                    c->pins[j].type=pin["type"].toBool();
            }
            c->pos=QPoint(g[i].toObject()["x"].toInt(),g[i].toObject()["y"].toInt());
            c->color=QColor(g[i].toObject()["color"].toString());
            for(int i=0;i<c->pins.length();i++)
                c->pins[i].parent=c;
            blocks.append(c);
            items.append(c);
        }
    }
    for(int i=0;i<blocks.length();i++)
    {
        QJsonObject c;
        c.insert("x",blocks[i]->pos.x());
        c.insert("y",blocks[i]->pos.y());
        c.insert("name",blocks[i]->name);
        g.append(c);
    }
    cleanUp();
    update();
}

void DigiView::clearSelection()
{
    selection.clear();
    update();
}

void DigiView::deleteSelection()
{
    lastSel=-1;
    for(int i=0;i<selection.length();i++)
    {
        Item* item=items[selection[i]];
        item->deleteLater();
        texts.removeAll((Text*)item);
        vias.removeAll((Via*)item);
        lines.removeAll((Line*)item);
        blocks.removeAll((Block*)item);
        items.removeAll(item);
    }
    emit changed();
    clearSelection();
    cleanUp();
}

QList<int> DigiView::getNet(QLine in)
{
    QList<int> ret;
    QList<QPoint> points;
    points.append(in.p1());
    points.append(in.p2());
    QList<QPoint> visited;
    while(points.length()>0)
    {
        QPoint p=points.takeFirst();
        bool c=false;
        for(auto via:vias)
            if(via->pos==p)
                if(!c)
                    for(int i=0;i<lines.length();i++)
                        if((lines[i]->line.p1()==p)||(lines[i]->line.p2()==p))
                        {
                            ret.append(i);
                            if(!visited.contains(lines[i]->line.p1()))
                            {
                                visited.append(lines[i]->line.p1());
                                points.append(lines[i]->line.p1());
                            }
                            if(!visited.contains(lines[i]->line.p2()))
                            {
                                visited.append(lines[i]->line.p2());
                                points.append(lines[i]->line.p2());
                            }
                        }
    }
    return ret;
}

QList<QPair<QColor,QString> > DigiView::loadColorProfile()
{
    QList<QPair<QColor,QString> > ret;
    QFile file("colors.txt");
    if(!file.exists())
        file.setFileName(":/colors.txt");
    file.open(QFile::ReadOnly);
    while(!file.atEnd())
    {
        QString line=file.readLine();
        line.replace("\r","");
        line.replace("\n","");
        QString name=line.left(line.indexOf(","));
        QString code=line.mid(line.indexOf(",")+1);
        if(QColor::isValidColor(code))
        {
            QPair<QColor,QString> round;
            QColor color(code);
            round.first=color;
            round.second=name;
            ret.append(round);
        }
        else
            qDebug()<<"Error Not Found: "<<code;
    }
    return ret;
}

void DigiView::createTable()
{
    QPoint pos(-1,-1);
    Block* lin;
    for(int i=0;i<blocks.length();i++)
    {
        if(blocks[i]->name=="Large-IN")
        {
            pos=blocks[i]->pos;
            lin=blocks[i];
            i=blocks.length();
        }
    }
    QList<QPair<QList<bool>,QList<bool> > > table;
    qDebug()<<pos;
    if(pos.x()==-1)
    {
        QMessageBox::warning(NULL,"QDigi","No Block Found");
    }
    else
    {
        int cnt=0;
        QList<QLine> pins;
        QMap<int,int> fakeGates;
        QList<int> outputs;
        for(int i=0;i<blocks.length();i++)
            if(blocks[i]->name=="OUT")
                outputs.append(i);
        while(cnt<16)
        {
            QPoint pin(pos+QPoint(2,cnt+1));
            if(getNet(QLine(QPoint(-1,-1),pin)).length()==0)
                break;
            pins.append(QLine(QPoint(-1,-1),pin));
            cnt++;
        }
        lin->useFake=true;
        lin->fake.clear();
        for(int i=0;i<pins.length();i++)
        {
            lin->fake.append(false);
        }
        while(lin->fake.length()<16)
            lin->fake.append(false);
        for(int i=0;i<pow(pins.length(),2);i++)
        {
            QPair<QList<bool>,QList<bool> > row;
            QList<bool> values;
            for(int j=0;j<pins.length();j++)
            {
                values.prepend((i>>j)%2==1);
            }
            lin->fake.clear();
            row.first=values;
            for(int j=0;j<pins.length();j++)
                lin->fake.prepend(values[j]);
            while(lin->fake.length()<16)
                lin->fake.append(false);
            for(int j=0;j<25;j++)
                timeout();
            QList<bool> res;
            for(int j=0;j<outputs.length();j++)
                res.append(blocks[outputs[j]]->pins[0].state);
            row.second=res;
            table.append(row);
        }
        lin->useFake=false;
        QFile out("logic.html");
        out.open(QFile::WriteOnly|QFile::Truncate);
        out.write("<html><body><table border=1><tr>");
        for(int i=0;i<cnt;i++)
            out.write("<th>IN"+QString::number((cnt-1)-i).toUtf8()+"</th>");
        for(int i=0;i<outputs.length();i++)
            out.write("<th>OUT"+QString::number(i).toUtf8()+"</th>");
        out.write("</tr>");
        for(int i=0;i<table.length();i++)
        {
            out.write("<tr>");
            for(int j=0;j<cnt;j++)
            {
                QString c="0";
                if(table[i].first[j])
                    c="1";
                out.write("<td>"+c.toUtf8()+"</td>");
            }
            for(int j=0;j<outputs.length();j++)
            {
                QString c="0";
                if(table[i].second[j])
                    c="1";
                out.write("<td>"+c.toUtf8()+"</td>");
            }
            out.write("</tr>\n");
        }
        out.write("</table></body></html>");
        out.flush();
        out.close();
        QDesktopServices::openUrl(QUrl("file:///"+QDir().absoluteFilePath(out.fileName())));
    }
}

/*QList<int> DigiView::linesAtPoint(QPoint point)
{
    QList<int> ret;
    for(int i=0;i<lines.length();i++)
    {
        if(lines[i]->line.p1()==point)
            ret.append(i);
        if(lines[i]->line.p2()==point)
            ret.append(i);
    }
    return ret;
}*/

void DigiView::keyPressEvent(QKeyEvent *event)
{
    setFocus();
    qDebug()<<"KEY EVENT"<<event->text();
    if(event->modifiers()==0)
    {
        switch(event->key())
        {
        case Qt::Key_A:
            largeIn(0);
            break;
        case Qt::Key_Escape:
            dragGate="";
            break;
        case Qt::Key_B:
            largeIn(1);
            break;
        case Qt::Key_C:
            largeIn(2);
            break;
        case Qt::Key_D:
            largeIn(3);
            break;
        case Qt::Key_E:
            largeIn(4);
            break;
        case Qt::Key_F:
            largeIn(5);
            break;
        case Qt::Key_G:
            largeIn(6);
            break;
        case Qt::Key_H:
            largeIn(7);
            break;
        case Qt::Key_I:
            largeIn(8);
            break;
        case Qt::Key_J:
            largeIn(9);
            break;
        case Qt::Key_K:
            largeIn(10);
            break;
        case Qt::Key_L:
            largeIn(11);
            break;
        case Qt::Key_M:
            largeIn(12);
            break;
        case Qt::Key_N:
            largeIn(13);
            break;
        case Qt::Key_O:
            largeIn(14);
            break;
        case Qt::Key_P:
            largeIn(15);
            break;
        default:
            emitKey(event->text());
            return;
        }
        event->accept();
    }
    update();
}

void DigiView::largeIn(int o)
{
    if(lastSel>=0)
        if(blocks[lastSel]->name=="Large-IN")
        {
            blocks[lastSel]->keyPress(o);
        }
}

void DigiView::emitKey(QString key)
{
    qDebug()<<"Emitting Key";
    if(lastSel>=0)
    {
        qDebug()<<"Last Sel is "<<blocks[lastSel]->name;
        blocks[lastSel]->keyPressNorm(key);
    }
}

QJsonObject DigiView::exportJSON()
{
    QJsonObject root;
    QJsonArray l;
    for(int i=0;i<lines.length();i++)
    {
        QJsonObject c;
        c.insert("x1",lines[i]->line.x1());
        c.insert("y1",lines[i]->line.y1());
        c.insert("x2",lines[i]->line.x2());
        c.insert("y2",lines[i]->line.y2());
        c.insert("color",lines[i]->color.name());
        l.append(c);
    }
    root.insert("lines",l);
    QJsonArray t;
    for(int i=0;i<texts.length();i++)
    {
        QJsonObject c;
        c.insert("x",texts[i]->pos.x());
        c.insert("y",texts[i]->pos.y());
        c.insert("text",texts[i]->text);
        c.insert("color",texts[i]->color.name());
        t.append(c);
    }
    root.insert("texts",t);
    QJsonArray g;
    for(int i=0;i<blocks.length();i++)
    {
        QJsonObject c;
        c.insert("x",blocks[i]->pos.x());
        c.insert("y",blocks[i]->pos.y());
        c.insert("name",blocks[i]->name);
        c.insert("color",blocks[i]->color.name());
        QJsonArray pins;
        for(int j=0;j<blocks[i]->pins.length();j++)
        {
            QJsonObject obj;
            obj.insert("type",blocks[i]->pins[j].type);
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
        c.insert("x",vias[i]->pos.x());
        c.insert("y",vias[i]->pos.y());
        v.append(c);
    }
    root.insert("vias",v);
    return root;
}

void DigiView::resizeEvent(QResizeEvent *event)
{
}

void DigiView::resizeNow()
{
    int bwid=0;
    int bhei=0;
    for(int i=0;i<items.length();i++)
    {
        QPointF rect=items[i]->clickRect().bottomRight();
        bwid=fmax(bwid,rect.x());
        bhei=fmax(bhei,rect.y());
    }
    bwid*=Settings::final()->gridSize();
    bhei*=Settings::final()->gridSize();
    bwid+=window()->width();
    bhei+=window()->height();
    QSize size(bwid,bhei);
    if(minimumSize()!=size)
        setMinimumSize(size);
    update();
}

void DigiView::addBlock(QPoint pos, QString type)
{
    clearSelection();
    Block* blk=BlockList::newBlock(type);
    for(int i=0;i<blk->pins.length();i++)
        blk->pins[i].parent=blk;
    blk->pos=pos;
    /*if(dragGate=="Large-IN")
    {
        for(int i=0;i<blocks.length();i++)
            if(blocks[i]->name=="Large-IN")
            {
                dragGate="";
                return;
            }
    }*/
    for(int i=0;i<items.length();i++)
        if(QRectF(items[i]->clickRect()).intersects(blk->clickRect()))
            if(isBlock(items[i]))
                return;
    blocks.append(blk);
    items.append(blk);
    emit changed();
    cleanUp();
}

QList<QPair<int,int> > DigiView::getItemsForOutput(QPoint pos, QList<int> *witems)
{
    QList<QPair<int,int> > ret;
    QList<QPoint> points;
    points.append(pos);
    QList<QPoint> done;
    while(points.length()>0)
    {
        QPoint point=points.takeFirst();
        done.append(point);
        for(int i=0;i<lines.length();i++)
        {
            if(lines[i]->line.p1()==point)
            {
                points.append(lines[i]->line.p2());
                witems->append(items.indexOf(lines[i]));
            }
            if(lines[i]->line.p2()==point)
            {
                points.append(lines[i]->line.p1());
                witems->append(items.indexOf(lines[i]));
            }
        }
        for(int i=0;i<vias.length();i++)
        {
            if(vias[i]->pos==point)
                witems->append(items.indexOf(vias[i]));
        }
        for(int i=0;i<blocks.length();i++)
            for(int j=0;j<blocks[i]->pins.length();j++)
            {
                if(blocks[i]->pins[j].pos()==point)
                    ret.append(QPair<int,int>(i,j));
            }

        for(auto p:done)
            points.removeAll(p);
    }
    return ret;
}


bool DigiView::isLine(Item *item)
{
    if(lines.contains((Line*)item))
        return true;
    return false;
}

bool DigiView::isText(Item *item)
{
    if(texts.contains((Text*)item))
        return true;
    return false;
}

bool DigiView::isVia(Item *item)
{
    if(vias.contains((Via*)item))
        return true;
    return false;
}

bool DigiView::isBlock(Item *item)
{
    if(blocks.contains((Block*)item))
        return true;
    return false;
}

QPoint DigiView::toScreen(QPointF pos)
{
    int x=round(pos.x()*Settings::final()->gridSize());
    int y=round(pos.y()*Settings::final()->gridSize());
    return QPoint(x,y);
}

void DigiView::check()
{
    for(auto item:items)
    {
        if(!(isText(item)||isBlock(item)||isVia(item)||isLine(item)))
        {
            qDebug()<<"INVALID ITEM"<<item;
            exit(1);
        }
    }
    for(auto via:vias)
    {
        if(!items.contains(via))
        {
            qDebug()<<"INVALID VIA"<<via->pos;
            exit(1);
        }
    }
    for(auto text:texts)
    {
        if(!items.contains(text))
        {
            qDebug()<<"INVALID TEXT"<<text->text;
            exit(1);
        }
    }
    for(auto block:blocks)
    {
        if(!items.contains(block))
        {
            qDebug()<<"INVALID BLOCK"<<block->name;
            exit(1);
        }
    }
    for(auto line:lines)
    {
        if(!items.contains(line))
        {
            qDebug()<<"INVALID LINE"<<line->line;
            exit(1);
        }
    }
}
