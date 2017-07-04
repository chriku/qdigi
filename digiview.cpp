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
    rootSchematic=new Schematic;
    rootSchematic->name="ROOT";
    curSchematic=rootSchematic;
    drag=false;
    lastSel=-1;
    recording=false;
    setFocus();
    lastContext=QTime::currentTime();
    startPoint=QPoint(-1,-1);
    curPoint=QPoint(-1,-1);
    BlockList list;
    setAcceptDrops(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout()));
    connect(&timer2,SIGNAL(timeout()),this,SLOT(update()));
    timer.setInterval(Settings::final()->simulationTime());
    setContextMenuPolicy(Qt::DefaultContextMenu);
    resizeNow();
    setMouseTracking(true);
    timer2.start(40);
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
            painter.drawPicture(dragPos.x()*Settings::final()->gridSize(),dragPos.y()*Settings::final()->gridSize(),block->draw());
        }
    }
    for(auto item:curSchematic->items)
    {
        painter.drawPicture(toScreen(item->pos),item->draw());
    }
    QPen pen(Qt::green);
    QBrush brush(Qt::green);
    painter.setPen(pen);
    brush.setStyle(Qt::Dense6Pattern);
    brush.setColor(QColor::fromRgbF(0.0,1.0,0.0,0.5));
    painter.setBrush(brush);
    for(int i=0;i<selection.length();i++)
    {
        QRectF rect=curSchematic->items[selection[i]]->clickRect();
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
        QRectF rect=curSchematic->blocks[lastSel]->clickRect();
        rect.setTop(rect.top()*(Settings::final()->gridSize()));
        rect.setLeft(rect.left()*(Settings::final()->gridSize()));
        rect.setBottom(rect.bottom()*Settings::final()->gridSize());
        rect.setRight(rect.right()*Settings::final()->gridSize());
        painter.drawRect(rect);
    }
    painter.setBrush(Qt::NoBrush);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    for(int i=0;i<curSchematic->blocks.length();i++)
    {
        for(int j=0;j<curSchematic->blocks[i]->pins.length();j++)
            if(curSchematic->blocks[i]->pins[j].state==2)
            {
                QPen pen(QColor::fromRgbF(1,0.5,0));
                pen.setWidth(0);
                painter.setPen(pen);
                for(int r=5;r<(Settings::final()->gridSize());r+=5)
                    painter.drawEllipse((curSchematic->blocks[i]->pins[j].pos() * Settings::final()->gridSize()),r,r);
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
    clearSelection();
    curSchematic->addBlock(dragPos.toPoint(),dragGate);
    change();
    if(!dragMany)
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
            clearSelection();
            curSchematic->addBlock(dragPos.toPoint(),dragGate);
            change();
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
        for(int i=0;i<curSchematic->blocks.length();i++)
            if(curSchematic->blocks[i]->clickRect().contains(curPoint))
            {
                idx=i;
            }
        int pin=-1;
        if(idx>=0)
            if(curSchematic->blocks[idx]->checkable)
                lastSel=idx;
        if(idx>=0)
            for(int i=0;i<curSchematic->blocks[idx]->pins.length();i++)
                if(startPoint==curSchematic->blocks[idx]->pins[i].pos())
                    pin=i;
        drag=false;
        dragIdx=-1;
        for(int i=0;i<curSchematic->items.length();i++)
            if(curSchematic->items[i]->clickRect().contains(curPoint))
                if(curSchematic->isText(curSchematic->items[i]))
                {
                    dragIdx=i;
                    drag=true;
                }
        if((pin==-1)&&(idx>=0))
        {
            clear=false;
            startBlock=curSchematic->blocks[idx]->pos;
            drag=true;
        }
        if(idx>=0)
        {
            double x=event->pos().x()/Settings::final()->gridSize();
            double y=event->pos().y()/Settings::final()->gridSize();
            QPointF p(x,y);
            curSchematic->blocks[idx]->onpress(curSchematic->blocks[idx]->unmap(p));
        }
        if(clear)
            clearSelection();
    }
    if((event->button()==Qt::LeftButton)&&(event->modifiers()==Qt::CTRL))
    {
        QPointF pf=QPointF(event->pos())/Settings::final()->gridSize();
        for(int i=0;i<curSchematic->items.length();i++)
            if(curSchematic->items[i]->clickRect().contains(pf))
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
            change();
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
            change();
            dragged=true;
            if(dragIdx>=0)
                curSchematic->items[dragIdx]->pos+=curPoint-startPoint;
            for(auto i:selection)
                if(i!=dragIdx)
                    curSchematic->items[i]->pos+=curPoint-startPoint;
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
                    for(int i=0;i<curSchematic->lines.length();i++)
                        if(curSchematic->onLine(curSchematic->lines[i]->line,startPoint)||curSchematic->onLine(curSchematic->lines[i]->line,curPoint))
                            line->color=curSchematic->lines[i]->color;
                    curSchematic->lines.append(line);
                    curSchematic->items.append(line);
                    curSchematic->cleanUp();
                    change();
                }
            if(curPoint!=startPoint)
                if(!(((curPoint.x()!=startPoint.x())&&(curPoint.y()==startPoint.y()))||((curPoint.y()!=startPoint.y())&&(curPoint.x()==startPoint.x()))))
                {
                    QRectF sel(curPoint,startPoint);
                    for(int i=0;i<curSchematic->items.length();i++)
                        if(curSchematic->items[i]->clickRect().intersects(sel))
                            selection.append(i);
                }
        }
        if(curPoint==startPoint)
        {
            int idx=-1;
            for(int i=0;i<curSchematic->blocks.length();i++)
                if(curSchematic->blocks[i]->clickRect().contains(curPoint))
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
                    curSchematic->blocks[idx]->onclick(curSchematic->blocks[idx]->unmap(p));
                    curSchematic->blocks[idx]->onrelease(curSchematic->blocks[idx]->unmap(p));
                }
            }
        }
        startPoint=QPoint(-1,-1);
        curPoint=QPoint(-1,-1);
    }
    curSchematic->cleanUp();
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
    QJsonObject root=rootSchematic->exportJSON();
    QJsonArray childSchema;
    for(auto child:childSchematics)
        childSchema.append(child->exportJSON());
    root.insert("children",childSchema);
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
    undoBuf.clear();
    redoBuf.clear();
    QByteArray data;
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
        if(file.size()>4)
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
            data=file.readAll();
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
                zip_file_t* dfile=zip_fopen(arch,"data.json",0);
                int len;
                do {
                    QByteArray buf(1024,0);
                    len=zip_fread(dfile,buf.data(),buf.length());
                    buf=buf.left(len);
                    data+=buf;
                }while(len>0);
            }
            else
            {
                QMessageBox::warning(NULL,"QDigi Fehler","Unbekanntes Dateiformat");
            }
            zip_close(arch);
            file.close();
        }
    }
    rootSchematic->deleteLater();
    rootSchematic=new Schematic;
    curSchematic=rootSchematic;
    QJsonObject root=QJsonDocument::fromJson(data).object();
    QMap<QString,QJsonObject> subItems;
    QJsonArray sub=root["children"].toArray();
    QList<QString> load;
    for(int i=0;i<sub.size();i++)
    {
        QString name=sub[i].toObject()["name"].toString();
        subItems.insert(name,sub[i].toObject());
        load.append(name);
    }
    while(load.length()>0)
    {
        QString cur=load.takeFirst();
        QStringList depends;
        for(int i=0;i<subItems[cur]["depends"].toArray().size();i++)
            depends.append(subItems[cur]["depends"].toArray()[i].toString());
        bool ok=true;
        for(int i=0;i<depends.length();i++)
            if(cur.contains(depends[i]))
                ok=false;
        qDebug()<<ok;
        if(ok)
        {
            Schematic* sc=new Schematic;;
            sc->loadJson(subItems[cur]);
            childSchematics.append(sc);
            mwp->updateBlocks();
        }
        else
            load.append(cur);
    }
    rootSchematic->loadJson(root);
    qDebug()<<data;
}

