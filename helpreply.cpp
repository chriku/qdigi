#include "helpreply.h"
#include "settings.h"
#include "blocklist.h"

HelpReply::HelpReply(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *outgoingData,QObject* parent):
    QNetworkReply(parent)
{
    qDebug()<<op<<req.url();
    QString file=req.url().toString();
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
        setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html; charset=UTF-8"));
        setError(QNetworkReply::NoError,"NO ERROR");
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
            setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html; charset=UTF-8"));
            setError(QNetworkReply::NoError,"NO ERROR");
        }
        else
        {
            setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain; charset=UTF-8"));
            setError(QNetworkReply::InternalServerError,"INTERNAL ERROR");
            data="INTERNAL ERROR";
        }
    }
    else
    {
        setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain; charset=UTF-8"));
        setError(QNetworkReply::ContentNotFoundError	,"NOT FOUND");
        data="Not Found";
    }
    data.replace("__HEAD__","");
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(data.size()));
    QTimer::singleShot(100,this,SLOT(timeout()));
    open(QIODevice::ReadOnly);
}

void HelpReply::abort()
{
}

qint64 HelpReply::bytesAvailable() const
{
    int av=data.length()+ QIODevice::bytesAvailable();
    qDebug()<<"AVAILABLE"<<av;
    return av;
}

bool HelpReply::isSequential() const
{
    return true;
}

qint64 HelpReply::readData(char *rdata, qint64 maxSize)
{
    qDebug()<<"READ"<<maxSize<<data;
    if(maxSize>=data.length())
    {
        memcpy(rdata,data.data(),data.length());
        int len=data.length();
        data.clear();
        return len;
    }
    else
    {
        memcpy(rdata,data.data(),maxSize);
        data.remove(0,maxSize);
        return maxSize;
    }
}

void HelpReply::timeout()
{
    qDebug()<<"TIMEOUT";
    emit readyRead();
    emit finished();
}
