#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = 0);
    ~HelpDialog();
    static void help(QString about);
    void showHelp(QString about);
public slots:
    void loadFile(QString fileName);
private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H