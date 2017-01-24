#include "kvdiagram.h"
#include "ui_kvdiagram.h"

KVDiagram::KVDiagram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KVDiagram)
{
    ui->setupUi(this);
}

KVDiagram::~KVDiagram()
{
    delete ui;
}
