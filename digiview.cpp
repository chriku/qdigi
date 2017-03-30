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

int lastSel=-1;

DigiView::DigiView(QWidget *parent) : QWidget(parent)
{
    recording=false;
    setFocus();
    lastContext=QTime::currentTime();
    startPoint=QPoint(-1,-1);
    curPoint=QPoint(-1,-1);
    BlockList list;
    setAcceptDrops(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer.setInterval(100);
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
            painter.drawPicture(dragPos.x()*Settings::final()->gridSize(),dragPos.y()*Settings::final()->gridSize(),block->drawBlock(Qt::black));
        }
    }
    painter.setPen(Qt::black);
    for(int i=0;i<texts.length();i++)
    {
        QFont font("Arial");
        font.setPixelSize(Settings::final()->gridSize());
        painter.setFont(font);
        text_t cur=texts[i];
        QRectF br;
        QRect rect((cur.pos.x()*Settings::final()->gridSize()),(cur.pos.y()*Settings::final()->gridSize())-Settings::final()->gridSize(),texts[i].len*Settings::final()->gridSize(),Settings::final()->gridSize());
        painter.setPen(cur.color);
        painter.drawText(rect,Qt::AlignBottom|Qt::AlignLeft,cur.text,&br);
        texts[i].len=br.width()/Settings::final()->gridSize();
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
        QRectF rect=blocks[selectedBlocks[i]]->rect();
        rect.setTop(rect.top()*(Settings::final()->gridSize()));
        rect.setLeft(rect.left()*(Settings::final()->gridSize()));
        rect.setBottom(rect.bottom()*Settings::final()->gridSize());
        rect.setRight(rect.right()*Settings::final()->gridSize());
        painter.drawRect(rect);
    }
    for(int i=0;i<selectedLines.length();i++)
    {
        QRect rect(lines[selectedLines[i]].line.p1()*Settings::final()->gridSize(),lines[selectedLines[i]].line.p2()*Settings::final()->gridSize());
        rect=rect.normalized();
        rect.setWidth(fmax(1,rect.width()));
        rect.setHeight(fmax(1,rect.height()));
        rect.setX(rect.x()-4);
        rect.setY(rect.y()-4);
        rect.setWidth(rect.width()+8);
        rect.setHeight(rect.height()+8);
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
        QRectF rect=blocks[lastSel]->rect();
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
        painter.drawPicture(blocks[i]->m_pos*Settings::final()->gridSize(),blocks[i]->block->drawBlock(blocks[i]->color));
        for(int j=0;j<blocks[i]->block->pins.length();j++)
            if(blocks[i]->block->pins[j].state==2)
            {
                QPen pen(QColor::fromRgbF(1,0.5,0));
                pen.setWidth(0);
                painter.setPen(pen);
                for(int r=5;r<(Settings::final()->gridSize());r+=5)
                    painter.drawEllipse((blocks[i]->block->pins[j].pos() * Settings::final()->gridSize()),r,r);
            }
    }
    QPen line(Qt::black);
    line.setWidth(3);
    painter.setPen(line);
    if(!drag)
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
    int d=(2.0*Settings::final()->penWidth()*Settings::final()->gridSize());
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
            for(int k=0;k<blocks[j]->block->pins.length();k++)
                if(blocks[j]->block->pins[k].pos()==vias[i])
                {
                    if(blocks[j]->block->pins[k].direction==0)
                        painter.drawPie((vias[i].x()*Settings::final()->gridSize())-d,(vias[i].y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,-16*45,16*90);
                    else
                        painter.drawPie((vias[i].x()*Settings::final()->gridSize())-d,(vias[i].y()*Settings::final()->gridSize())-d,d*2.0,d*2.0,16*135,16*90);
                }
    }
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
    block_t* blk=new block_t;
    blk->block=BlockList::newBlock(dragGate);
    for(int i=0;i<blk->block->pins.length();i++)
        blk->block->pins[i].parent=blk;
    blk->m_pos=dragPos.toPoint();
    if(dragGate=="Large-IN")
    {
        for(int i=0;i<blocks.length();i++)
            if(blocks[i]->block->name=="Large-IN")
            {
                dragGate="";
                return;
            }
    }
    if(!dragMany)
        dragGate="";
    for(int i=0;i<blocks.length();i++)
        if(QRectF(blocks[i]->rect()).intersects(blk->rect()))
            return;
    blocks.append(blk);
    emit changed();
    cleanUp();
    update();
}

