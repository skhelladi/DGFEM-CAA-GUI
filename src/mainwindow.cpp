#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <fstream>

QStringList listBC={"Absorbing","Reflecting"};
QStringList listInit={"gaussian"};
QStringList listSrc={"monopole","dipole","quadrupole","formula","file"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initTerminal();

    ui->toolButton_res_pause->setVisible(false);

    QStringList m_TableHeader;
    m_TableHeader<<"Name             "<<"Type                   ";
    size_t textSize=12;

    ui->tableWidget_BC->setColumnCount(2);
    ui->tableWidget_BC->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_BC->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");
    ui->tableWidget_BC->resizeColumnsToContents();
    ui->tableWidget_BC->horizontalHeader()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_TableHeader.clear();
    m_TableHeader<<"Type                "<<"X[m]   "<<"Y[m]   "<<"Z[m]   "<<"Size[m]   "<<"Amplitude[Pa]   ";
    ui->tableWidget_Init->setColumnCount(6);
    ui->tableWidget_Init->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_Init->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");
    ui->tableWidget_Init->resizeColumnsToContents();
    ui->tableWidget_Init->horizontalHeader()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_TableHeader.clear();
    m_TableHeader<<"X[m]   "<<"Y[m]   "<<"Z[m]   "<<"Size[m]   ";
    ui->tableWidget_Obs->setColumnCount(4);
    ui->tableWidget_Obs->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_Obs->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");
    ui->tableWidget_Obs->resizeColumnsToContents();
    ui->tableWidget_Obs->horizontalHeader()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_TableHeader.clear();
    m_TableHeader<<"Type                                         "<<"fct                                                                        "
                  <<"X[m]   "<<"Y[m]   "<<"Z[m]   "<<"Size[m]   "
                  <<"amplitude[Pa]   "<<"Frequency[Hz]   "<<"Phase[rad]   "<<"Duration[s]   ";
    ui->tableWidget_Src->setColumnCount(10);
    ui->tableWidget_Src->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_Src->horizontalHeader()->setStyleSheet("QHeaderView {font: bold "+QString::number(textSize)+"px;}");
    ui->tableWidget_Src->resizeColumnsToContents();
    ui->tableWidget_Src->horizontalHeader()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->toolBox->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);

    ui->termwidget->sendText(tr("mkdir results\nclear\n"));
    //    ui->termwidget->
    ui->termwidget->sendText(tr("export TERM=linux\nclear\n"));

    // timer setup
    timer = new QTimer(this);

    showMaximized();

    chart = new QChart();

    for(size_t i=0;i<nb_series;i++)
    {
        series.push_back(new QLineSeries());
    }

    vector<QPen> pen;
    pen.push_back(QPen(QColor(255, 0, 0)));
    pen.push_back(QPen(QColor(0, 0, 255)));
    pen.push_back(QPen(QColor(255, 0, 0, 100)));
    pen.push_back(QPen(QColor(0, 0, 255, 100)));
    pen.push_back(QPen(QColor(0, 255, 255, 100)));

    vector<QString> Name = {"Pressure","Density","Velocity - X","Velocity - Y","Velocity - Z"};

    axisX = new QValueAxis;
    axisX->setRange(0, 1.0e-3);
    axisX->setTitleText("Time");
    axisX->setLabelFormat("%g");


    axisY = new QValueAxis;
    axisY->setRange(-10, 10);
    axisY->setTitleText("|log10(Residuals)|");
    axisY->setLabelFormat("%g");
    axisY->setMinorTickCount(-1);


    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for(size_t i=0;i<nb_series;i++)
    {
        chart->addSeries(series[i]);

        pen[i].setWidth(2);
        series[i]->setPen(pen[i]);
        series[i]->setName(Name[i]);
        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
    }




    filename = ui->lineEdit_res_file->text().toStdString();
    load_config();
}


MainWindow::~MainWindow()
{
    // delete series1;
    // delete series2;
    // delete series3;
    // delete series4;
    // delete series5;
    for(size_t i=0;i<nb_series;i++)
    {
        delete series[i];
    }
    delete chart;
    delete ui;
}

