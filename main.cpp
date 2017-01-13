#include "mainwindow.h"
#include <QApplication>
#include "updater.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Updater updater;
    updater.update();
    MainWindow w;
    updater.screen.finish(&w);
    w.show();
    return a.exec();
}
