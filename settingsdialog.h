#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QDebug>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_gridType_currentIndexChanged(int index);

    void on_defaultSimu_clicked(bool checked);

    void on_rasterSize_valueChanged(int arg1);

    void on_penWidth_valueChanged(double arg1);
    void load();

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_changeLicense_clicked();

    void on_background_clicked();

    void on_simulationTime_valueChanged(int arg1);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