void MainWindow::getFields()
{
    jsonData.clear();
    jsonData["mesh"]["File"]=ui->lineEdit_mesh->text().toStdString();

    // loop on table
    jsonData["mesh"]["BC"]["number"]=ui->tableWidget_BC->rowCount();
    for(int i=0;i<ui->tableWidget_BC->rowCount();i++)
    {
        jsonData["mesh"]["BC"]["boundary"+QString::number(i+1).toStdString()]["name"] = ui->tableWidget_BC->item(i,0)->text().toStdString();
        QComboBox *cb= qobject_cast<QComboBox*>(ui->tableWidget_BC->cellWidget(i,1));
        jsonData["mesh"]["BC"]["boundary"+QString::number(i+1).toStdString()]["type"] = cb->currentText().toStdString();
    }

    jsonData["solver"]["time"]["start"]=ui->lineEdit_timeStart->text().toDouble();
    jsonData["solver"]["time"]["end"]=ui->lineEdit_timeEnd->text().toDouble();
    jsonData["solver"]["time"]["step"]=ui->lineEdit_timeStep->text().toDouble();
    jsonData["solver"]["time"]["rate"]=ui->lineEdit_timeRate->text().toDouble();
    jsonData["solver"]["elementType"]=(ui->comboBox_elemType->currentIndex()==0)?"Lagrange":"IsoParametric";
    jsonData["solver"]["timeIntMethod"]=(ui->comboBox_timeInteg->currentIndex()==0)?"Euler1":"Runge-Kutta";
    jsonData["solver"]["numThreads"]=ui->spinBox_cpu->value();

    jsonData["initialization"]["meanFlow"]["vx"] = ui->lineEdit_vx->text().toDouble();
    jsonData["initialization"]["meanFlow"]["vy"] = ui->lineEdit_vy->text().toDouble();
    jsonData["initialization"]["meanFlow"]["vz"] = ui->lineEdit_vz->text().toDouble();
    jsonData["initialization"]["meanFlow"]["rho"] = ui->lineEdit_rho->text().toDouble();
    jsonData["initialization"]["meanFlow"]["c"] = ui->lineEdit_c->text().toDouble();

    // loop on table
    jsonData["initialization"]["number"]=ui->tableWidget_Init->rowCount();
    for(int i=0;i<ui->tableWidget_Init->rowCount();i++)
    {
        jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["type"] =
            qobject_cast<QComboBox*>(ui->tableWidget_Init->cellWidget(i,0))->currentText().toStdString();
        jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["x"] = ui->tableWidget_Init->item(i,1)->text().toDouble();
        jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["y"] = ui->tableWidget_Init->item(i,2)->text().toDouble();
        jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["z"] = ui->tableWidget_Init->item(i,3)->text().toDouble();
        jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["size"] = ui->tableWidget_Init->item(i,4)->text().toDouble();
        jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["amplitude"] = ui->tableWidget_Init->item(i,5)->text().toDouble();
    }

    // observers
    jsonData["observers"]["number"]=ui->tableWidget_Obs->rowCount();
    for(int i=0;i<ui->tableWidget_Obs->rowCount();i++)
    {
        jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["x"] = ui->tableWidget_Obs->item(i,0)->text().toDouble();
        jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["y"] = ui->tableWidget_Obs->item(i,1)->text().toDouble();
        jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["z"] = ui->tableWidget_Obs->item(i,2)->text().toDouble();
        jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["size"] = ui->tableWidget_Obs->item(i,3)->text().toDouble();
    }

    // sources
    jsonData["sources"]["number"]=ui->tableWidget_Src->rowCount();
    for(int i=0;i<ui->tableWidget_Src->rowCount();i++)
    {
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["type"] =
            qobject_cast<QComboBox*>(ui->tableWidget_Src->cellWidget(i,0))->currentText().toStdString();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["fct"] = ui->tableWidget_Src->item(i,1)->text().toStdString();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["x"] = ui->tableWidget_Src->item(i,2)->text().toDouble();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["y"] = ui->tableWidget_Src->item(i,3)->text().toDouble();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["z"] = ui->tableWidget_Src->item(i,4)->text().toDouble();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["size"] = ui->tableWidget_Src->item(i,5)->text().toDouble();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["amplitude"] = ui->tableWidget_Src->item(i,6)->text().toDouble();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["frequency"] = ui->tableWidget_Src->item(i,7)->text().toDouble();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["phase"] = ui->tableWidget_Src->item(i,8)->text().toDouble();
        jsonData["sources"]["source"+QString::number(i+1).toStdString()]["duration"] = ui->tableWidget_Src->item(i,9)->text().toDouble();
    }
}

