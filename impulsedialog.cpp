#include "impulsedialog.h"
#include "ui_impulsedialog.h"

ImpulseDialog::ImpulseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImpulseDialog)
{
    ui->setupUi(this);
}

ImpulseDialog::~ImpulseDialog()
{
    delete ui;
}
