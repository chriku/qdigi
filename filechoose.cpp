#include "filechoose.h"
#include "ui_filechoose.h"

FileChoose::FileChoose(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileChoose)
{
    ui->setupUi(this);
}

FileChoose::~FileChoose()
{
    delete ui;
}

void FileChoose::on_fileName_returnPressed()
{
    ret=ui->fileName->text();
    close();
}

void FileChoose::on_buttonBox_accepted()
{
    ret=ui->fileName->text();
}
