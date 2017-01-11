#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->digiView->bar=ui->statusBar;
    ui->digiView->load("test.json");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSpeichern_triggered()
{
    setWindowTitle("test.json");
    ui->digiView->save("test.json");
}

void MainWindow::on_actionEinstellungen_triggered()
{
    SettingsDialog dialog;
    dialog.exec();
}
