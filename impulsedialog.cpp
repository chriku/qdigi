#include "impulsedialog.h"
#include "ui_impulsedialog.h"

ImpulseDialog::ImpulseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImpulseDialog)
{
    ui->setupUi(this);
    widget=ui->widget;
}

ImpulseDialog::~ImpulseDialog()
{
    delete ui;
}

void ImpulseDialog::on_exportImage_clicked()
{
    widget->exportImage();
}