void MainWindow::setFields()
{
    //    jsonData["configurationFile"]="file.conf";

    ui->lineEdit_mesh->setText(QString::fromStdString(jsonData["mesh"]["File"]));

    // Mesh
    ui->spinBox_nbBC->setValue(jsonData["mesh"]["BC"]["number"]);
    ui->tableWidget_BC->setRowCount(jsonData["mesh"]["BC"]["number"]);
    for(int i=0;i<ui->tableWidget_BC->rowCount();i++)
    {
        std::string str = jsonData["mesh"]["BC"]["boundary"+QString::number(i+1).toStdString()]["name"];
        ui->tableWidget_BC->setItem(i,0, new QTableWidgetItem(QString::fromStdString(str)));
        QComboBox *cb = new QComboBox;
        cb->addItems(listBC);
        cb->setStyleSheet("QComboBox {font: bold "+QString::number(12)+"px;}");
        ui->tableWidget_BC->setCellWidget(i,1,cb);
        str = jsonData["mesh"]["BC"]["boundary"+QString::number(i+1).toStdString()]["type"];
        cb->setCurrentIndex((str=="Absorbing")?0:1);
    }

    // Initialization
    double nbr = jsonData["solver"]["time"]["start"];
    ui->lineEdit_timeStart->setText(QString::number(nbr));
    nbr = jsonData["solver"]["time"]["end"];
    ui->lineEdit_timeEnd->setText(QString::number(nbr));
    nbr = jsonData["solver"]["time"]["step"];
    ui->lineEdit_timeStep->setText(QString::number(nbr));
    nbr = jsonData["solver"]["time"]["rate"];
    ui->lineEdit_timeRate->setText(QString::number(nbr));
    ui->comboBox_elemType->setCurrentIndex((jsonData["solver"]["elementType"]=="Lagrange")?0:1);
    ui->comboBox_timeInteg->setCurrentIndex((jsonData["solver"]["timeIntMethod"]=="Euler1")?0:1);
    ui->spinBox_cpu->setValue(jsonData["solver"]["numThreads"]);

    nbr = jsonData["initialization"]["meanFlow"]["vx"];
    ui->lineEdit_vx->setText(QString::number(nbr));
    nbr = jsonData["initialization"]["meanFlow"]["vy"];
    ui->lineEdit_vy->setText(QString::number(nbr));
    nbr = jsonData["initialization"]["meanFlow"]["vz"];
    ui->lineEdit_vz->setText(QString::number(nbr));
    nbr = jsonData["initialization"]["meanFlow"]["rho"];
    ui->lineEdit_rho->setText(QString::number(nbr));
    nbr = jsonData["initialization"]["meanFlow"]["c"];
    ui->lineEdit_c->setText(QString::number(nbr));

    ui->spinBox_nbInit->setValue(jsonData["initialization"]["number"]);
    ui->tableWidget_Init->setRowCount(jsonData["initialization"]["number"]);
    for(int i=0;i<ui->tableWidget_Init->rowCount();i++)
    {
        //        QString str = "initialCondition"+QString::number(i+1);
        //        ui->tableWidget_Init->setItem(i,0, new QTableWidgetItem(str));

        QComboBox *cb = new QComboBox;
        cb->addItems(listInit);
        cb->setStyleSheet("QComboBox {font: bold "+QString::number(12)+"px;}");
        ui->tableWidget_Init->setCellWidget(i,0,cb);
        QString str = QString::fromStdString(jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["type"]);
        int index=0;
        if(str=="gaussian")   index=0;
        cb->setCurrentIndex(index);

        nbr = jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["x"];
        ui->tableWidget_Init->setItem(i,1, new QTableWidgetItem(QString::number(nbr)));
        nbr = jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["y"];
        ui->tableWidget_Init->setItem(i,2, new QTableWidgetItem(QString::number(nbr)));
        nbr = jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["z"];
        ui->tableWidget_Init->setItem(i,3, new QTableWidgetItem(QString::number(nbr)));
        nbr = jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["size"];
        ui->tableWidget_Init->setItem(i,4, new QTableWidgetItem(QString::number(nbr)));
        nbr = jsonData["initialization"]["initialCondition"+QString::number(i+1).toStdString()]["amplitude"];
        ui->tableWidget_Init->setItem(i,5, new QTableWidgetItem(QString::number(nbr)));
    }

    // Observers position
    ui->spinBox_nbObs->setValue(jsonData["observers"]["number"]);
    ui->tableWidget_Obs->setRowCount(jsonData["observers"]["number"]);
    for(int i=0;i<ui->tableWidget_Obs->rowCount();i++)
    {
        //        QString str = "observer"+QString::number(i+1);
        //        ui->tableWidget_Obs->setItem(i,0, new QTableWidgetItem(str));
        nbr = jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["x"];
        ui->tableWidget_Obs->setItem(i,0, new QTableWidgetItem(QString::number(nbr)));
        nbr = jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["y"];
        ui->tableWidget_Obs->setItem(i,1, new QTableWidgetItem(QString::number(nbr)));
        nbr = jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["z"];
        ui->tableWidget_Obs->setItem(i,2, new QTableWidgetItem(QString::number(nbr)));
        nbr = jsonData["observers"]["observer"+QString::number(i+1).toStdString()]["size"];
        ui->tableWidget_Obs->setItem(i,3, new QTableWidgetItem(QString::number(nbr)));
    }

    // Sources
    ui->spinBox_nbSrc->setValue(jsonData["sources"]["number"]);
    ui->tableWidget_Src->setRowCount(jsonData["sources"]["number"]);
    for(int i=0;i<ui->tableWidget_Src->rowCount();i++)
    {
        //        QString str = "source"+QString::number(i+1);
        //        ui->tableWidget_Src->setItem(i,0, new QTableWidgetItem(str));

        QComboBox *cb = new QComboBox;
        cb->addItems(listSrc);
        cb->setStyleSheet("QComboBox {font: bold "+QString::number(12)+"px;}");
        ui->tableWidget_Src->setCellWidget(i,0,cb);
        QString str = QString::fromStdString(jsonData["sources"]["source"+QString::number(i+1).toStdString()]["type"]);
        int index=0;
        if(str=="monopole")   index=0;
        if(str=="dipole")     index=1;
        if(str=="quadrupole") index=2;
        if(str=="formula")    index=3;
        if(str=="file")       index=4;
        cb->setCurrentIndex(index);
        str = QString::fromStdString(jsonData["sources"]["source"+QString::number(i+1).toStdString()]["fct"]);
        ui->tableWidget_Src->setItem(i,1, new QTableWidgetItem(str));

        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["x"];
        ui->tableWidget_Src->setItem(i,2, new QTableWidgetItem(QString::number(nbr)));
        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["y"];
        ui->tableWidget_Src->setItem(i,3, new QTableWidgetItem(QString::number(nbr)));
        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["z"];
        ui->tableWidget_Src->setItem(i,4, new QTableWidgetItem(QString::number(nbr)));
        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["size"];
        ui->tableWidget_Src->setItem(i,5, new QTableWidgetItem(QString::number(nbr)));
        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["amplitude"];
        ui->tableWidget_Src->setItem(i,6, new QTableWidgetItem(QString::number(nbr)));
        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["frequency"];
        ui->tableWidget_Src->setItem(i,7, new QTableWidgetItem(QString::number(nbr)));
        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["phase"];
        ui->tableWidget_Src->setItem(i,8, new QTableWidgetItem(QString::number(nbr)));
        nbr =  jsonData["sources"]["source"+QString::number(i+1).toStdString()]["duration"];
        ui->tableWidget_Src->setItem(i,9, new QTableWidgetItem(QString::number(nbr)));
    }
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

        for(size_t i=0;i<nb_series;i++)
        {
            series[i]->clear();
        }
        event->accept();

        break;
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
    if(DEBUG)
    {
        qDebug() << "* INFO *************************";
        qDebug() << " availableKeyBindings:" << ui->termwidget->availableKeyBindings();
        qDebug() << " keyBindings:" << ui->termwidget->keyBindings();
        qDebug() << " availableColorSchemes:" << ui->termwidget->availableColorSchemes();
        qDebug() << "* INFO END *********************";
    }

    // real startup
    connect(ui->termwidget, SIGNAL(finished()), this, SLOT(close()));
}