QPoint DigiView::toGrid(QPoint in)
{
    int x=round(in.x()/Settings::final()->gridSize());
    int y=round(in.y()/Settings::final()->gridSize());
    return QPoint(x,y);
}

void DigiView::timeout()
{
    timer.setInterval(Settings::final()->simulationTime());
    error=!curSchematic->simulate();
    if(error)
        mwp->statusBar()->showMessage(curSchematic->errorStr,1000);
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
    for(int i=0;i<curSchematic->blocks.length();i++)
        if(QRectF(curSchematic->blocks[i]->clickRect()).contains(pf))
            block=i;
    int pin=-1;
    if(block>=0)
    {
        for(int i=0;i<curSchematic->blocks[block]->pins.length();i++)
        {
            if(curSchematic->blocks[block]->pins[i].pos()==p)
                pin=i;
            if(curSchematic->blocks[block]->pins[i].direction==0)
                if(curSchematic->blocks[block]->pins[i].pos()+QPoint(1,0)==p)
                    pin=i;
            if(curSchematic->blocks[block]->pins[i].direction==2)
                if(curSchematic->blocks[block]->pins[i].pos()+QPoint(-1,0)==p)
                    pin=i;
        }
    }
    for(int i=0;i<curSchematic->items.length();i++)
    {
        if(curSchematic->items[i]->clickRect().contains(pf))
            clickedItems.append(i);
    }
    QAction* delBlockAct=NULL;
    QAction* delILAct=NULL;
    QAction* delJLAct=NULL;
    QAction* addViaAct=NULL;
    QAction* delLineAct=NULL;
    QAction* delSelAct=NULL;
    QAction* delLineNetAct=NULL;
    QAction* delViaAct=NULL;
    QAction* delTextAct=NULL;
    QAction* changePinAct=NULL;
    QAction* addTextAct=NULL;
    QAction* addILAct=NULL;
    QAction* addJLAct=NULL;
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
        Item* item=curSchematic->items[clicked];
        if(curSchematic->isLine(item))
            lcnt++;
    }
    Block* blk=NULL;
    Line* line=NULL;
    ImpulseLabel* impulseLabel=NULL;
    JumpLabel* jumpLabel=NULL;
    Via* via=NULL;
    Text* text=NULL;
    bool colorC=false;
    bool lineC=false;
    for(auto clicked:clickedItems)
    {
        Item* item=curSchematic->items[clicked];
        if(curSchematic->isBlock(item))
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
        if(!curSchematic->isVia(item))
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
        if(curSchematic->isVia(item))
        {
            via=(Via*)item;
            delViaAct=menu.addAction("Knotenpunkt Löschen");
            ok=true;
        }
        if(curSchematic->isText(item))
        {
            text=(Text*)item;
            delTextAct=menu.addAction("Text Löschen");
            ok=true;
        }
        if(curSchematic->isImpulseLabel(item))
        {
            impulseLabel=(ImpulseLabel*)item;
            delILAct=menu.addAction("Label Löschen");
            ok=true;
        }
        if(curSchematic->isJumpLabel(item))
        {
            jumpLabel=(JumpLabel*)item;
            delJLAct=menu.addAction("Label Löschen");
            ok=true;
        }
        if(curSchematic->isLine(item)&&(!lineC))
        {
            lineC=true;
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
    addILAct=menu.addAction("Impuls Label hinzufügen");
    addJLAct=menu.addAction("Jump Label hinzufügen");
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
                    curSchematic->blocks.removeAll(blk);
                    curSchematic->items.removeAll(blk);
                    change();
                    return;
                }
                for(int i=0;i<alt.length();i++)
                    if(altAction[i]==act)
                    {
                        clearSelection();
                        curSchematic->items.removeAll(blk);
                        curSchematic->blocks.removeAll(blk);
                        clearSelection();
                        curSchematic->addBlock(blk->pos,alt[i]);
                        change();
                        Block* nblk=curSchematic->blocks.last();
                        for(int i=0;i<nblk->pins.length();i++)
                            nblk->pins[i].parent=nblk;
                        for(int i=0;i<nblk->pins.length();i++)
                            nblk->pins[i].type=blk->pins[i].type;
                        blk->deleteLater();
                        change();
                        clearSelection();
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
                    change();
                    return;
                }
                if(setSelectionColorAction.contains(act))
                {
                    QColor c=setSelectionColorAction[act];
                    for(auto item:selection)
                        curSchematic->items[item]->color=c;
                    change();
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
                    curSchematic->texts.append(text);
                    curSchematic->items.append(text);
                }
                change();
                return;
            }
            if(act==addILAct)
            {
                clearSelection();
                bool ok;
                QString message=QInputDialog::getText(NULL,"QDigi","Label Einfügen",QLineEdit::Normal,QString(),&ok);
                if(ok)
                {
                    ImpulseLabel* il=new ImpulseLabel;
                    il->pos=p;
                    il->name=message;
                    curSchematic->impulseLabels.append(il);
                    curSchematic->items.append(il);
                }
                change();
                return;
            }
            if(act==addJLAct)
            {
                clearSelection();
                bool ok;
                QString message=QInputDialog::getText(NULL,"QDigi","Label Einfügen",QLineEdit::Normal,QString(),&ok);
                if(ok)
                {
                    JumpLabel* il=new JumpLabel;
                    il->pos=p;
                    il->name=message;
                    curSchematic->jumpLabels.append(il);
                    curSchematic->items.append(il);
                }
                change();
                return;
            }
            if(via!=NULL)
                if(act==delViaAct)
                {
                    clearSelection();
                    curSchematic->vias.removeAll(via);
                    curSchematic->items.removeAll(via);
                    via->deleteLater();
                    change();
                    return;
                }
            if(via==NULL)
            {
                if(setColorAction.contains(act))
                {
                    clearSelection();
                    QColor c=setColorAction[act];
                    for(auto idx:clickedItems)
                        curSchematic->items[idx]->color=c;
                    change();
                    clearSelection();
                    return;
                }
            }
            if(pin>=0)
                if(act==changePinAct)
                {
                    clearSelection();
                    blk->pins[pin].type=!blk->pins[pin].type;
                    change();
                    return;
                }
            if(text!=NULL)
                if(act==delTextAct)
                {
                    clearSelection();
                    curSchematic->texts.removeAll(text);
                    curSchematic->items.removeAll(text);
                    text->deleteLater();
                    change();
                    return;
                }
            if(impulseLabel!=NULL)
                if(act==delILAct)
                {
                    clearSelection();
                    curSchematic->impulseLabels.removeAll(impulseLabel);
                    curSchematic->items.removeAll(impulseLabel);
                    impulseLabel->deleteLater();
                    change();
                    return;
                }
            if(jumpLabel!=NULL)
                if(act==delILAct)
                {
                    clearSelection();
                    curSchematic->jumpLabels.removeAll(jumpLabel);
                    curSchematic->items.removeAll(jumpLabel);
                    jumpLabel->deleteLater();
                    change();
                    return;
                }
            if(lcnt==2)
                if(act==addViaAct)
                {
                    clearSelection();
                    Via* via=new Via;
                    via->pos=p;
                    curSchematic->vias.append(via);
                    curSchematic->items.append(via);
                    curSchematic->cleanUp();
                    change();
                    return;
                }
            if(line!=NULL)
                if(act==delLineAct)
                {
                    clearSelection();
                    curSchematic->lines.removeAll(line);
                    curSchematic->items.removeAll(line);
                    line->deleteLater();
                    change();
                    return;
                }
            if(line!=NULL)
                if(act==delLineNetAct)
                {
                    clearSelection();
                    curSchematic->cleanUp();
                    QList<QPoint> points;
                    points.append(line->line.p1());
                    points.append(line->line.p2());
                    curSchematic->lines.removeAll(line);
                    curSchematic->items.removeAll(line);
                    line->deleteLater();
                    while(points.length()>0)
                    {
                        bool ok=false;
                        QList<Line*> del;
                        QPoint point=points.takeFirst();
                        for(int i=0;i<curSchematic->lines.length();i++)
                        {
                            if(curSchematic->lines[i]->line.p1()==point)
                            {
                                del.append(curSchematic->lines[i]);
                                ok=true;
                            }
                            else if(curSchematic->lines[i]->line.p2()==point)
                            {
                                del.append(curSchematic->lines[i]);
                                ok=true;
                            }
                        }
                        for(int i=0;i<curSchematic->vias.length();i++)
                            if(curSchematic->vias[i]->pos==point)
                                ok=false;
                        qDebug()<<ok<<del;
                        if(ok)
                            for(auto l:del)
                            {
                                points.append(l->line.p1());
                                points.append(l->line.p2());
                                l->deleteLater();
                                curSchematic->items.removeAll(l);
                                curSchematic->lines.removeAll(l);
                            }
                    }
                    curSchematic->cleanUp();
                    change();
                    return;
                }
        }
    }
    curSchematic->cleanUp();
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



