#include <QInputDialog>
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

DigiView::DigiView(QWidget *parent) : QWidget(parent)
{
    startPoint=QPoint(-1,-1);
    curPoint=QPoint(-1,-1);
    BlockList list;
    setAcceptDrops(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer.setInterval(100);
    setContextMenuPolicy(Qt::DefaultContextMenu);
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
            painter.drawPixmap(dragPos.x()*Settings::final()->gridSize(),dragPos.y()*Settings::final()->gridSize(),block->drawBlock(Qt::black));
        }
    }
    painter.setPen(Qt::black);
    for(int i=0;i<texts.length();i++)
    {
        QFont font("Arial");
        font.setPixelSize(Settings::final()->gridSize());
        painter.setFont(font);
        text_t cur=texts[i];
        QRect rect((cur.pos.x()*Settings::final()->gridSize()),(cur.pos.y()*Settings::final()->gridSize())-Settings::final()->gridSize(),500,Settings::final()->gridSize());
        painter.drawText(rect,Qt::AlignBottom|Qt::AlignLeft,cur.text);
        QPen pen(Qt::green);
        QBrush brush(Qt::green);
        painter.setPen(pen);
        brush.setStyle(Qt::Dense6Pattern);
        brush.setColor(QColor::fromRgbF(0.0,1.0,0.0,0.5));
        painter.setBrush(brush);
        if(selectedTexts.contains(i))
            painter.drawRect(rect);
    }
    QPen pen(Qt::green);
    QBrush brush(Qt::green);
    painter.setPen(pen);
    brush.setStyle(Qt::Dense6Pattern);
    brush.setColor(QColor::fromRgbF(0.0,1.0,0.0,0.5));
    painter.setBrush(brush);
    for(int i=0;i<selectedBlocks.length();i++)
    {
        QRect rect(((QPointF(blocks[selectedBlocks[i]].pos)+QPointF(0,0.5))*Settings::final()->gridSize()).toPoint(),QSize(1,1));
        rect.setWidth((blocks[selectedBlocks[i]].block->width+1)*Settings::final()->gridSize());
        rect.setHeight((blocks[selectedBlocks[i]].block->height)*Settings::final()->gridSize());
        painter.drawRect(rect);
    }
    for(int i=0;i<selectedLines.length();i++)
    {
        QRect rect(lines[selectedLines[i]].line.p1()*Settings::final()->gridSize(),lines[selectedLines[i]].line.p2()*Settings::final()->gridSize());
        rect.setWidth(fmax(1,rect.width()));
        rect.setHeight(fmax(1,rect.height()));
        rect.setX(rect.x()-4);
        rect.setY(rect.y()-4);
        rect.setWidth(rect.width()+8);
        rect.setHeight(rect.height()+8);
        painter.drawRect(rect);
    }
    if(curPoint!=QPoint(-1,-1))
        if(!(((curPoint.x()!=startPoint.x())&&(curPoint.y()==startPoint.y()))||((curPoint.y()!=startPoint.y())&&(curPoint.x()==startPoint.x()))))
        {
            QRect rect(startPoint*Settings::final()->gridSize(),curPoint*Settings::final()->gridSize());
            painter.drawRect(rect);
        }
    for(int i=0;i<blocks.length();i++)
    {
        painter.drawPixmap(blocks[i].pos*Settings::final()->gridSize(),blocks[i].block->drawBlock(blocks[i].color));
        for(int j=0;j<blocks[i].block->pins.length();j++)
            if(blocks[i].block->pins[j].state==2)
            {
                QPen pen(QColor::fromRgbF(1,0.5,0));
                pen.setWidth(0);
                painter.setPen(pen);
                for(int r=5;r<(Settings::final()->gridSize());r+=5)
                    painter.drawEllipse(((blocks[i].pos+blocks[i].block->pins[j].point) * Settings::final()->gridSize()),r,r);
            }
    }
    QPen line(Qt::black);
    line.setWidth(3);
    painter.setPen(line);
    if(curPoint!=QPoint(-1,-1))
        if(((curPoint.x()!=startPoint.x())&&(curPoint.y()==startPoint.y()))||((curPoint.y()!=startPoint.y())&&(curPoint.x()==startPoint.x())))
            painter.drawLine(startPoint*Settings::final()->gridSize(),curPoint*Settings::final()->gridSize());
    line.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
    for(int i=0;i<lines.length();i++)
    {
        line.setColor(lines[i].color);
        line.setStyle(Qt::SolidLine);
        painter.setPen(line);
        painter.drawLine(lines[i].line.p1()*Settings::final()->gridSize(),lines[i].line.p2()*Settings::final()->gridSize());
    }
    for(int i=0;i<lines.length();i++)
    {
        if(lines[i].state)
        {
            line.setColor(Qt::red);
            line.setStyle(Qt::DotLine);
            painter.setPen(line);
            painter.drawLine(lines[i].line.p1()*Settings::final()->gridSize(),lines[i].line.p2()*Settings::final()->gridSize());
        }
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    int d=(2.0*Settings::final()->penWidth()*Settings::final()->gridSize());
    for(int i=0;i<vias.length();i++)
        painter.drawEllipse(vias[i]*Settings::final()->gridSize(),d,d);
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
    if(event->button()==Qt::LeftButton)
    {
        clearSelection();
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
    }
    update();
}

