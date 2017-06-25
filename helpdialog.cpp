#include "helpdialog.h"
#include <QFile>
#include <QDir>
#include "settings.h"
#include "blocklist.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    ui->mainContent->hd=this;
    connect(ui->mainContent,SIGNAL(linkClicked(QString)),this,SLOT(openLink(QString)));
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::openLink(QString link)
{
    if(!((last.length()>0)&&(last.last()==link)))
        last.append(link);
    QString data=getFile(link);
    if(data.isEmpty())
        data="ERROR";
    ui->mainContent->setText(data);
}

QString HelpDialog::getFile(QString url)
{
    QString data;
    QString file=url;
    file.remove(0,7);
    QDir dir(Settings::final()->applicationDir());
    dir.cd("help");
    if(file=="blocks")
    {
        QFile file(dir.absoluteFilePath("blocks.md"));
        file.open(QFile::ReadOnly);
        data=file.readAll();
        file.close();
        QByteArray list;
        dir.cd("blocks");
        for(auto block:BlockList::blocks)
            if(block->subItem==false)
        {
            QDir dir2(QFile(((LuaBlock*)block)->fileName).fileName());
            dir2.cdUp();
            QString fn=dir2.relativeFilePath(((LuaBlock*)block)->fileName);
            fn.replace(".lua","");
            list+="- ["+block->name+"](help://blocks/"+fn+")\n";
        }
        list+="</ul>";
        data.replace("__BLOCKS__",list);
    }
    else if(file.startsWith("blocks/"))
    {
        dir.cd("blocks");
        file.remove(0,7);
        QFile filef(dir.absoluteFilePath(file+".md"));
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
            if(((LuaBlock*)b2)->fileName==search)
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
    return data;
}

void HelpDialog::on_back_clicked()
{
    if(last.length()>1)
    {
        last.removeLast();
        openLink(last.takeLast());
    }
}
