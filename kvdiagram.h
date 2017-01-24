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

private:
    Ui::KVDiagram *ui;
};

#endif // KVDIAGRAM_H
