#include "updater.h"
#include <QMessageBox>
#include <QBitmap>
#include <QDir>
#include <QApplication>
#include "settings.h"

Updater::Updater(QObject *parent) : QObject(parent),
    screen(QPixmap(":/splash.png"))
{
    screen.setMask(QBitmap(":/mask.png"));
    screen.show();
}

void Updater::update()
{
    if(Settings::final()->license().isEmpty())
        return;
    QFile rfile(QApplication::applicationFilePath()+".old");
    if(rfile.exists())
        rfile.remove();
    screen.showMessage("Auf Updates prüfen...",Qt::AlignCenter);
#ifdef Q_OS_LINUX
    QString platform="linux";
#endif
#ifdef Q_OS_WIN32
    QString platform="windows";
#endif
    QNetworkRequest req(QUrl("https://talstrasse.hp-lichtblick.de/qdigi/update?platform="+platform));
    req.setRawHeader("LICENSE",Settings::final()->license().toUtf8());
    QNetworkReply*rep=manager.get(req);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    if(rep->error()==QNetworkReply::NoError)
    {
        QByteArray resp=rep->readAll();
        //qDebug()<<resp;
        QJsonParseError error;
        QJsonDocument doc=QJsonDocument::fromJson(resp,&error);
        if(error.error==error.NoError)
        {
            QJsonObject root=doc.object();
            QString nhash=root["app"].toString();
            QString fname=QApplication::applicationFilePath();
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
                        if(QMessageBox::information(NULL,"Update Installieren","Neues Update Installieren?",QMessageBox::Ok,QMessageBox::Cancel)==QMessageBox::Ok)
                        {
                            screen.showMessage("Herunterladen...",Qt::AlignCenter);
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
                                QMessageBox::information(NULL,"Update fetig","Update Fertig, bitte nocheinmal starten.");
                                exit(0);
                            }
                        }
            }
            QJsonArray files=root["files"].toArray();
            QStringList requestFiles;
            for(int i=0;i<files.size();i++)
            {
                QJsonObject curFile=files[i].toObject();
                QCryptographicHash hash(QCryptographicHash::Sha512);
                QStringList parts=curFile["name"].toString().split('/');
                QDir dir(Settings::final()->applicationDir());
                for(int i=0;i<parts.length()-1;i++)
                {
                    if(!dir.entryList().contains(parts[i]))
                        dir.mkdir(parts[i]);
                    dir.cd(parts[i]);
                }
                QString name=dir.absoluteFilePath(parts.last());
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
            if(requestFiles.length()==0)
                return;
            screen.showMessage(""+QString::number(requestFiles.length())+" Dateien werden heruntergeladen",Qt::AlignCenter);
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
                    QStringList parts=requestFiles[i].split('/');
                    QDir dir(Settings::final()->applicationDir());
                    for(int i=0;i<parts.length()-1;i++)
                    {
                        if(!dir.entryList().contains(parts[i]))
                            dir.mkdir(parts[i]);
                        dir.cd(parts[i]);
                    }
                    QString name=dir.absoluteFilePath(parts.last());
                    QFile file(name);
                    file.open(QFile::WriteOnly|QFile::Truncate);
                    file.write(data);
                    file.flush();
                    file.close();
                }
                else
                    qDebug()<<rep->errorString();
                screen.showMessage(QString::number(i+1)+" von "+QString::number(requestFiles.length())+" Dateien sind heruntergeladen",Qt::AlignCenter);
            }
        }else
            screen.showMessage("Internal Error",Qt::AlignCenter);
    }else if(rep->error()==QNetworkReply::AuthenticationRequiredError){
        Settings::final()->setLicense("");
        QMessageBox::warning(NULL,"QDigi","Falscher Lizenzschlüssel\nBitte beim nächsten Start Gültigen Schlüssel eingeben");
        exit(0);
    }
    else
    {
        qDebug()<<rep->errorString();
        screen.showMessage("Kein Netzwerk?",Qt::AlignCenter);
    }
    QTimer::singleShot(1000,&loop,SLOT(quit()));
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
