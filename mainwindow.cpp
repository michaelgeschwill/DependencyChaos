/****************************************************************************
**
** Copyright (C) 2016 Michael Geschwill
** Im Kleinen Eschle 21
** 78054 Villingen-Schwenningen
** hpmacher@gmail.com
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "worker.h"
#include "idmodform.h"
#include "dependform.h"
#include <QFileDialog>
#include <QListView>
#include <QMessageBox>

// Main constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    currentrpath = "";
    lastFileList = new QStringList();
    retList = new QStringList();
    this->setUnifiedTitleAndToolBarOnMac(true);
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
    qApp->setAttribute(Qt::AA_DontShowIconsInMenus, true);

    ui->setupUi(this);

    ui->progressFrame->setVisible(false);
    ui->treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setSelectionMode(QTreeView::SingleSelection);
    ui->treeView->setSelectionBehavior(QTreeView::SelectRows);


    dependListModel = new DependListModel();
    ui->treeView->setModel(dependListModel);

    ui->actionSearch_and_replace_in_depend->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QString("configs/config.ini"), QSettings::IniFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event){
    QSettings settings(QString("configs/config.ini"), QSettings::IniFormat);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    QMainWindow::showEvent(event);
}

void MainWindow::workdone(QString resultstr){

    QStringList r = resultstr.split("\n");
    dependListModel->layoutAboutToBeChanged();

    retList->clear();
    foreach(QString rr, r)
        retList->append(rr);

    dependListModel->setupModelData(retList);
    dependListModel->layoutChanged();

    for(int i = 0; i < ui->treeView->model()->columnCount(); i++)
        ui->treeView->resizeColumnToContents(i);

    disen(true);
}

void MainWindow::errorString(QString errStr){
    qDebug() << errStr;
    disen(true);
}

void MainWindow::progdone(int current, int from)
{
    ui->progressBar->setMaximum(from);
    ui->progressBar->setValue(current);
}

void MainWindow::infomessage(QString what)
{
    ui->LogTextFrame->moveCursor (QTextCursor::End);
    ui->LogTextFrame->insertPlainText(QDateTime::currentDateTime().toString("HH:mm:ss") + " - " + what + "\r\n");
}

void MainWindow::disen(bool what)
{
    ui->actionSearch_and_replace_in_depend->setEnabled(what);
    ui->action_ffnen->setEnabled(what);
    ui->treeView->setEnabled(what);
    ui->actionSet_current_rpath->setEnabled(what);
    ui->actionRefresh->setEnabled(what);
    ui->progressFrame->setVisible(!what);
}

void MainWindow::on_action_ffnen_triggered()
{
    QSettings settings(QString("configs/config.ini"), QSettings::IniFormat);
    QString currentsearchpath = settings.value("currentsearchpath", "").toString();

    QFileDialog o;
    QStringList fileToOpen = o.getOpenFileNames(this, "Open file", currentsearchpath, "All libs (*.dylib *.framework *.app);;Dylibs (*.dylib);;Frameworks (*.framework);;Apps (*.app);;All files (*.*)");
    if (fileToOpen.count()==0)
        return;

    if (fileToOpen.count()==1 && QFileInfo(fileToOpen.at(0)).completeSuffix().toLower()=="app"){
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowModality(Qt::WindowModal);
        msgBox->setText("App bundle detected");
        msgBox->setInformativeText("Do you want browse in Contents of the selected App? Click no to use App as File and scan his dependencies.");
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox->setDefaultButton(QMessageBox::Yes);
        msgBox->setIconPixmap(QPixmap(":/imglist/img/Open-folder-info.png"));
        int ret = msgBox->exec();
        if (ret == QMessageBox::Yes){
            fileToOpen = o.getOpenFileNames(this, "Open file", fileToOpen.at(0) + "/Contents", "All libs (*.dylib *.framework *.app);;Dylibs (*.dylib);;Frameworks (*.framework);;Apps (*.app);;All files (*.*)");
        } else if (ret == QMessageBox::Cancel){
            return;
        }
        msgBox->deleteLater();
    }
    if (!fileToOpen.empty()){
        disen(false);
        settings.setValue("currentsearchpath",QFileInfo(fileToOpen.at(0)).absolutePath());
        lastFileList->clear();
        for(int i=0; i<fileToOpen.count(); i++)
            lastFileList->append(fileToOpen.at(i));
        reload();
    }
}

void MainWindow::reload(){
    QThread* thread = new QThread;
    Worker *worker = new Worker(lastFileList,currentrpath);
    worker->moveToThread(thread);
    connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(worker, SIGNAL(progress(int,int)), this, SLOT(progdone(int,int)));
    connect(worker, SIGNAL(infomessage(QString)), this, SLOT(infomessage(QString)));
    connect(worker, SIGNAL(finished(QString)), this, SLOT(workdone(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));

    connect(worker, SIGNAL(finished(QString)), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    disen(false);
    thread->start();
}

// On change selection set currentindex
void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->actionFix_Dependency->setEnabled(false);
    ui->actionFix_ID->setEnabled(false);
    ui->actionSearch_and_replace_in_depend->setEnabled(false);
    ui->actionCheck_selected->setEnabled(false);

    if (!index.isValid())
        return;

    // Set currentindex to column 0
    currentindex = ui->treeView->model()->index(index.row(),0,index.parent());

    if (static_cast<DependListModel*>(ui->treeView->model())->hasChildren(currentindex))
    {
        ui->actionFix_ID->setEnabled(true);
    } else {
        ui->actionFix_Dependency->setEnabled(true);
        ui->actionCheck_selected->setEnabled(true);
    }
}

// Action fix id with install_name_tool
void MainWindow::on_actionFix_ID_triggered()
{
    if (ui->treeView->selectionModel()->selectedRows().count() == 0)
        return;

    if (ui->treeView->selectionModel()->selectedRows().count() == 1){
        QString fileId = static_cast<DependListModel*>(ui->treeView->model())->getData(currentindex,1).toString();
        auto idform = new IdModForm(fileId,this);

        idform->setWindowModality(Qt::WindowModality::WindowModal);
        idform->exec();

        if (idform->result() == QDialog::Accepted){
            QProcess q;
            QString fileName = static_cast<DependListModel*>(ui->treeView->model())->getData(ui->treeView->selectionModel()->currentIndex(),4).toString();
            q.start("install_name_tool",QStringList() << "-id" << idform->getId() << fileName);
            q.waitForFinished(5000);
            QString erg = q.readAllStandardOutput();
            if (erg.trimmed()==""){
                QString info = "Success: install_name_tool -id " + idform->getId() + " " + fileName;
                ui->statusBar->showMessage(info);
                ui->treeView->model()->layoutAboutToBeChanged();
                static_cast<DependListModel*>(ui->treeView->model())->setMyData(currentindex,1,idform->getId() );
                ui->treeView->model()->layoutChanged();
            }
            q.close();
        }

        idform->deleteLater();
        return;
    }
}

// Action fix dependency with install_name_tool
void MainWindow::on_actionFix_Dependency_triggered()
{
    QString fileId = static_cast<DependListModel*>(ui->treeView->model())->getData(currentindex,0).toString();
    QString baseFile = static_cast<DependListModel*>(ui->treeView->model())->getData(currentindex,5).toString();
    auto depform = new DependForm(fileId,this);

    depform->setWindowModality(Qt::WindowModality::WindowModal);
    depform->exec();

    if (depform->result() == QDialog::Accepted){
        QProcess q;
        q.start("install_name_tool",QStringList() << "-change" << fileId << depform->getDep() << baseFile);
        q.waitForFinished(5000);
        QString erg = q.readAllStandardOutput();
        if (erg.trimmed()==""){
            QString info = "Success: install_name_tool -change " + fileId + " " + depform->getDep() + " " + baseFile;
            ui->statusBar->showMessage(info);
            ui->treeView->model()->layoutAboutToBeChanged();
            static_cast<DependListModel*>(ui->treeView->model())->setMyData( currentindex,0,depform->getDep() );
            ui->treeView->model()->layoutChanged();
        }
        q.close();
    }

    depform->deleteLater();
}

void MainWindow::on_actionSet_current_rpath_triggered()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    auto result = dialog.exec();

    if (result == QDialog::Accepted){
        currentrpath = dialog.selectedFiles().first() + "/";
    }

    ui->currentrpath->setText("rpath: " + currentrpath);
    dialog.deleteLater();
    reload();
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &)
{
    if (!static_cast<DependListModel*>(ui->treeView->model())->hasChildren(currentindex))
        on_actionFix_Dependency_triggered();
}


// Action for reload last selected files
void MainWindow::on_actionRefresh_triggered()
{
    reload();
}

void MainWindow::on_actionCheck_selected_triggered()
{
    QString selectedFile = static_cast<DependListModel*>(ui->treeView->model())->getData(currentindex,4).toString();
    disen(false);
    lastFileList->clear();
    lastFileList->append(selectedFile);
    reload();
}