void MainWindow::on_toolButton_add_term_clicked()
{
    ui->termwidget->close();
    ui->termwidget=new QTermWidget();
    ui->gridLayout->addWidget(ui->termwidget, 1, 0, 1, 1);
    ui->termwidget->show();
    initTerminal();

}

void MainWindow::on_toolButton_rm_term_clicked()
{
    ui->termwidget->close();
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
    projectFile = QFileDialog::getOpenFileName(this, tr("Open project File"),QDir::currentPath(),tr("Files (*.json)"));

    if (projectFile.isEmpty())
        return;

    QFile file(projectFile);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Project file"),tr("Cannot read file %1:\n%2.").arg(projectFile).arg(file.errorString()));
        return;
    }

    QTextStream ReadFile(&file);

    std::ifstream infile(projectFile.toStdString().c_str());
    infile >> jsonData;
    infile.close();

    setFields();

    QString title =  QCoreApplication::organizationName()+tr(" - ")+projectFile;
    setWindowTitle(title);

    statusBar()->showMessage(tr("Project loaded"), 2000);
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


void MainWindow::on_action_Save_triggered()
{
    QString project_filename=tr("project.json");
    projectFile = QFileDialog::getSaveFileName(this, tr("Save Project File"),project_filename,tr("Project files (*.json)"));

    if (projectFile.isEmpty())
        return;

    QFile file(projectFile);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Project file"),tr("Cannot write file %1:\n%2.").arg(projectFile).arg(file.errorString()));
        return;
    }

    getFields();
    std::ofstream outfile(projectFile.toStdString().c_str());
    outfile<<jsonData;
    outfile.close();

    QString title =  QCoreApplication::organizationName()+tr(" - ")+projectFile;
    setWindowTitle(title);

    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::on_toolButton_BC_apply_clicked()
{
    int row0 = ui->tableWidget_BC->rowCount();
    int row = ui->spinBox_nbBC->value();
    ui->tableWidget_BC->setRowCount(row);
    if(row!=0)
    {
        row0=(row0<row)?row0:row;
        for(int i=row0;i<row;i++)
        {
            QComboBox *cb = new QComboBox;
            cb->addItems(listBC);
            cb->setStyleSheet("QComboBox {font: bold "+QString::number(12)+"px;}");
            ui->tableWidget_BC->setCellWidget(i,1,cb);
            QAbstractItemModel *model = ui->tableWidget_BC->model();
            model->setData(model->index(i, 1), QColor(0, 255, 0, 50), Qt::BackgroundRole);
            model->setData(model->index(i, 0), QColor(0, 255, 0, 50), Qt::BackgroundRole);
        }
    }
}

