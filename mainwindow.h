#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <qtermwidget5/qtermwidget.h>

#define VERSION "1.0.0a"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);


private slots:
    void on_toolButton_add_term_clicked();


    void on_spinBox_nbBC_valueChanged(int arg1);

    void on_action_Load_Gmsh_mesh_file_triggered();

    void on_action_Open_triggered();

    void on_toolButton_rm_term_clicked();

    void on_spinBox_nbInit_valueChanged(int arg1);

    void on_spinBox_nbObs_valueChanged(int arg1);

    void on_spinBox_nbSrc_valueChanged(int arg1);

    void on_toolButton_goSolver_clicked();

    void on_toolButton_goInit_clicked();

    void on_toolButton_goObs_clicked();

    void on_toolButton_goSrc_clicked();

    void on_action_About_triggered();

private:
    Ui::MainWindow *ui;
    void initTerminal();
};
#endif // MAINWINDOW_H
