#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <qtermwidget5/qtermwidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_toolButton_add_term_clicked();


    void on_spinBox_nbBC_valueChanged(int arg1);

    void on_action_Load_Gmsh_mesh_file_triggered();

    void on_action_Open_triggered();

    void on_toolButton_rm_term_clicked();

private:
    Ui::MainWindow *ui;
    void initTerminal();

};
#endif // MAINWINDOW_H