void MainWindow::on_toolButton_Init_apply_clicked()
{
    int row0 = ui->tableWidget_Init->rowCount();
    int row = ui->spinBox_nbInit->value();
    ui->tableWidget_Init->setRowCount(row);

    if(row!=0)
    {
        row0=(row0<row)?row0:row;

        for(int i=row0;i<row;i++)
        {
            //            QTableWidgetItem *item = new QTableWidgetItem("initialCondtition"+QString::number(i+1));
            //            item->setFlags(item->flags() ^ Qt::ItemIsEditable); // set item not editable
            //            ui->tableWidget_Init->setItem(i,0,item);
            QComboBox *cb = new QComboBox;
            cb->addItems(listInit);
            cb->setStyleSheet("QComboBox {font: bold "+QString::number(12)+"px;}");
            ui->tableWidget_Init->setCellWidget(i,0,cb); //type

            ui->tableWidget_Init->setItem(i,1, new QTableWidgetItem("0.0")); //X
            ui->tableWidget_Init->setItem(i,2, new QTableWidgetItem("0.0")); //Y
            ui->tableWidget_Init->setItem(i,3, new QTableWidgetItem("0.0")); //Z
            ui->tableWidget_Init->setItem(i,4, new QTableWidgetItem("1.0")); //Size
            ui->tableWidget_Init->setItem(i,5, new QTableWidgetItem("1.0")); //Amplitude

            QAbstractItemModel *model = ui->tableWidget_Init->model();
            for(int j=0;j<ui->tableWidget_Init->columnCount();j++)
                model->setData(model->index(i, j), QColor(0, 255, 0, 50), Qt::BackgroundRole);
        }

    }
    ui->tableWidget_Init->show();
}


