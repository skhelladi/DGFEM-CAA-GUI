#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initTerminal();

    QStringList m_TableHeader;
    m_TableHeader<<"Name"<<"Type";
    size_t textSize=12;

    ui->tableWidget_BC->setColumnCount(2);
    ui->tableWidget_BC->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_BC->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");

    m_TableHeader.clear();
    m_TableHeader<<"Name"<<"Type"<<"X"<<"Y"<<"Z"<<"Size"<<"Amplitude";
    ui->tableWidget_Init->setColumnCount(7);
    ui->tableWidget_Init->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_Init->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");

    m_TableHeader.clear();
    m_TableHeader<<"Name"<<"X"<<"Y"<<"Z"<<"Size";
    ui->tableWidget_Obs->setColumnCount(5);
    ui->tableWidget_Obs->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_Obs->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");

    m_TableHeader.clear();
    m_TableHeader<<"Name"<<"Type"<<"Formula/File"<<"X"<<"Y"<<"Z"<<"Size"<<"Intensity"<<"Frequency"<<"Phase"<<"Duration";
    ui->tableWidget_Src->setColumnCount(11);
    ui->tableWidget_Src->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_Src->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");

    ui->toolBox->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);

    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setText("Close the application:");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Yes:
    {
        event->accept();
        //bool hasMdiChild = (activeMdiChild() != nullptr);
        //if(hasMdiChild) ui->mdiArea->closeActiveSubWindow();
        exit(0);
    }
    case QMessageBox::Cancel:
        event->ignore();
        break;
    default:
        event->ignore();
        break;
    }

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

    QString title =  QCoreApplication::organizationName()+tr(" - ")+fileName;
    setWindowTitle(title);

    statusBar()->showMessage(tr("Case loaded"), 2000);
}

void MainWindow::on_spinBox_nbInit_valueChanged(int row)
{
    ui->tableWidget_Init->setRowCount(row);

    if(row!=0)
    {
        for(int i=0;i<row;i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem("initialCondtition"+QString::number(i+1));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable); // set item not editable
            ui->tableWidget_Init->setItem(i,0,item);
            QComboBox *cb = new QComboBox;
            cb->addItems({"Gaussian"});
            ui->tableWidget_Init->setCellWidget(i,1,cb); //type
        }
        ui->tableWidget_Init->setItem(row-1,2, new QTableWidgetItem("0.0")); //X
        ui->tableWidget_Init->setItem(row-1,3, new QTableWidgetItem("0.0")); //Y
        ui->tableWidget_Init->setItem(row-1,4, new QTableWidgetItem("0.0")); //Z
        ui->tableWidget_Init->setItem(row-1,5, new QTableWidgetItem("1.0")); //Size
        ui->tableWidget_Init->setItem(row-1,6, new QTableWidgetItem("1.0")); //Amplitude
    }
    ui->tableWidget_Init->show();
}


void MainWindow::on_spinBox_nbObs_valueChanged(int row)
{
    ui->tableWidget_Obs->setRowCount(row);

    if(row!=0)
    {
        for(int i=0;i<row;i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem("observer"+QString::number(i+1));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable); // set item not editable
            ui->tableWidget_Obs->setItem(i,0,item);
        }
        ui->tableWidget_Obs->setItem(row-1,1, new QTableWidgetItem("0.0")); //X
        ui->tableWidget_Obs->setItem(row-1,2, new QTableWidgetItem("0.0")); //Y
        ui->tableWidget_Obs->setItem(row-1,3, new QTableWidgetItem("0.0")); //Z
        ui->tableWidget_Obs->setItem(row-1,4, new QTableWidgetItem("0.0")); //size
    }
    ui->tableWidget_Obs->show();
}


void MainWindow::on_spinBox_nbSrc_valueChanged(int row)
{
    ui->tableWidget_Src->setRowCount(row);

    if(row!=0)
    {
        for(int i=0;i<row;i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem("source"+QString::number(i+1));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable); // set item not editable
            ui->tableWidget_Src->setItem(i,0,item);
            QComboBox *cb = new QComboBox;
            cb->addItems({"Monopole","Dipole","quadrupole","formula","file"});
            ui->tableWidget_Src->setCellWidget(i,1,cb);
        }
        ui->tableWidget_Src->setItem(row-1,2, new QTableWidgetItem("\"\"")); //formula/data file name
        ui->tableWidget_Src->setItem(row-1,3, new QTableWidgetItem("0.0")); //X
        ui->tableWidget_Src->setItem(row-1,4, new QTableWidgetItem("0.0")); //Y
        ui->tableWidget_Src->setItem(row-1,5, new QTableWidgetItem("0.0")); //Z
        ui->tableWidget_Src->setItem(row-1,6, new QTableWidgetItem("0.0")); //Size
        ui->tableWidget_Src->setItem(row-1,7, new QTableWidgetItem("")); //Intensity
        ui->tableWidget_Src->setItem(row-1,8, new QTableWidgetItem("")); //Frequency
        ui->tableWidget_Src->setItem(row-1,9, new QTableWidgetItem("")); //Phase
        ui->tableWidget_Src->setItem(row-1,10, new QTableWidgetItem(ui->lineEdit_timeEnd->text())); //Duration
    }
    ui->tableWidget_Src->show();
}


void MainWindow::on_toolButton_goSolver_clicked()
{
    ui->toolBox->setCurrentIndex(ui->toolBox->currentIndex()+1);
}


void MainWindow::on_toolButton_goInit_clicked()
{
    ui->toolBox->setCurrentIndex(ui->toolBox->currentIndex()+1);
}


void MainWindow::on_toolButton_goObs_clicked()
{
    ui->toolBox->setCurrentIndex(ui->toolBox->currentIndex()+1);
}


void MainWindow::on_toolButton_goSrc_clicked()
{
    ui->toolBox->setCurrentIndex(ui->toolBox->currentIndex()+1);
}


void MainWindow::on_action_About_triggered()
{
    QString about_text;
    int yy,mm,dd;
    QDate date;
    date=QDate::currentDate();
    date.getDate(&yy,&mm,&dd);
    QString sdate=QString::number(dd)+"."+QString::number(mm)+"."+QString::number(yy);

    about_text=QCoreApplication::applicationName()+" "+
            QCoreApplication::applicationVersion()+"\n\n"+
            "built in "+sdate+" under GPL3 LICENSE\n"+
            "- based on Qt "+QT_VERSION_STR+"\n"+
            "- based on DGFEM-CAA-GUI "+VERSION+"\n\n"+
            "Developpers:\n- Pr. Sofiane KHELLADI <sofiane.khelladi@ensam.eu>";

    QMessageBox::about(this, "About TurboKit",about_text);
}

