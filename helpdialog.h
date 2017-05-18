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
    QStringList last;
public slots:
    void openLink(QString link);
    QString getFile(QString url);
private slots:
    void on_back_clicked();

private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
