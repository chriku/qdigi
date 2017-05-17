#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = 0);
    ~HelpDialog();
public slots:
    void openLink(QUrl link);

private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
