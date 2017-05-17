#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTextBrowser>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QTextBrowser
{
    Q_OBJECT
public:
    explicit HelpDialog(QWidget *parent = 0);
    void showHelp(QString about);
    static void help(QString about);
    QString getText(QUrl url);
public slots:
    void titleChanged(QString title);
};

#endif // HELPDIALOG_H
