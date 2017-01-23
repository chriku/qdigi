#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->gridType->setCurrentIndex(Settings::final()->gridType());
    ui->defaultSimu->setChecked(Settings::final()->defaultSimu());
    ui->rasterSize->setValue(Settings::final()->rasterSize());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_gridType_currentIndexChanged(int index)
{
    Settings::final()->setGridType((Settings::GRID)index);
}

void SettingsDialog::on_defaultSimu_clicked(bool checked)
{
    Settings().final()->setDefaultSimu(checked);
}

void SettingsDialog::on_rasterSize_valueChanged(int arg1)
{
    Settings::final()->setRasterSize(arg1);
}
