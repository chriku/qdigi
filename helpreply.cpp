#include "helpreply.h"
#include "settings.h"
#include "blocklist.h"

HelpReply::HelpReply(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *outgoingData,QObject* parent):
    QNetworkReply(parent)
{
    qDebug()<<op<<req.url();
    if(isValid(data).isEmpty())
    {
        QString head="";
        data="<html><head>"+head.toUtf8()+"</head><body>"+data+"</body></html>";
    }
    else
    {
        data="<html><body>Invalid Tag Found</body></html>";
    }
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

QString HelpReply::isValid(QString html)
{
    QStringList allowedTags;

    return "";
}
