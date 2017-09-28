#include "blocklist.h"
#include <QInputDialog>
#include <QMessageBox>
#include <iostream>
#include "mainwindow.h"
#include <QApplication>
#include "updater.h"
#include "settings.h"
#include "helpdialog.h"
#include "extension/extension.h"

QNetworkAccessManager manager;

#ifdef Q_OS_WIN
#include <windows.h>
#include <winreg.h>
#endif
//QString logs;
/*void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;
    switch (type) {
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
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
    //std::cout<<outFile.errorString().toUtf8().data()<<std::endl;
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();
}*/
int main(int argc, char *argv[])
{
    BlockList bl;
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    /*QFile outFile("out.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    outFile.write("Start Logging\r\n");
    outFile.close();*/
    QApplication a(argc, argv);
    //qInstallMessageHandler(myMessageHandler);
    QDir dir("/home/christian/qdigi/extensions");
    for(auto file:dir.entryList())
        Extension* e=new Extension(dir.absoluteFilePath(file));
    MainWindow w;
    w.show();
    a.exec();
}
