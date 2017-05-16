#include "helpmanager.h"
#include <QMessageBox>
#include <QtNetwork>
#include "helpreply.h"

HelpManager::HelpManager()
{

}

QNetworkReply* HelpManager::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
    qDebug()<<"REQ"<<req.url()<<op;
    if (req.url().scheme() != "help")
    {
        QMessageBox::warning(NULL,"ERROR","NOT SUPPORTED SCHEME \""+req.url().scheme()+"\", USE help");
        exit(1);
        return QNetworkAccessManager::createRequest(op,req,outgoingData);
    }
    if (op == GetOperation)
    {
        return new HelpReply(op,req,outgoingData);
    }
    else
    {
        QMessageBox::warning(NULL,"ERROR","UNSUPPORTED OPERATION");
        exit(1);
        return QNetworkAccessManager::createRequest(op,req,outgoingData);
    }
}