void MainWindow::on_toolButton_Obs_apply_clicked()
{
    int row0 = ui->tableWidget_Obs->rowCount();
    int row = ui->spinBox_nbObs->value();

    ui->tableWidget_Obs->setRowCount(row);

    if(row!=0)
    {
        row0=(row0<row)?row0:row;
        for(int i=row0;i<row;i++)
        {
            //            QTableWidgetItem *item = new QTableWidgetItem("observer"+QString::number(i+1));
            //            item->setFlags(item->flags() ^ Qt::ItemIsEditable); // set item not editable
            //            ui->tableWidget_Obs->setItem(i,0,item);
            ui->tableWidget_Obs->setItem(i,0, new QTableWidgetItem("0.0")); //X
            ui->tableWidget_Obs->setItem(i,1, new QTableWidgetItem("0.0")); //Y
            ui->tableWidget_Obs->setItem(i,2, new QTableWidgetItem("0.0")); //Z
            ui->tableWidget_Obs->setItem(i,3, new QTableWidgetItem("0.0")); //size

            QAbstractItemModel *model = ui->tableWidget_Obs->model();
            for(int j=0;j<ui->tableWidget_Obs->columnCount();j++)
                model->setData(model->index(i, j), QColor(0, 255, 0, 50), Qt::BackgroundRole);
        }

    }
    ui->tableWidget_Obs->show();
}


void MainWindow::on_toolButton_Src_apply_clicked()
{
    int row0 = ui->tableWidget_Src->rowCount();
    int row = ui->spinBox_nbSrc->value();
    ui->tableWidget_Src->setRowCount(row);

    if(row!=0)
    {
        row0=(row0<row)?row0:row;
        for(int i=row0;i<row;i++)
        {
            //            QTableWidgetItem *item = new QTableWidgetItem("source"+QString::number(i+1));
            //            item->setFlags(item->flags() ^ Qt::ItemIsEditable); // set item not editable
            //            ui->tableWidget_Src->setItem(i,0,item);
            QComboBox *cb = new QComboBox;
            cb->addItems(listSrc);
            cb->setStyleSheet("QComboBox {font: bold "+QString::number(12)+"px;}");
            ui->tableWidget_Src->setCellWidget(i,0,cb);
            ui->tableWidget_Src->setItem(i,1, new QTableWidgetItem("")); //formula/data file name
            ui->tableWidget_Src->setItem(i,2, new QTableWidgetItem("0.0")); //X
            ui->tableWidget_Src->setItem(i,3, new QTableWidgetItem("0.0")); //Y
            ui->tableWidget_Src->setItem(i,4, new QTableWidgetItem("0.0")); //Z
            ui->tableWidget_Src->setItem(i,5, new QTableWidgetItem("0.0")); //Size
            ui->tableWidget_Src->setItem(i,6, new QTableWidgetItem("")); //amplitude
            ui->tableWidget_Src->setItem(i,7, new QTableWidgetItem("")); //Frequency
            ui->tableWidget_Src->setItem(i,8, new QTableWidgetItem("")); //Phase
            ui->tableWidget_Src->setItem(i,9, new QTableWidgetItem(ui->lineEdit_timeEnd->text())); //Duration

            QAbstractItemModel *model = ui->tableWidget_Src->model();
            for(int j=0;j<ui->tableWidget_Src->columnCount();j++)
                model->setData(model->index(i, j), QColor(0, 255, 0, 50), Qt::BackgroundRole);
        }

    }
    ui->tableWidget_Src->show();
}


void MainWindow::on_commandLinkButton_save_clicked()
{
    if(projectFile!="")
    {
        getFields();
        std::ofstream outfile(projectFile.toStdString().c_str());
        outfile<<jsonData;
        outfile.close();
    }else
        on_action_Save_triggered();
}


void MainWindow::on_actionRun_triggered()
{
    std::string command;

    command=ui->lineEdit_solver->text().toStdString()+" "+projectFile.toStdString()+"\r";
    ui->termwidget->sendText(tr(command.c_str()));
    timer->start(500);


    for(size_t i=0;i<nb_series;i++)
    {
        series[i]->clear();
    }

    ui->toolButton_res_pause->setVisible(true);
    m_stop=ui->toolButton_res_pause->isChecked();

    // chart->setTitle("Convergence rates");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->gridLayout_chart->addWidget(chartView,1,1);

    set_watcher_pos_zero();

    save_config();

    on_watcher();

    statusBar()->showMessage(tr("Reading file"));
}

