/****************************************************************************
**
** Copyright (C) 2016 Michael Geschwill
** Im Kleinen Eschle 21
** 78054 Villingen-Schwenningen
** hpmacher@gmail.com
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dependlistmodel.h"
#include <QProcess>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    DependListModel *dependListModel;
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void on_action_ffnen_triggered();
    void on_treeView_clicked(const QModelIndex &index);
    void workdone(QString resultstr);
    void errorString(QString errStr);
    void progdone(int current, int from);
    void infomessage(QString what);

    void on_actionFix_ID_triggered();
    void on_actionFix_Dependency_triggered();
    void on_actionSet_current_rpath_triggered();
    void on_treeView_doubleClicked(const QModelIndex &);
    void on_actionRefresh_triggered();

private:
    QString currentrpath;
    QModelIndex currentindex;
    QStringList *retList;
    Ui::MainWindow *ui;
    QStringList *lastFileList;
    void disen(bool what);
    void reload();
};

#endif // MAINWINDOW_H
