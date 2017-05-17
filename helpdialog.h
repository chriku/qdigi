#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QWebView>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QWebView
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = 0);
    ~HelpDialog();
    static void help(QString about);
    void showHelp(QString about);
public slots:
    void titleChanged(QString title);
};

#endif // HELPDIALOG_H
