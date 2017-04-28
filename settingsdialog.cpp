#include "settingsdialog.h"
#include <QColorDialog>
#include "ui_settingsdialog.h"
#include "settings.h"
#include <QInputDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    load();
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

void SettingsDialog::on_penWidth_valueChanged(double arg1)
{
    Settings::final()->setPenWidth(arg1);
    qDebug()<<arg1;
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->standardButton(button)==QDialogButtonBox::RestoreDefaults)
    {
        Settings::final()->clear();
        load();
    }
}

void SettingsDialog::load()
{
    ui->gridType->setCurrentIndex(Settings::final()->gridType());
    ui->defaultSimu->setChecked(Settings::final()->defaultSimu());
    ui->rasterSize->setValue(Settings::final()->rasterSize());
    ui->penWidth->setValue(Settings::final()->penWidth());
    ui->simulationTime->setValue(Settings::final()->simulationTime());
}

void SettingsDialog::on_changeLicense_clicked()
{
    QInputDialog dialog;
    dialog.setInputMode(QInputDialog::TextInput);
    dialog.setWindowTitle("QDigi");
    dialog.setLabelText("Lizenzschlüssel eingeben\nAbbrechen für Offline-Arbeiten");
    dialog.exec();
    QString lic=dialog.textValue();
    Settings::final()->setLicense(lic);
}

void SettingsDialog::on_background_clicked()
{
    Settings::final()->setBackground(QColorDialog::getColor(Settings::final()->background(),NULL,"QDigi"));
}

void SettingsDialog::on_simulationTime_valueChanged(int arg1)
{
    Settings::final()->setSimulationTime(arg1);
}
