#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionSpeichern_triggered();

    void on_actionEinstellungen_triggered();

    void on_actionSpeichern_Unter_triggered();

    void on_action_ffnen_triggered();

    void on_simulation_clicked();

    void on_actionNeu_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
