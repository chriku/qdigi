#include "updater.h"
#include <QMessageBox>
#include <QBitmap>
#include <QDir>
#include <QApplication>
#include "settings.h"
#include <QInputDialog>

extern QNetworkAccessManager manager;
QNetworkReply*grep;
Updater::Updater(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_LINUX
    QString platform="linux";
#endif
#ifdef Q_OS_WIN32
    QString platform="windows";
#endif
    QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/update?platform="+platform));
    req.setRawHeader("LICENSE",Settings::final()->license().toUtf8());
    grep=manager.get(req);
    if(Settings::final()->license().isEmpty())
        return;
    QFile rfile(QApplication::applicationFilePath()+".old");
    if(rfile.exists())
        rfile.remove();
    connect(grep,SIGNAL(finished()),this,SLOT(update()));
    connect(&manager,SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),this,SLOT(authenticationRequired(QNetworkProxy,QAuthenticator*)));
}

void Updater::update()
{
    QEventLoop loop;
    if(grep->error()==QNetworkReply::NoError)
    {
        QByteArray resp=grep->readAll();
        //qDebug()<<resp;
        QJsonParseError error;
        QJsonDocument doc=QJsonDocument::fromJson(resp,&error);
        if(error.error==error.NoError)
        {
            QJsonObject root=doc.object();
            QJsonArray rm=root["rm"].toArray();
            for(int i=0;i<rm.size();i++)
            {
                QString cur=toPath(rm.at(i).toString());
                QFile file(cur);
                if(file.exists())
                    file.remove();
            }
            QString nhash=root["app"].toString();
            QString fname=QApplication::applicationFilePath();
            bool updateExe=false;
            if(fname.endsWith(".exe"))
            {
                QCryptographicHash hash(QCryptographicHash::Sha512);
                QFile file(fname);
                file.open(QFile::ReadOnly);
                hash.addData(&file);
                file.close();
                QString chash=hash.result().toHex();
                if(chash.length()==nhash.length())
                    if(chash!=nhash)
                        updateExe=true;
            }
            QJsonArray files=root["files"].toArray();
            QStringList requestFiles;
            for(int i=0;i<files.size();i++)
            {
                QJsonObject curFile=files[i].toObject();
                QCryptographicHash hash(QCryptographicHash::Sha512);
                QString name=toPath(curFile["name"].toString());
                QFile file(name);
                if(file.exists())
                {
                    file.open(QFile::ReadOnly);
                    hash.addData(&file);
                    file.close();
                    QString chash=hash.result().toHex();
                    if(curFile["hash"].toString()!=chash)
                        requestFiles.append(curFile["name"].toString());
                }
                else
                    requestFiles.append(curFile["name"].toString());
            }

            if((requestFiles.length()>0)||updateExe)
            {
                if(QMessageBox::information(NULL,"Update Installieren","Neues Update Installieren?",QMessageBox::Ok,QMessageBox::Cancel)==QMessageBox::Ok)
                {
                    if(updateExe)
                    {
                        QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/q/downloads/"+root["exeName"].toString()));
                        req.setRawHeader("LICENSE",Settings::final()->license().toUtf8());
                        QNetworkReply* rep=manager.get(req);
                        connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
                        loop.exec();
                        if(rep->error()==QNetworkReply::NoError)
                        {
                            QByteArray newExe=rep->readAll();
                            QString path=QDir(QApplication::applicationDirPath()).absoluteFilePath("qdigi.exe");
                            QDir dir(QApplication::applicationDirPath());
                            dir.rename(QApplication::applicationFilePath(),QApplication::applicationFilePath()+".old");
                            QFile file(path);
                            file.open(QFile::WriteOnly|QFile::Truncate);
                            file.write(newExe);
                            file.close();
                        }
                    }
                    for(int i=0;i<requestFiles.length();i++)
                    {
                        QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/q/downloads/"+requestFiles[i]));
                        req.setRawHeader("LICENSE",Settings::final()->license().toUtf8());
                        QNetworkReply* rep=manager.get(req);
                        connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
                        loop.exec();
                        if(rep->error()==QNetworkReply::NoError)
                        {
                            QByteArray data=rep->readAll();
                            QString name=toPath(requestFiles[i]);
                            QFile file(name);
                            file.open(QFile::WriteOnly|QFile::Truncate);
                            file.write(data);
                            file.flush();
                            file.close();
                        }
                        else
                            qDebug()<<rep->errorString();
                    }
                    QMessageBox::information(NULL,"Update fetig","Update Fertig, bitte noch einmal starten.");
                    exit(0);
                }
                else
                    return;
            }else
                return;
        }
    }else if(grep->error()==QNetworkReply::AuthenticationRequiredError){
        Settings::final()->setLicense("");
        QMessageBox::warning(NULL,"QDigi","Falscher Lizenzschlüssel\nBitte beim nächsten Start Gültigen Schlüssel eingeben");
        exit(0);
    }
    else
    {
        qDebug()<<grep->errorString();
    }
    //QTimer::singleShot(1000,&loop,SLOT(quit()));
    //loop.exec();
}

void Updater::registerReg()
{
#ifdef Q_OS_WIN

    QFile file(":/qdigi.reg");
    file.open(QFile::ReadOnly);
    QString data=file.readAll();
    file.close();
    QString fp=QApplication::applicationFilePath();
    fp.replace("/","\\");
    fp.replace("\\","\\\\");
    data.replace("___EXE_PATH___",fp);
    data.replace("\r\n","\n");
    data.replace("\n","\r\n");
    file.setFileName("qdigiFile.reg");
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.write(data.toUtf8());
    file.close();
    system("qdigiFile.reg");
    file.remove();
#endif
}

QString Updater::toPath(QString in)
{
    QStringList parts=in.split('/');
    QDir dir(Settings::final()->applicationDir());
    for(int i=0;i<parts.length()-1;i++)
    {
        if(!dir.entryList().contains(parts[i]))
            dir.mkdir(parts[i]);
        dir.cd(parts[i]);
    }
    QString name=dir.absoluteFilePath(parts.last());
    return name;
}

void Updater::authenticationRequired(QNetworkProxy proxy, QAuthenticator*auth)
{
    *auth=QAuthenticator();
    bool ok;
    QString username=QInputDialog::getText(NULL,"Login","Username",QLineEdit::Normal,"",&ok);
    if(!ok)
        return;
    QString password=QInputDialog::getText(NULL,"Login","Passwort",QLineEdit::Password,"",&ok);
    if(!ok)
        return;
    auth->setUser(username);
    auth->setPassword(password);
}
