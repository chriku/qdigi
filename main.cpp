#include <QInputDialog>
#include <QMessageBox>
#include <iostream>
#include "mainwindow.h"
#include <QApplication>
#include "updater.h"
#include "kvdiagram.h"
#include "settings.h"
#ifdef Q_OS_WIN
#include <windows.h>
#include <winreg.h>
#endif
QString logs;
void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }
    std::cout<<txt.toStdString() << std::endl;
    QFile outFile("out.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    std::cout<<outFile.errorString().toUtf8().data()<<std::endl;
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();
}
int main(int argc, char *argv[])
{
    QFile outFile("out.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    outFile.write("Start Logging\r\n");
    outFile.close();
    QApplication a(argc, argv);
    //qInstallMessageHandler(myMessageHandler);
    if(Settings::final()->license().isEmpty())
    {
        QInputDialog dialog;
        dialog.setInputMode(QInputDialog::TextInput);
        dialog.setWindowTitle("QDigi");
        dialog.setLabelText("Lizenzschlüssel eingeben");
        dialog.exec();
        QString lic=dialog.textValue();
        if(lic.isEmpty())
            return 0;
        Settings::final()->setLicense(lic);
    }
    QString lic=Settings::final()->license();
    if(lic.left(lic.indexOf(":"))=="1")
        if(QMessageBox::question(NULL,"QDigi","KV-Map")==QMessageBox::Yes)
        {
            KVDiagram diagram;
            diagram.exec();
            exit(0);
        }
    Updater updater;
    updater.update();
    MainWindow w;
    updater.screen.finish(&w);
    w.show();
    /*    long unsigned int len=1024*1024;
    char value[len];
    for(int i=0;i<len;i++)
        value[i]=0;
    HKEY hKey;
    if(0!=RegOpenKeyEx(HKEY_CLASSES_ROOT,TEXT(".qdigi\\shell\\Open"),NULL,REG_BINARY,&hKey))
    else
    {
    qDebug()<<RegGetValue(hKey,TEXT("command"),TEXT(""),RRF_RT_ANY,NULL,value,&len);
    QByteArray val(value,len);
    while(val.at(val.length()-1)==0)
        val=val.left(val.length()-1);
    qDebug()<<val;
    }
        #*/
    a.exec();
    QNetworkAccessManager manager;
    QNetworkRequest req;
    QUrlQuery query;
    if(1==1)
    {
        QFile outFile("out.txt");
        outFile.open(QIODevice::ReadOnly);
        std::cout<<outFile.errorString().toStdString()<<std::endl;
        query.addQueryItem("log",outFile.readAll().toBase64());
        query.addQueryItem("license",Settings::final()->license());
        QUrl url("https://talstrasse.hp-lichtblick.de/q/error.cgi");
        req.setUrl(url);
        QNetworkReply* rep=manager.post(req,query.toString().toUtf8());
        QEventLoop loop;
        QObject::connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
        loop.exec();
        std::cerr<<rep->errorString().toStdString()<<std::endl<<rep->readAll().data()<<std::endl;
    }
}
