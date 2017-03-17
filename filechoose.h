#ifndef FILECHOOSE_H
#define FILECHOOSE_H

#include <QDialog>

namespace Ui {
class FileChoose;
}

class FileChoose : public QDialog
{
    Q_OBJECT

public:
    explicit FileChoose(QWidget *parent = 0);
    ~FileChoose();

    QString ret;
private slots:
    void on_fileName_returnPressed();

    void on_buttonBox_accepted();

private:
    Ui::FileChoose *ui;
};

#endif // FILECHOOSE_H
