#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "updater.h"
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
QMap<QAction*,QUrl> lastChangedActions;
Updater* updater;//Pointer, damit Proxy immer gehandelt
private slots:
void keyPressEvent(QKeyEvent* event);
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

    void on_actionLogiktabelle_triggered();
    void onRemoteSave();
    void on_actionAuf_Updates_pr_fen_triggered();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
