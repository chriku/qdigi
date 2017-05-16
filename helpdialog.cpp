#include "helpdialog.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include "settings.h"
#include "ui_helpdialog.h"
#include "helpmanager.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    //ui->mainView->page()->setNetworkAccessManager(new HelpManager);
    //connect(ui->mainView,SIGNAL(titleChanged(QString)),this,SLOT(titleChanged(QString)));
    //connect(ui->backButton,SIGNAL(clicked(bool)),ui->mainView,SLOT(back()));
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
    //ui->mainView->load(QUrl("help://blocks"));
    ui->mainView->load(QUrl("http://www.google.de"));
    show();
}

void HelpDialog::titleChanged(QString title)
{
    qDebug()<<"SeT Title"<<title;
    ui->title->setText(title);
}
