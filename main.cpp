#include <QInputDialog>
#include "mainwindow.h"
#include <QApplication>
#include "updater.h"
#include "kvdiagram.h"
#include "settings.h"
#ifdef Q_OS_WIN
#include <windows.h>
#include <winreg.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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
    Updater updater;
    updater.update();
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
    MainWindow w;
    updater.screen.finish(&w);
    w.show();
    return a.exec();
}