void MainWindow::on_toolButton_Load_solver_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Solver File"),QDir::currentPath(),tr("Files (*)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Solver"),tr("Cannot read solver file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }

    ui->lineEdit_solver->setText(fileName);

    statusBar()->showMessage(tr("Mesh loaded"), 2000);
}

void MainWindow::on_action_View_results_triggered()
{
    std::string command;
    command="paraview results.pvd\r";
    ui->termwidget->sendText(tr(command.c_str()));
}

void MainWindow::on_toolButton_play_sound_clicked(bool checked)
{
    int observer_id=ui->tableWidget_Obs->currentRow()+1;
    QString const obs_snd_file = "observer_"+QString::number(observer_id)+".wav";

    if(ui->toolButton_play_sound->isEnabled())
    {
        ui->toolButton_play_sound->setEnabled(false);
        ui->toolButton_stop_sound->setEnabled(true);
    }

    if(observer_id!=0)
    {
        QSound::play(obs_snd_file);
    }
    else
    {
        QMessageBox::warning(this,"Warning","Select a valid item...");
    }
}

void MainWindow::on_toolButton_stop_sound_clicked()
{
    if(ui->toolButton_stop_sound->isEnabled())
    {
        ui->toolButton_play_sound->setEnabled(true);
        ui->toolButton_stop_sound->setEnabled(false);
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::drawChart(vector<double> vec)
{
    for(size_t i=1;i<nb_series;i++)
    {
        vec[i] = (vec[i]>0)?abs(log10(vec[i])):0;
        series[i]->append(vec[0], vec[i]);
        series[i]->append(vec[0], vec[i]);
        max_val=max(max_val,vec[i]);
        min_val=min(min_val,vec[i]);
    }

    axisX->setRange(0,vec[0]);
    axisY->setRange(min_val, max_val);
    QCoreApplication::processEvents();
    chart->update();
}

void MainWindow::save_config()
{
    ofstream outfile("init.conf");
    outfile<<filename<<endl;
    outfile.close();
}

void MainWindow::load_config()
{
    ifstream infile("init.conf");
    if(infile.is_open())
    {
      string str;
      infile>>str;

      filename = str;
      ui->lineEdit_res_file->setText(QString::fromStdString(filename));

      setWindowTitle(tr("DGFEM-CAA-GUI - ")+QString::fromStdString(filename));
    }
    infile.close();
}

void MainWindow::on_watcher()
{
    m_watcher = new QFileSystemWatcher();
    m_watcher->addPath(QString::fromStdString(filename));
    max_val=-99999.9;
    min_val= 99999.9;

    for(size_t i=0;i<nb_series;i++)
    {
        series[i]->clear();
    }

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->gridLayout_chart->addWidget(chartView,1,1);

    save_config();

    QFile file(QString::fromStdString(filename));
    file.close();
    connect(m_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(onFileChanged(const QString&)));
}

void MainWindow::onFileChanged( const QString& file_name)
{
    QFile file(QString::fromStdString(filename));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cerr<<"Fail to open data file..."<<endl;

        QMessageBox msgBox;
        msgBox.setText("Fail to open data file...\nPlease load a new data file");
        msgBox.exec();

        set_watcher_pos_zero();
        file.close();
        return;
    }

    QTextStream in(&file);
    in.seek(m_position);

    QString qline;

    if(m_position==0)
    {
        in.readLineInto(&qline);
        cerr<<qline.toStdString()<<endl;
        m_position = in.pos();
    }

    // m_stop=false;

    while (in.readLineInto(&qline)&&!m_stop)
    {

        m_size = file.size();

        if(m_position!=0)
        {
            qline.replace(";"," ");

            string line = qline.toStdString();

            vector<double> vec;
            stringstream ss( line );
            double f;
            while (ss >> f)
                vec.push_back(f);

            drawChart(vec);
            chart->update();
        }
        m_position = in.pos();

        if(m_size == file.size())
        {
            file.close();
            return;
        }
    }

    file.close();
}

void MainWindow::on_toolButton_load_res_file_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Residuals"), "",
                                                    tr("Data file (*.txt);;(*.dat);;All Files (*)"));

    filename = fileName.toStdString();
    ui->lineEdit_res_file->setText(fileName);
    save_config();

    setWindowTitle(tr("DGFEM-CAA-GUI - ")+fileName);
}

void MainWindow::on_toolButton_res_pause_clicked(bool checked)
{
    m_stop = checked;
}

