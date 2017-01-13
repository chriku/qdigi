#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QFileDialog>
#include "settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->digiView->bar=ui->statusBar;
    ui->simulation->setChecked(Settings::final()->defaultSimu());
    on_simulation_clicked();
    ui->digiView->load(Settings::final()->lastFile());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSpeichern_triggered()
{
    ui->digiView->save();
}

void MainWindow::on_actionEinstellungen_triggered()
{
    SettingsDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionSpeichern_Unter_triggered()
{
    QString fileName=QFileDialog::getSaveFileName(NULL,"Speichern unter...",QString(),"*.json");
    if(!fileName.isEmpty())
    ui->digiView->save(fileName);
}

void MainWindow::on_action_ffnen_triggered()
{
    QString fileName=QFileDialog::getOpenFileName(NULL,"Öffnen...",QString(),"*.json");
    if(!fileName.isEmpty())
    ui->digiView->load(fileName);
}

void MainWindow::on_simulation_clicked()
{
    if(ui->simulation->isChecked())
        ui->digiView->timer.start();
    else
    ui->digiView->timer.stop();
}

void MainWindow::on_actionNeu_triggered()
{
    ui->digiView->load("");
}
