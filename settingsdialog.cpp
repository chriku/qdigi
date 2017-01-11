#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->gridType->setCurrentIndex(Settings::final()->gridType());
    ui->gridSize->setValue(Settings::final()->gridSize());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_gridSize_valueChanged(int arg1)
{
    Settings::final()->setGridSize(arg1);
}

void SettingsDialog::on_gridType_currentIndexChanged(int index)
{
    Settings::final()->setGridType((Settings::GRID)index);
}
