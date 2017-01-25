#ifndef KVDIAGRAM_H
#define KVDIAGRAM_H

#include <QDialog>

namespace Ui {
class KVDiagram;
}

class KVDiagram : public QDialog
{
    Q_OBJECT

public:
    explicit KVDiagram(QWidget *parent = 0);
    ~KVDiagram();
void redoTable();
private slots:
    void on_inputs_valueChanged(int arg1);

    void on_buttonBox_accepted();

private:
    Ui::KVDiagram *ui;
};

#endif // KVDIAGRAM_H