void DigiView::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if(event->buttons()&Qt::LeftButton)
    {
        clearSelection();
        curPoint=toGrid(event->pos());
    }
    update();
}

void DigiView::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if(event->button()==Qt::LeftButton)
    {
        QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
        clearSelection();
        curPoint=toGrid(event->pos());
        if((startPoint.x()==curPoint.x())||(startPoint.y()==curPoint.y()))
            if(startPoint!=curPoint)
            {
                line_t line;
                line.line=QLine(startPoint,curPoint);
                for(int i=0;i<lines.length();i++)
                    if(onLine(lines[i].line,startPoint)||onLine(lines[i].line,curPoint))
                        line.color=lines[i].color;
                lines.append(line);
                cleanUp();
                emit changed();
            }
        if(curPoint!=startPoint)
            if(!(((curPoint.x()!=startPoint.x())&&(curPoint.y()==startPoint.y()))||((curPoint.y()!=startPoint.y())&&(curPoint.x()==startPoint.x()))))
            {
                QRect sel(curPoint,startPoint);
                for(int i=0;i<texts.length();i++)
                {
                    text_t cur=texts[i];
                    QRectF rect(cur.pos.x(),cur.pos.y()-1.0,500/Settings::final()->gridSize(),1.0);
                    if(rect.intersects(QRectF(sel)))
                        selectedTexts.append(i);
                }
                for(int i=0;i<blocks.length();i++)
                {
                    QRect blk(blocks[i].pos,QSize(blocks[i].block->width+1,blocks[i].block->height+1));
                    if(sel.intersects(blk))
                        selectedBlocks.append(i);
                }

                for(int i=0;i<lines.length();i++)
                {
                    QRect line(lines[i].line.p1(),lines[i].line.p2());
                    line.setWidth(fmax(1,line.width()));
                    line.setHeight(fmax(1,line.height()));
                    if(sel.intersects(line))
                        selectedLines.append(i);
                }
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
    }
    update();
}

