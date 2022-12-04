#include "mainwindow.h"
#include "./ui_mainwindow.h"

QStringList listBC={"Absorbing","Reflecting"};
QStringList listInit={"gaussian"};
QStringList listSrc={"monopole","dipole","quadrupole","formula","file"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initTerminal();

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

    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getFields()
{
//    jsonData["configurationFile"]="file.conf";
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
//    ui->textEdit_config->setText(ReadFile.readAll());

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
    //m_position=0;
//    ui->termwidget->sendText(tr("clear &"));
    command=ui->lineEdit_solver->text().toStdString()+" "+projectFile.toStdString()+"\r";
    ui->termwidget->sendText(tr(command.c_str()));
}


void MainWindow::on_toolButton_Load_solver_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Splver File"),QDir::currentPath(),tr("Files (*)"));

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

