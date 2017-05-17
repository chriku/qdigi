#include "helpdialog.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include "settings.h"
#include "helpmanager.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QWebView(parent)
{
    page()->setNetworkAccessManager(new HelpManager);
}

HelpDialog::~HelpDialog()
{
}

void HelpDialog::help(QString about)
{
    HelpDialog* dialog=new HelpDialog;
    dialog->showHelp(about);
}

void HelpDialog::showHelp(QString about)
{
    load(QUrl("help://blocks"));
    //load(QUrl("http://www.google.de"));
    show();
}

void HelpDialog::titleChanged(QString title)
{
}
