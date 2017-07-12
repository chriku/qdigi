#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
namespace Ui {
class PasswordDialog;
}

class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget *parent = 0);
    ~PasswordDialog();
    QLineEdit*username;
    QLineEdit*password;


private:
    Ui::PasswordDialog *ui;
};

#endif // PASSWORDDIALOG_H