bool DigiView::save(QString where)
{
    qDebug()<<"WHERE1"<<where;
    if(where.isEmpty())
        where=fileName;
    qDebug()<<"WHERE2"<<where;
    if(where.isEmpty())
        return false;
    fileName=where;
    if(!where.endsWith(".qdigi"))
        where+=".qdigi";
    QJsonObject root;
    QJsonArray l;
    for(int i=0;i<lines.length();i++)
    {
        QJsonObject c;
        c.insert("x1",lines[i].line.x1());
        c.insert("y1",lines[i].line.y1());
        c.insert("x2",lines[i].line.x2());
        c.insert("y2",lines[i].line.y2());
        c.insert("color",lines[i].color.name());
        l.append(c);
    }
    root.insert("lines",l);
    QJsonArray t;
    for(int i=0;i<texts.length();i++)
    {
        QJsonObject c;
        c.insert("x",texts[i].pos.x());
        c.insert("y",texts[i].pos.y());
        c.insert("text",texts[i].text);
        t.append(c);
    }
    root.insert("texts",t);
    QJsonArray g;
    for(int i=0;i<blocks.length();i++)
    {
        QJsonObject c;
        c.insert("x",blocks[i].pos.x());
        c.insert("y",blocks[i].pos.y());
        c.insert("name",blocks[i].block->name);
        c.insert("color",blocks[i].color.name());
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
    int err=0;
    QFile file(where);
    file.open(QFile::WriteOnly|QFile::Truncate);
    QByteArray data=QJsonDocument(root).toJson(QJsonDocument::Compact);
    file.write(data);
    file.close();
    /*qDebug()<<"PA"<<file.exists();
    /*qDebug()<<"A"<<file.exists();
    file.remove();
    qDebug()<<"POA"<<file.isOpen();
    if(file.isOpen())
        file.close();
    file.remove();
    qDebug()<<"A"<<file.exists();
    qDebug()<<where;
    zip_t* arch=zip_open(where.toStdString().data(),ZIP_CREATE|ZIP_TRUNCATE,&err);
    qDebug()<<"Open"<<err;
    qDebug()<<"Saving"<<data.left(100)<<"...";
    zip_source_t * source=zip_source_buffer(arch,data.data(),data.length(),0);
    qDebug()<<"src_bfr1"<<source;
    qDebug()<<"ADD1"<<zip_file_add(arch,"data.json",source,ZIP_FL_OVERWRITE);
    QByteArray version="0.1";
    source=zip_source_buffer(arch,version.data(),version.length(),0);
    qDebug()<<"src_bfr2"<<source;
    qDebug()<<"ADD2"<<zip_file_add(arch,"version.txt",source,ZIP_FL_OVERWRITE);
    if(zip_strerror(arch)!=NULL)
        qDebug()<<"STRERROR"<<zip_strerror(arch);
    if(zip_close(arch)!=0)
    {
        qDebug()<<"STRERRORCLOSE"<<zip_strerror(arch);
    }*/
    Settings::final()->setLastFile(where);
    return true;
}

void DigiView::load(QString where)
{
    lines.clear();
    for(int i=0;i<blocks.length();i++)
        blocks[i].block->deleteLater();
    blocks.clear();
    vias.clear();
    QFile file(where);
    file.open(QFile::ReadOnly);
    if(!file.isOpen())
        return;
    QByteArray header=file.read(4);
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
    Settings::final()->setLastFile(fileName);
    if(!isZip)
    {
        loadJson(file.readAll());
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
                                points.append(lines[k].line.p1());
                                points.append(lines[k].line.p2());
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
        bool state=states[keys[i]];
        blocks[k].block->pins[l].state=state;
    }
    if(!ok)
    {
        error=true;
        /*for(int i=0;i<blocks.length();i++)
            for(int j=0;j<blocks[i].block->pins.length();j++)
                if(blocks[i].block->pins[j].direction==0)
                    blocks[i].block->pins[j].state=false;
        for(int i=0;i<lines.length();i++)
            lines[i].state=false;*/
        for(int i=0;i<offen.length();i++)
            blocks[offen[i].first].block->pins[offen[i].second].state=2;
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
        if((blocks[i].pos.x()<=(pf.x()-0.5))&&((blocks[i].pos.x()+blocks[i].block->width)>=(pf.x()-0.5)))
            if((blocks[i].pos.y()<=(pf.y()-0.5))&&((blocks[i].pos.y()+blocks[i].block->height)>=(pf.y()-0.5)))
                block=i;
    int pin=-1;
    QStringList alt;
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
        alt=blocks[block].block->alt;
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
    int text=-1;
    for(int i=0;i<texts.length();i++)
    {
        text_t cur=texts[i];
        QRectF rect(cur.pos.x(),cur.pos.y()-1.0,500/Settings::final()->gridSize(),1.0);
        if(rect.contains(pf))
        {
            text=i;
        }
    }
    QAction* delBlockAct=NULL;
    QAction* addViaAct=NULL;
    QAction* delLineAct=NULL;
    QAction* delSelAct=NULL;
    QAction* delLineNetAct=NULL;
    QAction* delViaAct=NULL;
    QAction* delTextAct=NULL;
    QAction* changePinAct=NULL;
    QAction* addTextAct=NULL;
    QMap<QAction*,QColor> setLineColorAction;
    QMap<QAction*,QColor> setBlockColorAction;
    QList<QAction*> altAction;
    if(lcnt==2)
    {
        addViaAct=menu.addAction("Knotenpunkt hinzufügen");
        ok=true;
    }
    if(selectedBlocks.length()>0)
    {
        delSelAct=menu.addAction("Auswahl löschen");
        ok=true;
    }
    addTextAct=menu.addAction("Text hinzufügen");
    ok=true;
    if(block>=0)
    {
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
    if(block>=0)
    {
        QList<QPair<QColor,QString> > colors=loadColorProfile();
        QMenu* cm=menu.addMenu("Blockfarbe ändern");
        for(int i=0;i<colors.length();i++)
        {
            QPixmap map(24,24);
            map.fill(colors[i].first);
            QAction* act=cm->addAction(QIcon(map),colors[i].second);
            setBlockColorAction.insert(act,colors[i].first);
        }
        ok=true;
    }
    if(via>=0)
    {
        delViaAct=menu.addAction("Knotenpunkt Löschen");
        ok=true;
    }
    if(text>=0)
    {
        delTextAct=menu.addAction("Text Löschen");
        ok=true;
    }
    if(line>=0)
    {
        delLineAct=menu.addAction("Linie Löschen");
        ok=true;
    }
    if(line>=0)
    {
        QList<QPair<QColor,QString> > colors=loadColorProfile();
        QMenu* cm=menu.addMenu("Linienfarbe ändern");
        for(int i=0;i<colors.length();i++)
        {
            QPixmap map(24,24);
            map.fill(colors[i].first);
            QAction* act=cm->addAction(QIcon(map),colors[i].second);
            setLineColorAction.insert(act,colors[i].first);
        }
        ok=true;
    }
    if(line>=0)
    {
        delLineNetAct=menu.addAction("Linien bis Knotenpunk löschen");
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
        {
            if(act==delBlockAct)
            {
                blocks[block].block->deleteLater();
                blocks.removeAt(block);
                emit changed();
                clearSelection();
            }
            for(int i=0;i<alt.length();i++)
                if(altAction[i]==act)
                {
                    blocks[block].block->deleteLater();
                    blocks[block].block=BlockList::newBlock(alt[i]);
                    emit changed();
                    clearSelection();
                }
        }
        if(selectedBlocks.length()>0)
        {
            if(act==delSelAct)
            {
                deleteSelection();
            }
        }
        if(act==addTextAct)
        {
            bool ok;
            QString message=QInputDialog::getText(NULL,"QDigi","Text Einfügen",QLineEdit::Normal,QString(),&ok);
            if(ok)
            {
                text_t text;
                text.pos=p;
                text.text=message;
                texts.append(text);
            }
            deleteSelection();
        }
        if(via>=0)
            if(act==delViaAct)
            {
                vias.removeAt(via);
                emit changed();
                clearSelection();
            }
        if(pin>=0)
            if(act==changePinAct)
            {
                blocks[block].block->pins[pin].type=!blocks[block].block->pins[pin].type;
                emit changed();
                clearSelection();
            }
        if(text>=0)
            if(act==delTextAct)
            {
                texts.removeAt(text);
                emit changed();
                clearSelection();
            }
        if(lcnt==2)
            if(act==addViaAct)
            {
                vias.append(p);
                emit changed();
                clearSelection();
            }
        if(line>=0)
            if(act==delLineAct)
            {
                lines.removeAt(line);
                emit changed();
                clearSelection();
            }        if(line>=0)
            if(line>=0)
                if(setLineColorAction.contains(act))
                {
                    QList<int> net=getNet(lines[line].line);
                    QColor c=setLineColorAction[act];
                    for(int i=0;i<net.length();i++)
                        lines[net[i]].color=c;
                    emit changed();
                    clearSelection();
                }
        if(block>=0)
            if(setBlockColorAction.contains(act))
            {
                QColor c=setBlockColorAction[act];
                blocks[block].color=c;
                emit changed();
                clearSelection();
            }
        if(line>=0)
            if(act==delLineNetAct)
            {
                clearSelection();
                QList<QPoint> points;
                points.append(lines[line].line.p1());
                points.append(lines[line].line.p2());
                lines.removeAt(line);
                while(points.length()>0)
                {
                    int cnt=0;
                    int del=-1;
                    QPoint point=points.takeFirst();
                    for(int i=0;i<lines.length();i++)
                    {
                        if(onLine(lines[i].line,point,true))
                            cnt=100;
                        if(lines[i].line.p1()==point)
                        {
                            cnt++;
                            del=i;
                        }
                        else if(lines[i].line.p2()==point)
                        {
                            del=i;
                            cnt++;
                        }
                    }
                    for(int i=0;i<vias.length();i++)
                        if(vias[i]==point)
                            cnt=100;
                    if(cnt==1)
                    {
                        points.append(lines[del].line.p1());
                        points.append(lines[del].line.p2());
                        lines.removeAt(del);
                    }
                }
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
                    if((i<lines.length())&&(j<lines.length()))
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
                    if((i<lines.length())&&(j<lines.length()))
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
                                line.color=lines[i].color;
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
                    if(!vias.contains(point))
                        vias.append(point);
                }
        }
        for(int i=0;i<lines.length();i++)
            for(int j=0;j<vias.length();j++)
                if((i>=0)&&(j>=0))
                    if((i<lines.length())&&(j<vias.length()))
                        if(onLine(lines[i].line,vias[j],true))
                        {
                            line_t line=lines.takeAt(i);
                            i--;
                            line_t l1;
                            l1.state=line.state;
                            l1.line=QLine(line.line.p1(),vias[j]);
                            l1.color=line.color;
                            lines.append(l1);
                            line_t l2;
                            l2.color=line.color;
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
        painter.drawPixmap(blocks[i].pos*Settings::final()->gridSize(),blocks[i].block->drawBlock(blocks[i].color));
    QPen line(Qt::black);
    line.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
    for(int i=0;i<lines.length();i++)
    {
        if(lines[i].state)
            line.setColor(Qt::red);
        else
            line.setColor(Qt::black);
        painter.setPen(line);
        painter.drawLine(lines[i].line.p1()*Settings::final()->gridSize(),lines[i].line.p2()*Settings::final()->gridSize());
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    for(int i=0;i<vias.length();i++)
        painter.drawEllipse(vias[i]*Settings::final()->gridSize(),int(0.25*Settings::final()->gridSize()),int(0.25*Settings::final()->gridSize()));
    return picture;
}

void DigiView::loadJson(QByteArray json)
{
    QJsonObject root=QJsonDocument::fromJson(json).object();
    QJsonArray l=root["lines"].toArray();
    for(int i=0;i<l.size();i++)
    {
        line_t c;
        c.line.setP1(QPoint(l[i].toObject()["x1"].toInt(),l[i].toObject()["y1"].toInt()));
        c.line.setP2(QPoint(l[i].toObject()["x2"].toInt(),l[i].toObject()["y2"].toInt()));
        c.color=QColor(l[i].toObject()["color"].toString());
        lines.append(c);
    }
    QJsonArray t=root["texts"].toArray();
    for(int i=0;i<t.size();i++)
    {
        text_t c;
        c.pos=QPoint(t[i].toObject()["x"].toInt(),t[i].toObject()["y"].toInt());
        c.text=t[i].toObject()["text"].toString();
        texts.append(c);
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
        c.color=QColor(g[i].toObject()["color"].toString());
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
    cleanUp();
    update();
}

void DigiView::clearSelection()
{
    selectedBlocks.clear();
    selectedLines.clear();
    selectedTexts.clear();
    update();
}

void DigiView::deleteSelection()
{
    for(int i=0;i<selectedBlocks.length();i++)
        blocks[selectedBlocks[i]].block->deleteLater();
    for(int i=0;i<selectedBlocks.length();i++)
    {
        int at=selectedBlocks[i];
        blocks.removeAt(at);
        for(int j=0;j<selectedBlocks.length();j++)
            if(selectedBlocks[j]>at)
                selectedBlocks[j]--;
    }
    for(int i=0;i<selectedLines.length();i++)
    {
        int at=selectedLines[i];
        lines.removeAt(at);
        for(int j=0;j<selectedLines.length();j++)
            if(selectedLines[j]>at)
                selectedLines[j]--;
    }

    for(int i=0;i<selectedTexts.length();i++)
    {
        int at=selectedTexts[i];
        texts.removeAt(at);
        for(int j=0;j<selectedTexts.length();j++)
            if(selectedTexts[j]>at)
                selectedTexts[j]--;
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
        for(int i=0;i<lines.length();i++)
            if((lines[i].line.p1()==p)||(lines[i].line.p2()==p))
            {
                ret.append(i);
                if(!visited.contains(lines[i].line.p1()))
                {
                    visited.append(lines[i].line.p1());
                    points.append(lines[i].line.p1());
                }
                if(!visited.contains(lines[i].line.p2()))
                {
                    visited.append(lines[i].line.p2());
                    points.append(lines[i].line.p2());
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
