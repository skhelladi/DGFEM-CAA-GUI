#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initTerminal();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTerminal()
{
    QFont font = QApplication::font();
#ifdef Q_WS_MAC
    font.setFamily("Monaco");
#elif defined(Q_WS_QWS)
    font.setFamily("fixed");
#else
    font.setFamily("Monospace");
#endif
    font.setPointSize(10);

    ui->termwidget->setTerminalFont(font);
    ui->termwidget->setColorScheme("BlackOnWhite");
    ui->termwidget->setScrollBarPosition(QTermWidget::ScrollBarRight);


    foreach (QString arg, QApplication::arguments())
    {
        if (ui->termwidget->availableColorSchemes().contains(arg))
            ui->termwidget->setColorScheme(arg);
        if (ui->termwidget->availableKeyBindings().contains(arg))
            ui->termwidget->setKeyBindings(arg);
    }
    // info output
    qDebug() << "* INFO *************************";
    qDebug() << " availableKeyBindings:" << ui->termwidget->availableKeyBindings();
    qDebug() << " keyBindings:" << ui->termwidget->keyBindings();
    qDebug() << " availableColorSchemes:" << ui->termwidget->availableColorSchemes();
    qDebug() << "* INFO END *********************";

    // real startup
    connect(ui->termwidget, SIGNAL(finished()), this, SLOT(close()));
}

void MainWindow::on_toolButton_add_term_clicked()
{
    ui->termwidget->close();
    ui->termwidget=new QTermWidget();
    ui->gridLayout_2->addWidget(ui->termwidget, 1, 0, 1, 1);
    ui->termwidget->show();
    initTerminal();

}

void MainWindow::on_toolButton_rm_term_clicked()
{
    ui->termwidget->close();
}


void MainWindow::on_spinBox_nbBC_valueChanged(int row)
{
    ui->tableWidget_BC->setRowCount(row);
    if(row!=0)
    {
        for(int i=0;i<row;i++)
        {
            QComboBox *cb = new QComboBox;
            cb->addItems({"Absorbing","Reflecting"});
            ui->tableWidget_BC->setCellWidget(i,1,cb);
        }
    }
}

void MainWindow::on_action_Load_Gmsh_mesh_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Gmsh mesh File"),QDir::currentPath(),tr("Files (*.msh)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Case file"),tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }

    ui->lineEdit_mesh->setText(fileName);

    statusBar()->showMessage(tr("Mesh loaded"), 2000);
}


void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open case File"),QDir::currentPath(),tr("Files (*.conf)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Case file"),tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }

    QTextStream ReadFile(&file);
    ui->textEdit_config->setText(ReadFile.readAll());

//    Set_Fields();

//    ui->action_Generate_geometry->setEnabled(true);
    //ui->actionSurge_analysis->setEnabled(true);

    QString title =  QCoreApplication::organizationName()+tr(" - ")+fileName;
    setWindowTitle(title);

    statusBar()->showMessage(tr("Case loaded"), 2000);
}