void DigiView::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    if(dragGate!="")
        if(dragMany)
        {
            event->accept();
            clearSelection();
            dragPos=toGrid(event->pos())-QPoint(1,1);
            block_t* blk=new block_t;
            blk->block=BlockList::newBlock(dragGate);
            for(int i=0;i<blk->block->pins.length();i++)
                blk->block->pins[i].parent=blk;
            blk->m_pos=dragPos.toPoint();
            if(dragGate=="Large-IN")
            {
                for(int i=0;i<blocks.length();i++)
                    if(blocks[i]->block->name=="Large-IN")
                    {
                        dragGate="";
                        return;
                    }
            }
            if(event->button()!=Qt::LeftButton)
                dragGate="";
            for(int i=0;i<blocks.length();i++)
                if(QRectF(blocks[i]->rect()).intersects(blk->rect()))
                    return;
            blocks.append(blk);
            emit changed();
            cleanUp();
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
            if(blocks[i]->rect().contains(curPoint))
            {
                idx=i;
            }
        int pin=-1;
        if(idx>=0)
            if(blocks[idx]->block->checkable)
                lastSel=idx;
        if(idx>=0)
            for(int i=0;i<blocks[idx]->block->pins.length();i++)
                if(startPoint==blocks[idx]->block->pins[i].pos())
                    pin=i;
        drag=false;
        blkIdx=idx;
        texIdx=-1;
        if((pin==-1)&&(idx>=0))
        {
            clear=false;
            startBlock=blocks[idx]->m_pos;
            drag=true;
        }
        if(idx>=0)
        {
            double x=event->pos().x()/Settings::final()->gridSize();
            double y=event->pos().y()/Settings::final()->gridSize();
            QPointF p(x,y);
            blocks[idx]->block->onpress(blocks[idx]->unmap(p));
        }
        if(clear)
            clearSelection();
        texIdx=-1;
        if(drag==false)
        {
            for(int i=0;i<texts.length();i++)
            {
                text_t cur=texts[i];
                QRectF rect(cur.pos.x(),cur.pos.y()-1.0,cur.len,1.0);
                qDebug()<<rect<<startPoint;
                if(rect.contains(startPoint))
                {
                    drag=true;
                    texIdx=i;
                }
            }
        }
    }
    if((event->button()==Qt::LeftButton)&&(event->modifiers()==Qt::CTRL))
    {
        QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
        int block=-1;
        for(int i=0;i<blocks.length();i++)
            if(QRectF(blocks[i]->rect()).contains(pf))
                block=i;
        if(block>=0)
        {
            if(selectedBlocks.contains(block))
                selectedBlocks.removeAll(block);
            else
                selectedBlocks.append(block);
        }
        QPoint p=toGrid(event->pos());
        int line=-1;
        for(int i=0;i<lines.length();i++)
            if(onLine(lines[i].line,p))
            {
                line=i;
            }
        if(line>=0)
        {
            if(selectedLines.contains(line))
                selectedLines.removeAll(line);
            else
                selectedLines.append(line);
        }
        int text=-1;
        for(int i=0;i<texts.length();i++)
        {
            text_t cur=texts[i];
            QRectF rect(cur.pos.x(),cur.pos.y()-1.0,cur.len,1.0);
            if(rect.contains(pf))
            {
                text=i;
            }
        }
        if(text>=0)
        {
            if(selectedTexts.contains(text))
                selectedTexts.removeAll(text);
            else
                selectedTexts.append(text);
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
            if(blkIdx>=0)
            {
                blocks[blkIdx]->m_pos+=curPoint-startPoint;
                for(int i=0;i<selectedBlocks.length();i++)
                    if(selectedBlocks[i]!=blkIdx)
                        blocks[selectedBlocks[i]]->m_pos+=curPoint-startPoint;
                for(int i=0;i<selectedLines.length();i++)
                    lines[selectedLines[i]].line.translate(curPoint-startPoint);
                for(int i=0;i<selectedTexts.length();i++)
                    texts[selectedTexts[i]].pos+=curPoint-startPoint;
            }
            if(texIdx>=0)
                texts[texIdx].pos+=curPoint-startPoint;
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
    qDebug()<<"EVENTR";
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
                    QRectF sel(curPoint,startPoint);
                    for(int i=0;i<texts.length();i++)
                    {
                        text_t cur=texts[i];
                        QRectF rect(cur.pos.x(),cur.pos.y()-1.0,cur.len,1.0);
                        if(rect.intersects(QRectF(sel)))
                            selectedTexts.append(i);
                    }
                    for(int i=0;i<blocks.length();i++)
                    {
                        if(sel.intersects(blocks[i]->rect()))
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
        }
        if(curPoint==startPoint)
        {
            int idx=-1;
            for(int i=0;i<blocks.length();i++)
                if(blocks[i]->rect().contains(curPoint))
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
                    blocks[idx]->block->onclick(blocks[idx]->unmap(p));
                    blocks[idx]->block->onrelease(blocks[idx]->unmap(p));
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
        blocks[i]->block->deleteLater();
    blocks.clear();
    vias.clear();
    texts.clear();
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
        for(int j=0;j<blocks[i]->block->pins.length();j++)
        {
            if(blocks[i]->block->pins[j].direction==2)
                done[i].append(true);
            else
                done[i].append(false);
        }
    }
    QMap<QPair<int,int>,QPair<bool,QColor> > states;
    for(int i=0;i<blocks.length();i++)
        for(int j=0;j<blocks[i]->block->pins.length();j++)
            if(blocks[i]->block->pins[j].direction==2)
            {
                QList<QPair<QPoint,QColor> > points;
                points.append(QPair<QPoint,QColor> (blocks[i]->block->pins[j].pos(),Qt::black));
                QPoint sp=blocks[i]->block->pins[j].pos();
                bool state=blocks[i]->block->getState(j);
                if(blocks[i]->block->pins[j].type)
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
                                    blocks[i]->block->pins[j].color=lines[k].color;
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
                        for(int l=0;l<blocks[k]->block->pins.length();l++)
                            if((blocks[k]->block->pins[l].pos()+QPoint(0,0))==p)
                            {
                                if(blocks[k]->block->pins[l].direction==0)
                                {
                                    done[k][l]=true;
                                    states.insert(QPair<int,int>(k,l),QPair<bool,QColor>(state,pp.second));
                                }
                                if(blocks[k]->block->pins[l].direction==2)
                                {
                                    if(!((i==k)&&(j==l)))
                                    {
                                        ok=false;
                                        bar->showMessage("Zwei Ausgänge",1000);
                                    }
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
        bool state=states[keys[i]].first;
        QColor col=states[keys[i]].second;
        blocks[k]->block->pins[l].state=state;
        blocks[k]->block->pins[l].color=col;
    }
    if(!ok)
    {
        error=true;
        /*for(int i=0;i<blocks.length();i++)
            for(int j=0;j<blocks[i]->block->pins.length();j++)
                if(blocks[i]->block->pins[j].direction==0)
                    blocks[i]->block->pins[j].state=false;
        for(int i=0;i<lines.length();i++)
            lines[i].state=false;*/
        for(int i=0;i<offen.length();i++)
            blocks[offen[i].first]->block->pins[offen[i].second].state=2;
    }
    if(recording==true)
    {
        for(int i=0;i<blocks.length();i++)
        {
            if(blocks[i]->block->name=="OUT")
            {
                dialog.widget->pushValue(i,blocks[i]->block->pins.first().state);
            }
        }
        for(int i=0;i<blocks[recorder]->block->pins.length();i++)
        {
            QPoint p=blocks[recorder]->block->pins[i].pos();
            if(getNet(QLine(p,p)).length()>0)
            {
                dialog.widget->pushValue(-1-i,blocks[recorder]->block->getState(i));
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
    int pblock=-1;
    bool ok=false;
    QMenu menu;
    QPoint p=toGrid(event->pos());
    QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
    for(int i=0;i<blocks.length();i++)
        if(QRectF(blocks[i]->rect()).contains(pf))
            block=i;
    for(int i=0;i<blocks.length();i++)
        if(QRectF(blocks[i]->pinsRect()).contains(pf))
            pblock=i;
    int pin=-1;
    QStringList alt;
    if(pblock>=0)
    {
        for(int i=0;i<blocks[pblock]->block->pins.length();i++)
        {
            if(blocks[pblock]->block->pins[i].pos()==p)
                pin=i;
            if(blocks[pblock]->block->pins[i].direction==0)
                if(blocks[pblock]->block->pins[i].pos()+QPoint(1,0)==p)
                    pin=i;
            if(blocks[pblock]->block->pins[i].direction==2)
                if(blocks[pblock]->block->pins[i].pos()+QPoint(-1,0)==p)
                    pin=i;
        }
        alt=blocks[pblock]->block->alt;
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
        QRectF rect(cur.pos.x(),cur.pos.y()-1.0,cur.len,1.0);
        if(rect.contains(pf))
        {
            text=i;
        }
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
    QMap<QAction*,QColor> setTextColorAction;
    QMap<QAction*,QColor> setLineColorAction;
    QMap<QAction*,QColor> setBlockColorAction;
    QList<QAction*> altAction;
    if(block>=0)
    {
        int cnt=0;
        QList<QString> keys=blocks[block]->block->contextMenu.keys();
        for(auto key:keys)
        {
            int val=blocks[block]->block->contextMenu[key];
            QAction* act=menu.addAction(key);
            blockDefAct.insert(act,val);
            cnt++;
        }
        if(cnt>0)
            menu.addSeparator();
        if(blocks[block]->block->name=="Large-IN")
        {
            impulseBlockAct=menu.addAction("Impulsdiagramm erstellen");
        }
        ok=true;
    }
    if(lcnt>=2)
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
    if(text>=0)
    {
        QList<QPair<QColor,QString> > colors=loadColorProfile();
        QMenu* cm=menu.addMenu("Textfarbe ändern");
        for(int i=0;i<colors.length();i++)
        {
            QPixmap map(24,24);
            map.fill(colors[i].first);
            QAction* act=cm->addAction(QIcon(map),colors[i].second);
            setTextColorAction.insert(act,colors[i].first);
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
            if(blockDefAct.contains(act))
            {
                int func=blockDefAct[act];
                blocks[block]->block->execContext(func);
            }
            if(act==delBlockAct)
            {
                lastSel=-1;
                blocks[block]->block->deleteLater();
                blocks.removeAt(block);
                emit changed();
                clearSelection();
            }
            if(act==impulseBlockAct)
            {
                dialog.show();
                recording=true;
                recorder=block;
            }
            for(int i=0;i<alt.length();i++)
                if(altAction[i]==act)
                {
                    blocks[block]->block->deleteLater();
                    blocks[block]->block=BlockList::newBlock(alt[i]);
                    block_t* blk=blocks[block];
                    for(int i=0;i<blk->block->pins.length();i++)
                        blk->block->pins[i].parent=blk;
                    emit changed();
                    clearSelection();
                }
        }
        if(selectedBlocks.length()>0)
        {
            if(act==delSelAct)
            {
                lastSel=-1;
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
                text.len=1000.0;
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
                blocks[pblock]->block->pins[pin].type=!blocks[pblock]->block->pins[pin].type;
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
            }
        if(line>=0)
            if(setLineColorAction.contains(act))
            {
                QList<int> net=getNet(lines[line].line);
                net.append(line);
                QColor c=setLineColorAction[act];
                for(int i=0;i<net.length();i++)
                    lines[net[i]].color=c;
                lines[line].color=c;
                emit changed();
                clearSelection();
            }
        if(text>=0)
            if(setTextColorAction.contains(act))
            {
                QColor c=setTextColorAction[act];
                qDebug()<<c;
                texts[text].color=c;
                emit changed();
                clearSelection();
            }
        if(block>=0)
            if(setBlockColorAction.contains(act))
            {
                QColor c=setBlockColorAction[act];
                blocks[block]->color=c;
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
                            QPoint kp;
                            if(lines[i].line.p1()==lines[j].line.p1())
                                if(lines[i].line.p2()!=lines[j].line.p2())
                                {
                                    if(lines[i].line.p2().x()==lines[j].line.p2().x())
                                    {
                                        kill=true;
                                        kp=lines[i].line.p1();
                                    }
                                    if(lines[i].line.p2().y()==lines[j].line.p2().y())
                                    {
                                        kp=lines[i].line.p1();
                                        kill=true;
                                    }
                                }
                            if(lines[i].line.p2()==lines[j].line.p2())
                                if(lines[i].line.p1()!=lines[j].line.p1())
                                {
                                    if(lines[i].line.p1().x()==lines[j].line.p1().x())
                                    {
                                        kp=lines[i].line.p2();
                                        kill=true;
                                    }
                                    if(lines[i].line.p1().y()==lines[j].line.p1().y())
                                    {
                                        kp=lines[i].line.p2();
                                        kill=true;
                                    }
                                }
                            if(lines[i].line.p1()==lines[j].line.p2())
                                if(lines[i].line.p2()!=lines[j].line.p1())
                                {
                                    if(lines[i].line.p2().x()==lines[j].line.p1().x())
                                    {
                                        kp=lines[i].line.p1();
                                        kill=true;
                                    }
                                    if(lines[i].line.p2().y()==lines[j].line.p1().y())
                                    {
                                        kp=lines[i].line.p1();
                                        kill=true;
                                    }
                                }
                            if(lines[i].line.p2()==lines[j].line.p1())
                                if(lines[i].line.p1()!=lines[j].line.p2())
                                {
                                    if(lines[i].line.p1().x()==lines[j].line.p2().x())
                                    {
                                        kp=lines[i].line.p2();
                                        kill=true;
                                    }
                                    if(lines[i].line.p1().y()==lines[j].line.p2().y())
                                    {
                                        kp=lines[i].line.p2();
                                        kill=true;
                                    }
                                }
                            if(kill)
                            {
                                if(vias.contains(kp))
                                    kill=false;
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
            for(int k=0;k<blocks[j]->block->pins.length();k++)
                testVias.append(blocks[j]->block->pins[k].pos());
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
                for(int k=0;k<blocks[j]->block->pins.length();k++)
                    if((blocks[j]->block->pins[k].pos())==point)
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
    resizeNow();
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
        for(int j=0;j<blocks[i]->block->pins.length();j++)
            if(onLine(line,blocks[i]->block->pins[j].pos()))
                ret.append(blocks[i]->block->pins[j].pos());
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
    for(int i=0;i<blocks.length();i++)
        painter.drawPicture(blocks[i]->m_pos*Settings::final()->gridSize(),blocks[i]->block->drawBlock(blocks[i]->color,true));
    QPen line(Qt::black);
    line.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
    for(int i=0;i<lines.length();i++)
    {
        line.setColor(lines[i].color);
        painter.setPen(line);
        painter.drawLine(lines[i].line.p1()*Settings::final()->gridSize(),lines[i].line.p2()*Settings::final()->gridSize());
    }
    for(int i=0;i<texts.length();i++)
    {
        QFont font("Arial");
        font.setPixelSize(Settings::final()->gridSize());
        painter.setFont(font);
        text_t cur=texts[i];
        line.setColor(texts[i].color);
        painter.setPen(line);
        QRect rect((cur.pos.x()*Settings::final()->gridSize()),(cur.pos.y()*Settings::final()->gridSize())-Settings::final()->gridSize(),texts[i].len*Settings::final()->gridSize(),Settings::final()->gridSize());
        painter.drawText(rect,Qt::AlignBottom|Qt::AlignLeft,cur.text);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    for(int i=0;i<vias.length();i++)
        painter.drawEllipse(vias[i]*Settings::final()->gridSize(),int(0.25*Settings::final()->gridSize()),int(0.25*Settings::final()->gridSize()));
    painter.end();
    qDebug()<<picture.boundingRect();
    return picture;
}

void DigiView::loadJson(QByteArray json)
{
    lastSel=-1;
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
        c.color=QColor(t[i].toObject()["color"].toString());
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
        block_t *c=new block_t;
        c->m_pos=QPoint(g[i].toObject()["x"].toInt(),g[i].toObject()["y"].toInt());
        c->block=0;
        BlockList list;
        for(int j=0;j<list.blocks.length();j++)
            if(list.blocks[j]->name==g[i].toObject()["name"].toString())
            {
                c->block=list.blocks[j]->clone();
            }
        QJsonArray pins=g[i].toObject()["pins"].toArray();
        for(int j=0;j<pins.size();j++)
        {
            QJsonObject pin=pins[j].toObject();
            if(c->block->pins.length()>j)
                c->block->pins[j].type=pin["type"].toBool();
        }
        c->color=QColor(g[i].toObject()["color"].toString());
        if(c->block!=0)
        {
            for(int i=0;i<c->block->pins.length();i++)
                c->block->pins[i].parent=c;
            blocks.append(c);
        }
    }
    for(int i=0;i<blocks.length();i++)
    {
        QJsonObject c;
        c.insert("x",blocks[i]->m_pos.x());
        c.insert("y",blocks[i]->m_pos.y());
        c.insert("name",blocks[i]->block->name);
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
    lastSel=-1;
    for(int i=0;i<selectedBlocks.length();i++)
        blocks[selectedBlocks[i]]->block->deleteLater();
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
        if(!vias.contains(p))
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

void DigiView::createTable()
{
    QPoint pos(-1,-1);
    Block* lin;
    for(int i=0;i<blocks.length();i++)
    {
        if(blocks[i]->block->name=="Large-IN")
        {
            pos=blocks[i]->m_pos;
            lin=blocks[i]->block;
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
            if(blocks[i]->block->name=="OUT")
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
                res.append(blocks[outputs[j]]->block->pins[0].state);
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

QList<int> DigiView::linesAtPoint(QPoint point)
{
    QList<int> ret;
    for(int i=0;i<lines.length();i++)
    {
        if(lines[i].line.p1()==point)
            ret.append(i);
        if(lines[i].line.p2()==point)
            ret.append(i);
    }
    return ret;
}

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
        if(blocks[lastSel]->block->name=="Large-IN")
        {
            blocks[lastSel]->block->keyPress(o);
        }
}

void DigiView::emitKey(QString key)
{
    qDebug()<<"Emitting Key";
    if(lastSel>=0)
    {
        qDebug()<<"Last Sel is "<<blocks[lastSel]->block->name;
        blocks[lastSel]->block->keyPressNorm(key);
    }
}

QJsonObject DigiView::exportJSON()
{
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
        c.insert("color",texts[i].color.name());
        t.append(c);
    }
    root.insert("texts",t);
    QJsonArray g;
    for(int i=0;i<blocks.length();i++)
    {
        QJsonObject c;
        c.insert("x",blocks[i]->m_pos.x());
        c.insert("y",blocks[i]->m_pos.y());
        c.insert("name",blocks[i]->block->name);
        c.insert("color",blocks[i]->color.name());
        QJsonArray pins;
        for(int j=0;j<blocks[i]->block->pins.length();j++)
        {
            QJsonObject obj;
            obj.insert("type",blocks[i]->block->pins[j].type);
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
    return root;
}

void DigiView::resizeEvent(QResizeEvent *event)
{
}

void DigiView::resizeNow()
{
    int bwid=0;
    int bhei=0;
    for(int i=0;i<blocks.length();i++)
    {
        QPointF rect=blocks[i]->rect().bottomRight();
        bwid=fmax(bwid,rect.x());
        bhei=fmax(bhei,rect.y());
    }
    for(int i=0;i<lines.length();i++)
    {
        QPointF rect=lines[i].line.p1();
        bwid=fmax(bwid,rect.x());
        bhei=fmax(bhei,rect.y());
        rect=lines[i].line.p2();
        bwid=fmax(bwid,rect.x());
        bhei=fmax(bhei,rect.y());
    }
    for(int i=0;i<texts.length();i++)
    {
        QPointF rect=QRect(texts[i].pos.x(),texts[i].pos.y(),texts[i].len,1).bottomRight();
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
