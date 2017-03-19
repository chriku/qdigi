/*#include "gdrive.h"
#include "settings.h"
#include <QtNetwork>
#include <QDesktopServices>
#include <iostream>

//QNetworkAccessManager manager;

GDrive::GDrive(QObject *parent) : QObject(parent)
{

}

void GDrive::loadToken()
{
    if(!access_token.isEmpty())
    {
        if(access_timeout>QDateTime::currentDateTimeUtc())
            return;
    }
    if(1==1)
        return;
    if(!Settings::final()->refresh_token().isEmpty())
    {
        qDebug()<<"REFRESH_TOKEN";
        QNetworkRequest req;
        QUrl url("https://www.googleapis.com/oauth2/v4/token");
        QUrlQuery query;
        query.addQueryItem("refresh_token",Settings::final()->refresh_token());
        query.addQueryItem("client_id",Settings::final()->oauth_id);
        query.addQueryItem("client_secret",Settings::final()->oauth_key);
        query.addQueryItem("grant_type","refresh_token");
        req.setUrl(url);
        QNetworkReply* rep=manager.post(req,query.toString().toUtf8());
        QEventLoop loop;
        connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
        loop.exec();
        QJsonObject obj=QJsonDocument::fromJson(rep->readAll()).object();
        access_token=obj["access_token"].toString();
        access_timeout=QDateTime::currentDateTimeUtc().addSecs(obj["expires_in"].toInt()/2);
    }
    if(access_token.isEmpty())
    {
        QTcpServer server;
        server.listen(QHostAddress::Any,1024);
        QNetworkRequest req;
        QUrl url("https://accounts.google.com/o/oauth2/v2/auth");
        QUrlQuery query;
        query.addQueryItem("client_id",Settings::final()->oauth_id);
        query.addQueryItem("redirect_uri","http://127.0.0.1:"+QString::number(server.serverPort()));
        query.addQueryItem("response_type","code");
        query.addQueryItem("scope","https://www.googleapis.com/auth/drive");
        url.setQuery(query);
        qDebug()<<url;
        qDebug()<<QDesktopServices::openUrl(url);
        QEventLoop loop;
        connect(&server,SIGNAL(newConnection()),&loop,SLOT(quit()));
        loop.exec();
        disconnect(&server,SIGNAL(newConnection()),&loop,SLOT(quit()));
        QTcpSocket* client=server.nextPendingConnection();
        connect(client,SIGNAL(readyRead()),&loop,SLOT(quit()));
        loop.exec();
        QString data;
        while(true)
        {
            data+=client->read(client->bytesAvailable());
            if(data.contains("\r\n\r\n"))
                break;
            if(client->bytesAvailable()==0)
                loop.exec();
        }
        disconnect(client,SIGNAL(readyRead()),&loop,SLOT(quit()));
        client->write("HTTP/1.1 200 Ok\r\n");
        client->write("Content-Type: text/plain\r\n");
        client->write("\r\n");
        client->write("Close this Window");
        client->close();
        QStringList lines=data.split('\n');
        QString head=lines.first();
        head.replace("\r","");
        head=head.mid(head.indexOf(" ")+1);
        head=head.left(head.indexOf(" "));
        head="https://localhost"+head;
        QUrlQuery rquery(QUrl(head).query());
        if(!rquery.hasQueryItem("code"))
            return;
        QString code=rquery.queryItemValue("code");
        query.clear();
        query.addQueryItem("code",code);
        query.addQueryItem("client_id",Settings::final()->oauth_id);
        query.addQueryItem("client_secret",Settings::final()->oauth_key);
        query.addQueryItem("redirect_uri","http://127.0.0.1:"+QString::number(server.serverPort()));
        query.addQueryItem("grant_type","authorization_code");
        url=QUrl("https://www.googleapis.com/oauth2/v4/token");
        qDebug()<<url;
        req.setUrl(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        QNetworkReply* rep=manager.post(req,query.toString().toUtf8());
        connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
        loop.exec();
        disconnect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
        QJsonObject obj=QJsonDocument::fromJson(rep->readAll()).object();
        access_timeout=QDateTime::currentDateTimeUtc().addSecs(obj["expires_in"].toInt()/2);
        access_token=obj["access_token"].toString();
        Settings::final()->setRefresh_token(obj["refresh_token"].toString());
    }
}

void GDrive::uploadFile(QString name, QByteArray data)
{
    loadToken();
    qDebug()<<access_token;
    QNetworkRequest req;
    req.setUrl(QUrl("https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart"));
    req.setRawHeader("Authorization","Bearer "+access_token.toUtf8());
    QJsonObject meta;
    meta.insert("name",QJsonValue(name));
    QString first="Content-Type: application/json; charset=UTF-8\r\n\r\n"+QJsonDocument(meta).toJson();
    QString second="Content-Type: application/x-qdigi\r\n\r\n"+data;
    QByteArray boundary=QDateTime::currentDateTimeUtc().toString().toUtf8();
    boundary=QCryptographicHash::hash(boundary,QCryptographicHash::Sha512).toHex();
    while(first.contains(boundary)||second.contains(boundary))
    {
        boundary=QCryptographicHash::hash(boundary,QCryptographicHash::Sha512).toHex();
    }
    req.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/related; boundary="+boundary);
    data="--"+boundary+"\r\n";
    data+=first+"\r\n";
    data+="--"+boundary+"\r\n";
    data+=second+"\r\n";
    data+="--"+boundary+"\r\n";
    req.setHeader(QNetworkRequest::ContentLengthHeader,QString::number(data.length()));
    QNetworkReply* rep=manager.post(req,data);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    qDebug()<<rep->readAll()<<rep->error()<<rep->errorString();
}

QString GDrive::selectFile()
{
    qDebug()<<listFiles();
    return "";
}

QJsonObject GDrive::post(QString resource, QString data)
{
    loadToken();
    QNetworkRequest req;
    req.setUrl(QUrl("https://www.googleapis.com"+resource));
    req.setRawHeader("Authorization","Bearer "+access_token.toUtf8());
    QNetworkReply* rep=manager.post(req,data.toUtf8());
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    QJsonObject ret=QJsonDocument::fromJson(rep->readAll()).object();
    return ret;
}

QJsonObject GDrive::get(QString resource)
{
    loadToken();
    QNetworkRequest req;
    req.setUrl(QUrl("https://www.googleapis.com"+resource));
    req.setRawHeader("Authorization","Bearer "+access_token.toUtf8());
    QNetworkReply* rep=manager.get(req);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    QJsonObject ret=QJsonDocument::fromJson(rep->readAll()).object();
    return ret;
}

QStringList GDrive::listFiles()
{
    QStringList ret;
    QUrlQuery query;
    query.addQueryItem("q","mimeType='application/x-qdigi'");
    QJsonObject root=get("/drive/v3/files?"+query.toString());
    qDebug()<<root;
    QJsonArray files=root["files"].toArray();
    for(int i=0;i<files.size();i++)
    {
        QJsonObject file=files.at(i).toObject();
        qDebug()<<file["kind"].toString()<<file["name"].toString();
    }
    return ret;
}
*/
