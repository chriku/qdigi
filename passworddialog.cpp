#include "passworddialog.h"
#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    ui->setupUi(this);
    username=ui->username;
    password=ui->password;
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}
