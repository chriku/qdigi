#ifndef IMPULSEDIALOG_H
#define IMPULSEDIALOG_H

#include <QDialog>
#include "impulsewidget.h"

namespace Ui {
class ImpulseDialog;
}

class ImpulseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImpulseDialog(QWidget *parent = 0);
    ~ImpulseDialog();
ImpulseWidget* widget;
private slots:
void on_exportImage_clicked();

private:
    Ui::ImpulseDialog *ui;
};

#endif // IMPULSEDIALOG_H
