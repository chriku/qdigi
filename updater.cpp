#include "updater.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include "blocklist.h"
#include <QBitmap>
#include <QDir>
#include <QApplication>
#include "settings.h"
#include <QInputDialog>
#include "items/block.h"

extern QNetworkAccessManager manager;
QNetworkReply*grep;
Updater::Updater(QObject *parent) : QObject(parent)
{
    startUpdate();
    authCount=0;
}

void Updater::update()
{
    QEventLoop loop;
    if(grep->error()==QNetworkReply::NoError)
    {
        QByteArray resp=grep->readAll();
        qDebug()<<"Update data" <<resp;
        QJsonParseError error;
        QJsonDocument doc=QJsonDocument::fromJson(resp,&error);
        if(error.error==error.NoError)
        {
            qDebug()<<"NO ERROR";
            QJsonObject root=doc.object();
            QJsonArray rm=root["rm"].toArray();
            for(int i=0;i<rm.size();i++)
            {
                QString cur=toPath(rm.at(i).toString());
                QFile file(cur);
                if(file.exists())
                    file.remove();
            }
            qDebug()<<"RM"<<root["rm"];
#ifdef Q_OS_WIN
            QString nhash=root["wapp"].toString();
#else
            QString nhash=root["lapp"].toString();
#endif
            QString fname=QApplication::applicationFilePath();
            bool updateExe=false;
            QCryptographicHash hash(QCryptographicHash::Sha512);
            QFile file(fname);
            file.open(QFile::ReadOnly);
            hash.addData(&file);
            file.close();
            QString chash=hash.result().toHex();
            qDebug()<<"EXE"<<chash<<nhash;
            if(chash.length()==nhash.length())
#ifdef Q_OS_LINUX
                if(Settings::final()->username()!="admin")
#endif
                    if(chash!=nhash)
                    {
                        QSystemTrayIcon *icon=new QSystemTrayIcon;
                        icon->setIcon(QIcon(":/icon.png"));
                        icon->setContextMenu(new QMenu());
                        icon->show();
#ifdef Q_OS_WIN
                        icon->showMessage("QDigi","qdigi.exe aktualisieren");
#else
                        icon->showMessage("QDigi","qdigi aktualisieren");
#endif
                        QTimer::singleShot(10000,[icon](){
                            icon->deleteLater();
                        });
#ifdef Q_PROCESSOR_X86_64
                        updateExe=true;
#else
#ifndef Q_OS_LINUX
                        updateExe=true;
#endif
#endif
                    }
            QJsonArray files=root["files"].toArray();
            QStringList requestFiles;
            for(int i=0;i<files.size();i++)
            {
                QJsonObject curFile=files[i].toObject();
                qDebug()<<curFile;
                QCryptographicHash hash(QCryptographicHash::Sha512);
                QString name=toPath(curFile["name"].toString());
                QFile file(name);
                if(file.exists())
                {
                    file.open(QFile::ReadOnly);
                    hash.addData(&file);
                    file.close();
                    QString chash=hash.result().toHex();
                    qDebug()<<"HASH"<<curFile["name"].toString()<<chash<<curFile["hash"].toString();
                    if(curFile["hash"].toString()!=chash)
                        requestFiles.append(curFile["name"].toString());
                }
                else
                {
                    requestFiles.append(curFile["name"].toString());
                    QSystemTrayIcon *icon=new QSystemTrayIcon;
                    icon->setIcon(QIcon(":/icon.png"));
                    icon->setContextMenu(new QMenu());
                    icon->show();
                    icon->showMessage("QDigi",requestFiles.last()+" aktualisieren",QSystemTrayIcon::Information);
                    QTimer::singleShot(10000,[icon](){
                        icon->deleteLater();
                    });
                }
            }
            if(updateExe||(requestFiles.length()>0))
            {
                if(updateExe)
                {
                    if(QMessageBox::information(NULL,"Update Installieren","Neues Update Installieren?",QMessageBox::Ok,QMessageBox::Cancel)==QMessageBox::Ok)
                    {
#ifdef Q_OS_WIN
                        QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/downloads/"+root["exeName"].toString()+".exe"));
#else
                        QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/downloads/"+root["exeName"].toString()));
#endif
                        req.setRawHeader("token",Settings::final()->token().toUtf8());
                        QNetworkReply* rep=manager.get(req);
                        connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
                        loop.exec();
                        if(rep->error()==QNetworkReply::NoError)
                        {
                            QByteArray newExe=rep->readAll();
#ifdef Q_OS_WIN
                            QString path=QDir(QApplication::applicationDirPath()).absoluteFilePath("qdigi.exe");
#else
                            QString path=QDir(QApplication::applicationDirPath()).absoluteFilePath("qdigi");
#endif
                            QDir dir(QApplication::applicationDirPath());
                            dir.rename(QApplication::applicationFilePath(),QApplication::applicationFilePath()+".old");
                            QFile file(path);
                            file.open(QFile::WriteOnly|QFile::Truncate);
                            file.write(newExe);
                            file.close();
                            file.setPermissions(file.permissions()|QFile::ExeUser|QFile::ExeGroup|QFile::ExeOther|QFile::ExeOwner);
                        }
                    }
                    else
                        updateExe=false;
                }
                for(int i=0;i<requestFiles.length();i++)
                {
                    QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/downloads/"+requestFiles[i]));
                    req.setRawHeader("token",Settings::final()->token().toUtf8());
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
                        qDebug()<<"BLKNAME"<<name;
                        if(Block::usedBlocks.contains(name))
                        {
                            qDebug()<<Block::usedBlocks[name];
                            for(auto block:Block::usedBlocks[name])
                            {
                                qDebug()<<block->name;
                                block->refresh();
                            }
                        }
                        QSystemTrayIcon *icon=new QSystemTrayIcon;
                        icon->setIcon(QIcon(":/icon.png"));
                        icon->setContextMenu(new QMenu());
                        icon->show();
                        icon->showMessage("QDigi",requestFiles[i]+" aktualisiert");
                        QTimer::singleShot(10000,[icon](){
                            icon->deleteLater();
                        });
                    }
                    else
                        qDebug()<<rep->errorString();
                }
                QMessageBox::information(NULL,"Update fertig","Update Fertig, bitte noch einmal starten.");
                exit(0);
            }else
            {
                QSystemTrayIcon *icon=new QSystemTrayIcon;
                icon->setIcon(QIcon(":/icon.png"));
                icon->setContextMenu(new QMenu());
                icon->show();
                icon->showMessage("QDigi","Keine Updates");
                QTimer::singleShot(10000,[icon](){
                    icon->deleteLater();
                });
                qDebug()<<"NO NEW UPDATES";
                return;
            }
        }
    }else if(grep->error()==QNetworkReply::AuthenticationRequiredError){
        Settings::final()->resetToken();
        //QMessageBox::warning(NULL,"QDigi","Falscher Lizenzschlüssel\nBitte beim nächsten Start Gültigen Schlüssel eingeben");
        //exit(0);
    }
    else
    {
        qDebug()<<grep->errorString();
    }
    qDebug()<<"UPDATE FINISHED";
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
    if(authCount==0)
    {
        if(Settings::final()->saveFile->value("proxy_user").isValid()&&Settings::final()->saveFile->value("proxy_pass").isValid())
        {
            auth->setUser(Settings::final()->saveFile->value("proxy_user").toString());
            auth->setPassword(Settings::final()->saveFile->value("proxy_pass").toString());
        }
        authCount=1;
    }
    bool ok;
    QString username=QInputDialog::getText(NULL,"Login","Username",QLineEdit::Normal,"",&ok);
    if(!ok)
    {
        authCount=2;
        return;
    }
    QString password=QInputDialog::getText(NULL,"Login","Passwort",QLineEdit::Password,"",&ok);
    if(!ok)
    {
        authCount=2;
        return;
    }
    Settings::final()->saveFile->setValue("proxy_user",username);
    Settings::final()->saveFile->setValue("proxy_pass",password);
    auth->setUser(username);
    auth->setPassword(password);
}

void Updater::startUpdate()
{
    QString platform="";
#ifdef Q_OS_LINUX
    platform="linux";
#endif
#ifdef Q_OS_WIN32
    platform="windows";
#endif
    qDebug()<<"Starting Update";
    QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/update.cgi?platform="+platform));
    req.setRawHeader("token",Settings::final()->token().toUtf8());
    grep=manager.get(req);
    //if(Settings::final()->license().isEmpty())
    //    return;
    QFile rfile(QApplication::applicationFilePath()+".old");
    if(rfile.exists())
        rfile.remove();
    connect(grep,SIGNAL(finished()),this,SLOT(update()));
    connect(&manager,SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),this,SLOT(authenticationRequired(QNetworkProxy,QAuthenticator*)));
}
