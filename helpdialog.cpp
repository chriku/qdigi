#include "helpdialog.h"
#include <QFile>
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
    QFile file("help/blocks.md");
    file.open(QFile::ReadOnly);
    ui->content->setText(file.readAll());
    file.close();
    show();
}
