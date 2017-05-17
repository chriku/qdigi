#include "helpdialog.h"
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include "settings.h"
#include "helpmanager.h"
#include <QPainter>
#include "blocklist.h"

HelpDialog::HelpDialog(QWidget *parent):
    QDialog(parent)
{
}


void HelpDialog::showHelp(QString about)
{
    setHtml(getText(QUrl("help://"+about)));
}

void HelpDialog::titleChanged(QString title)
{
}

void HelpDialog::help(QString about)
{
    //dialog->show();
}

QString HelpDialog::getText(QUrl url)
{
    QString data;
    QString file=url.toString();
    file.remove(0,7);
    QDir dir(Settings::final()->applicationDir());
    dir.cd("help");
    if(file=="blocks")
    {
        QFile file(dir.absoluteFilePath("blocks.html"));
        file.open(QFile::ReadOnly);
        data=file.readAll();
        file.close();
        QByteArray list="<ul>";
        dir.cd("blocks");
        for(auto block:BlockList::blocks)
        {
            QDir dir2(QFile(block->fileName).fileName());
            dir2.cdUp();
            QString fn=dir2.relativeFilePath(block->fileName);
            fn.replace(".lua","");
            if(QFile(dir.absoluteFilePath(fn+".html")).exists())
                list+="<li><a class=\"block\" href=\"help://blocks/"+fn+"\">"+block->name+"</a></li>";
            else
                list+="<li><span class=\"block\" href=\"help://blocks/"+fn+"\">"+block->name+"</span></li>";
        }
        list+="</ul>";
        data.replace("__BLOCKS__",list);
    }
    else if(file.startsWith("blocks/"))
    {
        dir.cd("blocks");
        file.remove(0,7);
        QFile filef(dir.absoluteFilePath(file+".html"));
        qDebug()<<file;
        filef.open(QFile::ReadOnly);
        data=filef.readAll();
        filef.close();
        QDir dir2(Settings::final()->applicationDir());
        dir2.cd("modules");
        QString search=dir2.absoluteFilePath(file+".lua");
        Block* block=NULL;
        for(auto b2:BlockList::blocks)
        {
            if(b2->fileName==search)
            {
                block=b2;
            }
        }
        if(block!=NULL)
        {
            data.replace("__BLOCK_NAME__",block->name.toUtf8());
        }
        else
        {
            data="INTERNAL ERROR";
        }
    }
    else
    {
        data="Not Found";
    }
}
