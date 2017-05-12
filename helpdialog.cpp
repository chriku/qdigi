#include "helpdialog.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include "settings.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::help(QString about)
{
    HelpDialog* dialog=new HelpDialog;
    dialog->showHelp(about);
}

void HelpDialog::showHelp(QString about)
{
    loadFile("blocks");
    connect(ui->content,SIGNAL(linkClicked(QString)),this,SLOT(loadFile(QString)));
    show();
}

void HelpDialog::loadFile(QString fileName)
{
    qDebug()<<fileName;
    QDir dir(Settings::final()->applicationDir());
    dir=QDir(dir.absoluteFilePath("help"));
    QFile file(dir.absoluteFilePath(fileName+".md"));
    QByteArray data;
    if(!file.exists())
        data="# Datei nicht gefunden";
    else
    {
        file.open(QFile::ReadOnly);
        data=file.readAll();
        file.close();
    }
    ui->content->setText(data);
}
