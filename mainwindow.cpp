#include "updater.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QFileDialog>
#include "settings.h"
#include "blocklist.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->digiView->bar=ui->statusBar;
    ui->simulation->setChecked(Settings::final()->defaultSimu());
    on_simulation_clicked();
    //ui->digiView->load(Settings::final()->lastFile());
    setWindowTitle("NewDigi "+ui->digiView->fileName);
    connect(ui->digiView,SIGNAL(changed()),this,SLOT(changed()));
    BlockList list;
    QList<Block*> base;
    QList<Block*> coder;
    QList<Block*> other;
    for(int i=0;i<list.blocks.length();i++)
    {
        if(list.blocks[i]->category=="base")
            base.append(list.blocks[i]);
        else if(list.blocks[i]->category=="coder")
            coder.append(list.blocks[i]);
        else
            other.append(list.blocks[i]);
    }
    ui->base->blockList=base;
    ui->coder->blockList=coder;
    ui->other->blockList=other;
    QStringList args=QApplication::arguments();
    if(args.length()>1)
    ui->digiView->load(args[1]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSpeichern_triggered()
{
    if(!ui->digiView->fileName.isEmpty())
    {
    ui->digiView->save();
    setWindowTitle("NewDigi "+ui->digiView->fileName);
    }
    else
        on_actionSpeichern_Unter_triggered();
}

void MainWindow::on_actionEinstellungen_triggered()
{
    SettingsDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionSpeichern_Unter_triggered()
{
    QString fileName=QFileDialog::getSaveFileName(NULL,"Speichern unter...",QString(),"*.qdigi");
    if(!fileName.isEmpty())
    {
    ui->digiView->save(fileName);
    setWindowTitle("NewDigi "+ui->digiView->fileName);
    }
}

void MainWindow::on_action_ffnen_triggered()
{
    QString fileName=QFileDialog::getOpenFileName(NULL,"Öffnen...",QString(),"*.qdigi");
    if(!fileName.isEmpty())
    {
    ui->digiView->load(fileName);
    setWindowTitle("NewDigi "+ui->digiView->fileName);
    }
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
    setWindowTitle("NewDigi");
}

void MainWindow::changed()
{
    setWindowTitle("NewDigi * "+ui->digiView->fileName);
}

void MainWindow::on_actionZoom_R_cksetzen_triggered()
{
    ui->digiView->zoomReset();
}

void MainWindow::on_actionReinzoomen_triggered()
{
    ui->digiView->zoomIn();
}

void MainWindow::on_actionRauszoomen_triggered()
{
    ui->digiView->zoomOut();
}

void MainWindow::on_actionAls_Bild_Speichern_triggered()
{
    QString fileName=QFileDialog::getSaveFileName(NULL,"Exportieren nach...",QString(),"Bilder (*.png *.jpg *.jpeg *.bmp)");
    if(!fileName.isEmpty())
    {
    QImage img=ui->digiView->exportImage();
    img.save(fileName);
    }
}

void MainWindow::on_actionProgramm_zum_ffnen_von_qdigi_Dateien_eintragen_triggered()
{
    Updater::registerReg();
}
