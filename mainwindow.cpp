#include "updater.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QFileDialog>
#include "settings.h"
#include "blocklist.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->digiView->bar=ui->statusBar;
    ui->simulation->setChecked(Settings::final()->defaultSimu());
    on_simulation_clicked();
    //ui->digiView->load(Settings::final()->lastFile());
    setWindowTitle("QDigi "+ui->digiView->fileName);
    connect(ui->digiView,SIGNAL(changed()),this,SLOT(changed()));
    BlockList list;
    QList<Block*> base;
    QList<Block*> coder;
    QList<Block*> math;
    QList<Block*> multi;
    QList<Block*> other;
    for(int i=0;i<list.blocks.length();i++)
    {
        if(list.blocks[i]->category=="base")
            base.append(list.blocks[i]);
        else if(list.blocks[i]->category=="coder")
            coder.append(list.blocks[i]);
        else if(list.blocks[i]->category=="multi")
            multi.append(list.blocks[i]);
        else if(list.blocks[i]->category=="math")
            math.append(list.blocks[i]);
        else
            other.append(list.blocks[i]);
    }
    ui->base->blockList=base;
    ui->coder->blockList=coder;
    ui->other->blockList=other;
    ui->multi->blockList=multi;
    ui->math->blockList=math;
    QStringList args=QApplication::arguments();
    if(args.length()>1)
    {
        ui->digiView->load(QDir().absoluteFilePath(args[1]));
        Settings::final()->addLastChanged(QDir().absoluteFilePath(args[1]));
    }
    ui->toolBox->setCurrentWidget(ui->page_base);
    isChanged=false;
    updateLc();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSpeichern_triggered()
{
    qDebug()<<"SAVING N"<<ui->digiView->fileName;
    if(!ui->digiView->fileName.isEmpty())
    {
        if(ui->digiView->save())
        {
            qDebug()<<"SAVING GOOD";
            setWindowTitle("QDigi "+ui->digiView->fileName);
            isChanged=false;
        }
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
    qDebug()<<"SAVING A"<<ui->digiView->fileName;
    QFileDialog fd;
    fd.setDefaultSuffix(".qdigi");
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setWindowTitle("Speichern Unter...");
    fd.setNameFilter("QDigi (*.qdigi)");
    fd.exec();
    if(fd.selectedFiles().length()>0)
    {
        QString fileName=fd.selectedFiles().first();
        qDebug()<<fileName;
        if(ui->digiView->save(fileName))
        {
            qDebug()<<"SAVING GOOD";
        isChanged=false;
        setWindowTitle("QDigi "+ui->digiView->fileName);
        Settings::final()->addLastChanged(fileName);
        updateLc();
        }
    }
}

void MainWindow::on_action_ffnen_triggered()
{
    if(isChanged)
        if(askSave(false)==false)
            return;
    QString fileName=QFileDialog::getOpenFileName(NULL,"Öffnen...",QString(),"*.qdigi");
    if(!fileName.isEmpty())
    {
        isChanged=false;
        ui->digiView->load(fileName);
        Settings::final()->addLastChanged(fileName);
        setWindowTitle("QDigi "+ui->digiView->fileName);
        updateLc();
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
    setWindowTitle("QDigi");
}

void MainWindow::changed()
{
    isChanged=true;
    setWindowTitle("QDigi * "+ui->digiView->fileName);
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
    QString fileName=QFileDialog::getSaveFileName(NULL,"Exportieren nach...",QString(),"*.png");
    if(!fileName.isEmpty())
    {
        if(!fileName.endsWith(".png"))
            fileName+=".png";
        QImage img=ui->digiView->exportImage();
        img.save(fileName);
    }
}

void MainWindow::on_actionProgramm_zum_ffnen_von_qdigi_Dateien_eintragen_triggered()
{
    Updater::registerReg();
}

bool MainWindow::askSave(bool ending)
{
    QMessageBox box;
    box.addButton(QMessageBox::Save);
    box.addButton(QMessageBox::Discard);
    if(!ending)
        box.addButton(QMessageBox::Cancel);
    box.exec();
    QMessageBox::StandardButton clicked=box.standardButton(box.clickedButton());
    if(clicked==QMessageBox::Save)
        on_actionSpeichern_triggered();
    else if(clicked==QMessageBox::Discard);
    else if(clicked==QMessageBox::Cancel)
        return false;
    else
        qDebug()<<clicked;
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isChanged)
        askSave(true);
}

void MainWindow::on_actionL_schen_triggered()
{
    ui->digiView->deleteSelection();
}

void MainWindow::openRecent()
{
    QAction* send=(QAction*)sender();
    ui->digiView->load(lastChangedActions[send]);
}

void MainWindow::updateLc()
{
    QList<QAction*> lacts=lastChangedActions.keys();
    for(int i=0;i<lacts.length();i++)
        lacts[i]->deleteLater();
    lastChangedActions.clear();
    if(ui->actionZuletzt_ge_ndert->menu()!=NULL)
        ui->actionZuletzt_ge_ndert->menu()->deleteLater();
    QMenu *lastChanged=new QMenu;
    QStringList lc2=Settings::final()->lastChanged();
    QStringList lc;
    for(int i=0;i<lc2.length();i++)
        lc.prepend(lc2[i]);
    for(int i=0;i<fmin(lc.length(),10);i++)
    {
        QString name=lc[i];
        QAction* act=lastChanged->addAction(QDir(name).dirName());
        lastChangedActions.insert(act,name);
        connect(act,SIGNAL(triggered()),this,SLOT(openRecent()));
    }
    ui->actionZuletzt_ge_ndert->setMenu(lastChanged);
}

void MainWindow::on_actionLogiktabelle_triggered()
{
    ui->digiView->createTable();
}
