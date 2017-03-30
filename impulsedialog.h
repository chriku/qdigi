#ifndef IMPULSEDIALOG_H
#define IMPULSEDIALOG_H

#include <QDialog>

namespace Ui {
class ImpulseDialog;
}

class ImpulseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImpulseDialog(QWidget *parent = 0);
    ~ImpulseDialog();

private:
    Ui::ImpulseDialog *ui;
};

#endif // IMPULSEDIALOG_H
