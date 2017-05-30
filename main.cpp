/****************************************************************************
**
** Copyright (C) 2016 Michael Geschwill
** Im Kleinen Eschle 21
** 78054 Villingen-Schwenningen
** hpmacher@gmail.com
**
****************************************************************************/

#include "mainwindow.h"
#include <QApplication>
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList libraryPaths = QStringList();
    libraryPaths.append(qApp->applicationDirPath());
    libraryPaths.append(qApp->applicationDirPath().append("/Plugins/platforms"));
    qApp->setLibraryPaths(libraryPaths);

    MainWindow w;
    w.show();

    return a.exec();
}
