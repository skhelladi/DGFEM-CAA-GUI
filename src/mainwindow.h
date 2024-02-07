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
#include <nlohmann/json.hpp>
#include <QTimer>

#include <QProcess>
#include <QGridLayout>
#include <QThread>
#include <QTextStream>
#include <QMessageBox>

#include <vector>

#include <QFileSystemWatcher>

#include "charts.h"


#define VERSION "1.0.0a"

#define DEBUG false

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using json = nlohmann::json;
using ordered_json=nlohmann::ordered_json;
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getFields();
    void setFields();

    void drawChart(vector<double> vec);
    void on_watcher();
    void set_watcher_pos_zero(){m_position=0;}

protected :
    void closeEvent(QCloseEvent *event) override;

private slots:

    void onFileChanged( const QString&);

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

    // void updatePlot();

    void on_actionAbout_Qt_triggered();

    void on_toolButton_load_res_file_clicked();

    void on_toolButton_res_pause_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    void initTerminal();
    ordered_json jsonData;
    QString projectFile="";
    QTimer *timer;

    QChart          *chart;
    QValueAxis      *axisX;
    QValueAxis      *axisY;

    QLineSeries   *series1;
    QLineSeries   *series2;
    QLineSeries   *series3;
    QLineSeries   *series4;
    QLineSeries   *series5;

    size_t nb_series=5;
    vector<QLineSeries*> series;

//    vector<QLineSeries> series;

    string          filename;

    void save_config();
    void load_config();

    double max_val=-99999.9;
    double min_val= 99999.9;

    QFileSystemWatcher *m_watcher;
    //QFile file;
    QString file_name;

    qint64 m_position;
    qint64 m_size=0;

    bool m_stop;
};
#endif // MAINWINDOW_H