QList<QPoint> DigiView::allIntersect(QLine line)
{
    QList<QPoint> ret;
    ret.append(line.p1());
    ret.append(line.p2());
    for(int i=0;i<curSchematic->lines.length();i++)
    {
        if(curSchematic->onLine(curSchematic->lines[i]->line,line.p1())||curSchematic->onLine(curSchematic->lines[i]->line,line.p2()))
        {
            ret.append(curSchematic->lines[i]->line.p1());
            ret.append(curSchematic->lines[i]->line.p2());
        }
    }
    for(int i=0;i<curSchematic->lines.length();i++)
    {
        if(curSchematic->onLine(line,curSchematic->lines[i]->line.p1()))
            ret.append(curSchematic->lines[i]->line.p1());
        if(curSchematic->onLine(line,curSchematic->lines[i]->line.p2()))
            ret.append(curSchematic->lines[i]->line.p2());
    }
    for(int i=0;i<curSchematic->blocks.length();i++)
        for(int j=0;j<curSchematic->blocks[i]->pins.length();j++)
            if(curSchematic->onLine(line,curSchematic->blocks[i]->pins[j].pos()))
                ret.append(curSchematic->blocks[i]->pins[j].pos());
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
    for(int i=0;i<curSchematic->items.length();i++)
        painter.drawPicture(curSchematic->items[i]->pos*Settings::final()->gridSize(),curSchematic->items[i]->draw());
    painter.end();
    return picture;
}



