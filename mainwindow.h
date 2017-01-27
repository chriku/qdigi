#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
bool isChanged;
QMap<QAction*,QString> lastChangedActions;

private slots:
    void on_actionSpeichern_triggered();

    void on_actionEinstellungen_triggered();

    void on_actionSpeichern_Unter_triggered();

    void on_action_ffnen_triggered();

    void on_simulation_clicked();

    void on_actionNeu_triggered();

    void changed();

    void on_actionZoom_R_cksetzen_triggered();

    void on_actionReinzoomen_triggered();

    void on_actionRauszoomen_triggered();

    void on_actionAls_Bild_Speichern_triggered();

    void on_actionProgramm_zum_ffnen_von_qdigi_Dateien_eintragen_triggered();
    bool askSave(bool ending);
    void on_actionL_schen_triggered();
    void openRecent();
    void updateLc();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
