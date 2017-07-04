#include "schematic.h"
#include <math.h>
#include <algorithm>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
Schematic::Schematic(QObject *parent) : QObject(parent)
{

}

void Schematic::loadJson(QByteArray json)
{
    loadJson(QJsonDocument::fromJson(json).object());
}

void Schematic::loadJson(QJsonObject json,bool delIt)
{
    emit purgeSelection();
    if(delIt)
    {
        lines.clear();
        for(int i=0;i<blocks.length();i++)
            blocks[i]->deleteLater();
        blocks.clear();
        vias.clear();
        texts.clear();
        items.clear();
        impulseLabels.clear();
        jumpLabels.clear();
    }
    QJsonObject root=json;
    name=root["name"].toString();
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
    QJsonArray j=root["impulseLabels"].toArray();
    for(int i=0;i<j.size();i++)
    {
        ImpulseLabel* c=new ImpulseLabel;
        c->pos=QPoint(j[i].toObject()["x"].toInt(),j[i].toObject()["y"].toInt());
        c->name=j[i].toObject()["name"].toString();
        impulseLabels.append(c);
        items.append(c);
    }
    QJsonArray k=root["jumpLabels"].toArray();
    for(int i=0;i<k.size();i++)
    {
        JumpLabel* c=new JumpLabel;
        c->pos=QPoint(k[i].toObject()["x"].toInt(),k[i].toObject()["y"].toInt());
        c->name=k[i].toObject()["name"].toString();
        jumpLabels.append(c);
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
}
QJsonObject Schematic::exportJSON()
{
    return exportJSONPart(items);
}

QJsonObject Schematic::exportJSONPart(QList<Item*> eitems)
{
    QJsonObject root;
    root.insert("name",name);
    QJsonArray depends;
    QJsonArray l;
    QJsonArray k;
    QJsonArray v;
    QJsonArray t;
    QJsonArray j;
    QJsonArray g;
    for(int i=0;i<eitems.length();i++)
    {
        Item* item=eitems[i];
        Block* block=(Block*)item;
        Line* line=(Line*)item;
        Via* via=(Via*)item;
        Text* text=(Text*)item;
        ImpulseLabel* il=(ImpulseLabel*)item;
        JumpLabel* jl=(JumpLabel*)item;
        if(isBlock(item))
        {
            if(block->subItem)
                depends.append(block->name);
            QJsonObject c;
            c.insert("x",block->pos.x());
            c.insert("y",block->pos.y());
            c.insert("name",block->name);
            c.insert("color",block->color.name());
            QJsonArray pins;
            for(int j=0;j<block->pins.length();j++)
            {
                QJsonObject obj;
                obj.insert("type",block->pins[j].type);
                pins.append(obj);
            }
            c.insert("pins",pins);
            g.append(c);
        }
        if(isLine(item))
        {
            QJsonObject c;
            c.insert("x1",line->line.x1());
            c.insert("y1",line->line.y1());
            c.insert("x2",line->line.x2());
            c.insert("y2",line->line.y2());
            c.insert("color",line->color.name());
            l.append(c);
        }
        if(isText(item))
        {
            QJsonObject c;
            c.insert("x",text->pos.x());
            c.insert("y",text->pos.y());
            c.insert("text",text->text);
            c.insert("color",text->color.name());
            t.append(c);
        }
        if(isImpulseLabel(item))
        {
            QJsonObject c;
            c.insert("x",il->pos.x());
            c.insert("y",il->pos.y());
            c.insert("name",il->name);
            j.append(c);
        }
        if(isJumpLabel(item))
        {
            QJsonObject c;
            c.insert("x",jl->pos.x());
            c.insert("y",jl->pos.y());
            c.insert("name",jl->name);
            k.append(c);
        }
        if(isVia(item))
        {
            QJsonObject c;
            c.insert("x",via->pos.x());
            c.insert("y",via->pos.y());
            v.append(c);
        }
    }
    root.insert("texts",t);
    root.insert("impulseLabels",j);
    root.insert("lines",l);
    root.insert("jumpLabels",k);
    root.insert("blocks",g);
    root.insert("depends",depends);
    root.insert("vias",v);
    return root;
}

void Schematic::addBlock(QPoint pos, QString type)
{
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
    cleanUp();
}

QList<QPair<int,int> > Schematic::getItemsForOutput(QPoint pos, QList<int> *witems)
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
                if(witems!=NULL)
                    witems->append(items.indexOf(lines[i]));
            }
            if(lines[i]->line.p2()==point)
            {
                points.append(lines[i]->line.p1());
                if(witems!=NULL)
                    witems->append(items.indexOf(lines[i]));
            }
        }
        for(int i=0;i<vias.length();i++)
        {
            if(vias[i]->pos==point)
                if(witems!=NULL)
                    witems->append(items.indexOf(vias[i]));
        }
        for(int i=0;i<impulseLabels.length();i++)
        {
            if(impulseLabels[i]->pos==point)
                if(witems!=NULL)
                    witems->append(items.indexOf(impulseLabels[i]));
        }
        for(int i=0;i<jumpLabels.length();i++)
        {
            if(jumpLabels[i]->pos==point)
            {
                QString name=jumpLabels[i]->name;
                for(auto lab:jumpLabels)
                    if(lab->name==name)
                        points.append(lab->pos);
                if(witems!=NULL)
                    witems->append(items.indexOf(jumpLabels[i]));
            }
        }
        for(int i=0;i<jumpLabels.length();i++)
        {
            if(jumpLabels[i]->pos==point)
                if(witems!=NULL)
                    witems->append(items.indexOf(jumpLabels[i]));
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


bool Schematic::isLine(Item *item)
{
    if(lines.contains((Line*)item))
        return true;
    return false;
}

bool Schematic::isText(Item *item)
{
    if(texts.contains((Text*)item))
        return true;
    return false;
}

bool Schematic::isImpulseLabel(Item *item)
{
    if(impulseLabels.contains((ImpulseLabel*)item))
        return true;
    return false;
}

bool Schematic::isJumpLabel(Item *item)
{
    if(jumpLabels.contains((JumpLabel*)item))
        return true;
    return false;
}

bool Schematic::isVia(Item *item)
{
    if(vias.contains((Via*)item))
        return true;
    return false;
}

bool Schematic::isBlock(Item *item)
{
    if(blocks.contains((Block*)item))
        return true;
    return false;
}

void Schematic::check()
{
    for(auto item:items)
    {
        if(!(isText(item)||isBlock(item)||isVia(item)||isLine(item)||isImpulseLabel(item)||isJumpLabel(item)))
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
    for(auto il:impulseLabels)
    {
        if(!items.contains(il))
        {
            qDebug()<<"INVALID IMPULSELABEL"<<il->name;
            exit(1);
        }
    }
    for(auto il:jumpLabels)
    {
        if(!items.contains(il))
        {
            qDebug()<<"INVALID JUMPLABEL"<<il->name;
            exit(1);
        }
    }    for(auto block:blocks)
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

QList<int> Schematic::getNet(QLine in)
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

void Schematic::cleanUp()
{
    check();
    for(auto line:lines)
    {
        line->line.setP1(line->line.p1()+line->pos);
        line->line.setP2(line->line.p2()+line->pos);
        line->pos=QPoint(0,0);
    }
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
                                    lines[i]->deleteLater();
                                    items.removeAll((Item*)lines[i]);
                                    lines.removeAt(i);
                                    if(j>i)
                                        j--;
                                    lines[j]->deleteLater();
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
                                lines[i]->deleteLater();
                                items.removeAll(lines[i]);
                                lines.removeAt(i);
                                if(j>i)
                                    j--;
                                i--;
                                lines[j]->deleteLater();
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
                            line->deleteLater();
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
}

bool Schematic::onLine(QLine line, QPoint point, bool proper)
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

bool smaller(QString a,QString b)
{
    for(int i=0;i<fmin(a.length(),b.length());i++)
    {
        if(a.at(i)<b.at(i))
            return true;
        else if(a.at(i)>b.at(i))
            return false;
    }
    if(a.length()>b.length())
        return false;
    if(a.length()<b.length())
        return true;
    return true;
}

QList<QString> Schematic::getInputs()
{
    QList<QString> ret;
    for(auto key:getInputList())
        ret.append(key.first);
    bool changed=false;
    do{
        changed=false;
        for(int i=1;i<ret.length();i++)
            if(!smaller(ret[i-1],ret[i]))
            {
                qDebug()<<ret[i-1]<<ret[i]<<smaller(ret[i-1],ret[i]);
                ret.swap(i-1,i);
                changed=true;
                continue;
            }
    }
    while(changed);
    qDebug()<<ret;
    return ret;
}

QList<QString> Schematic::getOutputs()
{
    QList<QString> ret;
    for(auto key:getOutputList())
        ret.append(key.first);
    bool changed=false;
    do{
        changed=false;
        for(int i=1;i<ret.length();i++)
            if(!smaller(ret[i-1],ret[i]))
            {
                qDebug()<<ret[i-1]<<ret[i]<<smaller(ret[i-1],ret[i]);
                ret.swap(i-1,i);
                changed=true;
                continue;
            }
    }
    while(changed);
    qDebug()<<ret;
    return ret;
}

QList<QPair<QString,int> > Schematic::getInputList()
{
    QString blk="IN";
    QList<QPair<QString,int> > ret;
    for(int i=0;i<blocks.length();i++)
        if(blocks[i]->name==blk)
        {
            QPoint pp=blocks[i]->pins.first().pos();
            int label=-1;
            for(int i=0;i<impulseLabels.length();i++)
                if(impulseLabels[i]->pos==pp)
                    label=i;
            if(label>=0)
            {
                QString name=impulseLabels[label]->name;
                ret.append(QPair<QString,int>(name,i));
            }
        }
    return ret;
}

QList<QPair<QString, int> > Schematic::getOutputList()
{
    QString blk="OUT";
    QList<QPair<QString,int> > ret;
    for(int i=0;i<blocks.length();i++)
        if(blocks[i]->name==blk)
        {
            QPoint pp=blocks[i]->pins.first().pos();
            int label=-1;
            for(int i=0;i<impulseLabels.length();i++)
                if(impulseLabels[i]->pos==pp)
                    label=i;
            if(label>=0)
            {
                QString name=impulseLabels[label]->name;
                ret.append(QPair<QString,int>(name,i));
            }
        }
    return ret;
}

bool Schematic::simulate(QList<Schematic*> stack)
{
    for(auto block:blocks)
        block->simulate();
    bool error=false;
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
                blocks[i]->pins[j].state=state;
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
                    if(isImpulseLabel(cur))
                    {
                        ImpulseLabel* label=(ImpulseLabel*)cur;
                        label->state=state;
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
                            blocks[k]->pins[l].state=2;
                            blocks[i]->pins[j].state=2;
                            errorStr="Zwei Ausgänge";
                            return false;
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
        {
            errorStr="Offene Eingänge";
            return false;
        }
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
    return !error;
}