void DigiView::clearSelection()
{
    selection.clear();
    update();
}

void DigiView::deleteSelection()
{
    lastSel=-1;
    QList<Item*> del;
    for(int i=0;i<selection.length();i++)
        del.append(curSchematic->items[selection[i]]);
    for(int i=0;i<selection.length();i++)
    {
        Item* item=del[i];
        item->deleteLater();
        curSchematic->texts.removeAll((Text*)item);
        curSchematic->vias.removeAll((Via*)item);
        curSchematic->impulseLabels.removeAll((ImpulseLabel*)item);
        curSchematic->jumpLabels.removeAll((JumpLabel*)item);
        curSchematic->lines.removeAll((Line*)item);
        curSchematic->blocks.removeAll((Block*)item);
        curSchematic->items.removeAll(item);
    }
    clearSelection();
    curSchematic->cleanUp();
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
    for(int i=0;i<curSchematic->blocks.length();i++)
    {
        if(curSchematic->blocks[i]->name=="Large-IN")
        {
            pos=curSchematic->blocks[i]->pos;
            lin=curSchematic->blocks[i];
            i=curSchematic->blocks.length();
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
        QList<QPoint> pins;
        QMap<int,int> fakeGates;
        QList<int> outputs;
        for(int i=0;i<curSchematic->blocks.length();i++)
            if(curSchematic->blocks[i]->name=="OUT")
                outputs.append(i);
        while(cnt<16)
        {
            QPoint pin(pos+QPoint(2,cnt+1));
            if(curSchematic->getItemsForOutput(pin,NULL).length()<=1)
                break;
            pins.append(pin);
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
                res.append(curSchematic->blocks[outputs[j]]->pins[0].state);
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
        if(curSchematic->blocks[lastSel]->name=="Large-IN")
        {
            curSchematic->blocks[lastSel]->keyPress(o);
        }
}

void DigiView::emitKey(QString key)
{
    qDebug()<<"Emitting Key";
    if(lastSel>=0)
    {
        qDebug()<<"Last Sel is "<<curSchematic->blocks[lastSel]->name;
        curSchematic->blocks[lastSel]->keyPressNorm(key);
    }
}



void DigiView::resizeEvent(QResizeEvent *event)
{
}

void DigiView::resizeNow()
{
    int bwid=0;
    int bhei=0;
    for(int i=0;i<curSchematic->items.length();i++)
    {
        QPointF rect=curSchematic->items[i]->clickRect().bottomRight();
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



QPoint DigiView::toScreen(QPointF pos)
{
    int x=round(pos.x()*Settings::final()->gridSize());
    int y=round(pos.y()*Settings::final()->gridSize());
    return QPoint(x,y);
}


void DigiView::change()
{
    emit changed();
    qDebug()<<"Changed";
    QByteArray data=QJsonDocument(curSchematic->exportJSON()).toJson(QJsonDocument::Compact);
    if((undoBuf.length()==0)||(data!=undoBuf.last()))
    {
        redoBuf.clear();
        undoBuf.append(data);
    }
    curSchematic->cleanUp();
    resizeNow();

}

void DigiView::undo()
{
    qDebug()<<undoBuf;
    if(undoBuf.length()>1) {
        undoBuf.removeLast();
        QByteArray cur=undoBuf.last();
        redoBuf.append(cur);
        curSchematic->loadJson(cur);
    }
}

void DigiView::redo()
{
    if(redoBuf.length()>1) {
        redoBuf.removeLast();
        QByteArray cur=redoBuf.last();
        undoBuf.append(cur);
        curSchematic->loadJson(cur);
    }
}

void DigiView::purgeSchematic()
{
    lastSel=-1;
    update();
}
