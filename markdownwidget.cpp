#include "markdownwidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPicture>
#include <QPainter>

MarkDownWidget::MarkDownWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void MarkDownWidget::setText(QString text)
{
    update();
    this->text=text;
}

void MarkDownWidget::resize(int w, int h)
{
    update();
    //render();
}
enum TYPE {
    TYPE_EMPTY,
    TYPE_TEXT,
    TYPE_HEADLINE_1,
    TYPE_HEADLINE_2,
    TYPE_HEADLINE_3,
    TYPE_HEADLINE_4,
    TYPE_HEADLINE_5,
    TYPE_HEADLINE_6,
    TYPE_HR,
    TYPE_OL,
    TYPE_UL
};
struct line_t {
    TYPE type;
    QString content;
};

QPixmap MarkDownWidget::render()
{
    QStringList lines=text.split('\n');
    QList<line_t> lineTypes;
    for(int i=0;i<lines.length();i++)
    {
        line_t out;
        out.type=TYPE_TEXT;
        QString in=lines[i];
        out.content=in;
        if((!in.contains(QRegExp("[^=]")))&&(in.length()>3))
        {
            if((lineTypes.length()>0)&&(lineTypes.last().type==TYPE_TEXT))
            {
                lineTypes.last().type=TYPE_HEADLINE_1;
            }
        }
        else if((!in.contains(QRegExp("[^-]")))&&(in.length()>3))
        {
            if((lineTypes.length()>0)&&(lineTypes.last().type==TYPE_TEXT))
            {
                lineTypes.last().type=TYPE_HEADLINE_2;
            }
        }
        else if(in.startsWith("######"))
        {
            out.content.remove(0,6);
            out.type=TYPE_HEADLINE_6;
            lineTypes.append(out);
        }
        else if(in.startsWith("#####"))
        {
            out.content.remove(0,5);
            out.type=TYPE_HEADLINE_5;
            lineTypes.append(out);
        }
        else if(in.startsWith("####"))
        {
            out.content.remove(0,4);
            out.type=TYPE_HEADLINE_4;
            lineTypes.append(out);
        }
        else if(in.startsWith("###"))
        {
            out.content.remove(0,3);
            out.type=TYPE_HEADLINE_3;
            lineTypes.append(out);
        }
        else if(in.startsWith("##"))
        {
            out.content.remove(0,2);
            out.type=TYPE_HEADLINE_2;
            lineTypes.append(out);
        }
        else if(in.startsWith("#"))
        {
            out.content.remove(0,1);
            out.type=TYPE_HEADLINE_1;
            lineTypes.append(out);
        }
        else if(in.startsWith("  "))
        {
            lineTypes.last().content.append(in);
        }
        else if((in=="---")||(in=="___")||(in=="***"))
        {
            out.content="";
            out.type=TYPE_HR;
            lineTypes.append(out);
        }
        else if(in.length()==0)
        {
            out.type=TYPE_EMPTY;
            lineTypes.append(out);
        }
        else if(in.startsWith("- "))
        {
            out.content.remove(0,2);
            out.type=TYPE_UL;
            lineTypes.append(out);
        }
        else if(in.startsWith("  "))
        {
            out.content.remove(0,2);
            out.type=TYPE_UL;
            lineTypes.append(out);
        }
        else if(in.contains(QRegExp("^[0-9]*\\.")))
        {
            out.content.remove(0,out.content.indexOf(".")+1);
            out.type=TYPE_OL;
            lineTypes.append(out);
        }
        else
        {
            lineTypes.append(out);
        }
    }
    int w=width();
    int width=w;
    QList<QPixmap> paragraphs;
    QMap<int,QList<QPair<QRect,QString> > > linkP;
    for(int i=0;i<lineTypes.length();i++)
    {
        switch(lineTypes[i].type)
        {
        case TYPE_EMPTY:
        {
            paragraphs.append(QPixmap());
        }
            break;
        case TYPE_TEXT:
        {
            QList<QPair<QRect,QString> > linkPi;
            paragraphs.append(renderText(lineTypes[i].content,width,&(linkPi)));
            linkP.insert(i,linkPi);
        }
            break;
        case TYPE_HEADLINE_1:
        {
            paragraphs.append(renderHeadline(lineTypes[i].content,width,1));
        }
            break;
        case TYPE_HEADLINE_2:
        {
            paragraphs.append(renderHeadline(lineTypes[i].content,width,2));
        }
            break;
        case TYPE_HEADLINE_3:
        {
            paragraphs.append(renderHeadline(lineTypes[i].content,width,3));
        }
            break;
        case TYPE_HEADLINE_4:
        {
            paragraphs.append(renderHeadline(lineTypes[i].content,width,4));
        }
            break;
        case TYPE_HEADLINE_5:
        {
            paragraphs.append(renderHeadline(lineTypes[i].content,width,5));
        }
            break;
        case TYPE_HEADLINE_6:
        {
            paragraphs.append(renderHeadline(lineTypes[i].content,width,6));
        }
            break;
        case TYPE_HR:
        {
            paragraphs.append(renderLine(width));
        }
            break;
        case TYPE_OL:
        {
            int ni=i;
            QList<line_t> list;
            list.append(lineTypes[i]);
            for(int j=i+1;j<lineTypes.length();j++)
            {
                if(lineTypes[j].type==TYPE_OL)
                {
                    list.append(lineTypes[j]);
                }
                else
                {
                    ni=j;
                    j=lineTypes.length();
                }
            }
            QStringList slist;
            for(int i=0;i<list.length();i++)
                slist.append(list[i].content);
            QList<QPair<QRect,QString> > linkPi;
            paragraphs.append(renderOL(slist,width,&linkPi));
            linkP.insert(i,linkPi);
            i=ni;
        }
            break;
        case TYPE_UL:
        {
            int ni=i;
            QList<line_t> list;
            list.append(lineTypes[i]);
            for(int j=i+1;j<lineTypes.length();j++)
            {
                if(lineTypes[j].type==TYPE_UL)
                {
                    list.append(lineTypes[j]);
                }
                else
                {
                    ni=j;
                    j=lineTypes.length();
                }
            }
            QStringList slist;
            for(int i=0;i<list.length();i++)
                slist.append(list[i].content);
            QList<QPair<QRect,QString> > linkPi;
            paragraphs.append(renderUL(slist,width,&linkPi));
            linkP.insert(i,linkPi);
            i=ni;
        }
            break;
        default:
        {
            paragraphs.append(QPixmap());
        }
            break;
        }
    }
    int y=0;
    for(int i=0;i<paragraphs.length();i++)
    {
        y+=paragraphs[i].height()+5;
    }
    QPixmap ret(width,y-5);
    setMinimumHeight(ret.height());
    ret.fill(Qt::white);
    QPainter painter(&ret);
    y=0;
    for(int i=0;i<paragraphs.length();i++)
    {
        if(linkP.contains(i))
        {
            QList<QPair<QRect,QString> > linkPi=linkP[i];
            for(int i=0;i<linkPi.length();i++)
            {
                QRect r=linkPi[i].first;
                r.translate(QPoint(0,y));
                links.removeAll(QPair<QRect,QString>(r,linkPi[i].second));
                links.append(QPair<QRect,QString>(r,linkPi[i].second));
            }
        }
        painter.drawPixmap(0,y,paragraphs[i]);
        y+=paragraphs[i].height()+5;
    }
    return ret;
}

void MarkDownWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,render());
    painter.end();
}

QPixmap MarkDownWidget::renderHeadline(QString text, int width, int size)
{
    int sizes[6]={40,32,28,24,20,16};
    QPixmap ret(width,100);
    ret.fill(Qt::white);
    QPainter painter(&ret);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial",sizes[size-1]));
    QRect br;
    painter.drawText(0,0,width,100,Qt::AlignHCenter|Qt::AlignTop,text,&br);
    painter.end();
    return ret.copy(0,0,width,br.height());
}

QPixmap MarkDownWidget::renderLine(int width)
{
    QPixmap ret(width,1);
    ret.fill(Qt::white);
    QPainter painter(&ret);
    painter.setPen(Qt::gray);
    painter.drawLine(0,0,width,0);
    return ret;
}

QPixmap MarkDownWidget::renderUL(QStringList items,int width, QList<QPair<QRect, QString> > *linkP)
{
    QPixmap ret(width,items.length()*100);
    ret.fill(Qt::white);
    QPainter painter(&ret);
    int y=0;
    for(int i=0;i<items.length();i++)
    {
        QList<QPair<QRect, QString> > linkPi;
        QPixmap cur=renderText(items[i],width-20,&linkPi);
        for(int j=0;j<linkPi.length();j++)
        {
            linkPi[j].first.translate(20,y);
            linkP->append(linkPi[j]);
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawRect(7,y+3,6,6);
        painter.drawPixmap(20,y,cur);
        y+=cur.height()+5;
    }
    return ret.copy(0,0,width,y-5);
}

QPixmap MarkDownWidget::renderOL(QStringList items,int width, QList<QPair<QRect, QString> > *linkP)
{
    QPixmap ret(width,items.length()*100);
    ret.fill(Qt::white);
    QPainter painter(&ret);
    int y=0;
    for(int i=0;i<items.length();i++)
    {
        QList<QPair<QRect, QString> > linkPi;
        QPixmap cur=renderText(items[i],width-20,&linkPi);
        for(int j=0;j<linkPi.length();j++)
        {
            linkPi[j].first.translate(20,y);
            linkP->append(linkPi[j]);
        }
        painter.setPen(Qt::black);
        painter.drawText(0,y,20,cur.height(),Qt::AlignCenter,QString::number(i+1)+".");
        painter.drawPixmap(20,y,cur);
        y+=cur.height()+5;
    }
    return ret.copy(0,0,width,y-5);
}

struct word_t {
    QString text;
    bool bold;
    bool italic;
    int link;
    QString linkT;
    QPixmap rendered;
};

QPixmap MarkDownWidget::renderText(QString text, int width, QList<QPair<QRect, QString> > *linkP)
{
    QList<word_t> words;
    int pos=0;
    int link=-1;
    bool bold=false;
    bool italic=false;
    QString cur="";
    QList<QString> links;
    text.append(" ");
    for(int i=0;i<text.length();i++)
    {
        word_t w;
        w.text=cur;
        w.bold=bold;
        w.italic=italic;
        w.link=link;
        bool add=false;
        QChar c=text.at(i);
        if(c=='[')
        {
            if(cur.length()>0)
                words.append(w);
            cur="";
            link=links.length();
            add=true;
        }
        else if(c==']')
        {
            if(cur.length()>0)
                words.append(w);
            cur="";
            QString l=text.mid(text.indexOf("(",i)+1);
            l=l.left(l.indexOf(")"));
            links.append(l);
            i=text.indexOf(")",i);
            link=-1;
            add=true;
        }
        else if(c=='*')
        {
            if(cur.length()>0)
                words.append(w);
            cur="";
            if(text.at(i+1)=='*')
            {
                bold=!bold;
                i++;
            }
            else
                italic=!italic;
            add=true;
        }
        else if(c=='_')
        {
            if(cur.length()>0)
                words.append(w);
            cur="";
            if(text.at(i+1)=='_')
            {
                bold=!bold;
                i++;
            }
            else
                italic=!italic;
            add=true;
        }
        else if(c==' ')
        {
            add=true;
        }
        else
            cur.append(c);
        if(add)
        {
            word_t w;
            w.text=cur;
            w.bold=bold;
            w.italic=italic;
            w.link=link;
            if(cur.length()>0)
                words.append(w);
            cur="";
        }
    }
    for(int i=0;i<words.length();i++)
    {
        if(words[i].link>=0)
        {
            words[i].linkT=links[words[i].link];
        }
    }
    int fh=12;
    for(int i=0;i<words.length();i++)
    {
        QColor color=Qt::black;
        QFont font;
        font.setPixelSize(fh);
        font.setBold(words[i].bold);
        font.setItalic(words[i].italic);
        font.setUnderline(false);
        if(words[i].link>=0)
        {
            font.setUnderline(true);
            color=Qt::blue;
        }
        QPixmap pm(width,fh+5);
        pm.fill(Qt::transparent);
        QPainter painter(&pm);
        painter.setFont(font);
        painter.setPen(color);
        QRect br;
        painter.drawText(0,0,1000,fh+5,Qt::AlignVCenter|Qt::AlignLeft,words[i].text,&br);
        words[i].rendered=pm.copy(0,0,br.width(),fh+5);
    }
    QList<QList<QPixmap> > lines;
    lines.append(QList<QPixmap>());
    int px=0;
    for(int i=0;i<words.length();i++)
    {
        if((px+words[i].rendered.width())>=width)
        {
            lines.append(QList<QPixmap>());
            px=0;
        }
        if(words[i].link>=0)
        {
            linkP->append(QPair<QRect,QString>(QRect(px,(lines.length()-1)*(fh+5),words[i].rendered.width(),words[i].rendered.height()),words[i].linkT));
        }
        lines.last().append(words[i].rendered);
        px+=words[i].rendered.width()+5;
    }
    QPixmap ret(width,lines.length()*(fh+5));
    ret.fill(Qt::white);
    QPainter painter(&ret);
    int py=0;
    for(int i=0;i<lines.length();i++)
    {
        int px=0;
        for(int j=0;j<lines[i].length();j++)
        {
            painter.drawPixmap(px,py,lines[i][j]);
            px+=lines[i][j].width()+5;
        }
        py+=fh+5;
    }
    return ret;
}

void MarkDownWidget::mouseMoveEvent(QMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    for(auto link:links)
    {
        if(link.first.contains(event->pos()))
        {
            setCursor(Qt::PointingHandCursor);
        }
    }
}

void MarkDownWidget::mousePressEvent(QMouseEvent *event)
{
    for(auto link:links)
    {
        if(link.first.contains(event->pos()))
        {
            qDebug()<<"LINK"<<link.second;
            emit linkClicked(link.second);
        }
    }
}

