#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <QMessageBox>
#include <QtMultimedia/QSound>

#include <qtermwidget5/qtermwidget.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <QTimer>

#define VERSION "1.0.0a"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using json = nlohmann::json;
using ordered_json=nlohmann::ordered_json;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getFields();
    void setFields();

protected:
    void closeEvent(QCloseEvent *event);


private slots:
    void on_toolButton_add_term_clicked();

    void on_action_Load_Gmsh_mesh_file_triggered();

    void on_action_Open_triggered();

    void on_toolButton_rm_term_clicked();

    void on_toolButton_goSolver_clicked();

    void on_toolButton_goInit_clicked();

    void on_toolButton_goObs_clicked();

    void on_toolButton_goSrc_clicked();

    void on_action_About_triggered();

    void on_action_Save_triggered();

    void on_toolButton_BC_apply_clicked();

    void on_toolButton_Init_apply_clicked();

    void on_toolButton_Obs_apply_clicked();

    void on_toolButton_Src_apply_clicked();

    void on_commandLinkButton_save_clicked();

    void on_actionRun_triggered();

    void on_toolButton_Load_solver_clicked();

    void on_action_View_results_triggered();

    void on_toolButton_play_sound_clicked(bool checked);

    void on_toolButton_stop_sound_clicked();

    void updatePlot();

    void on_actionAbout_Qt_triggered();

private:
    Ui::MainWindow *ui;
    void initTerminal();
    ordered_json jsonData;
    QString projectFile="";
    QTimer *timer;
};
#endif // MAINWINDOW_H
