QT       += core gui


QMAKE_RPATHDIR += @loader_path/../Frameworks

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DependencyChaos
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    dependlistmodel.cpp \
    dependlistnode.cpp \
    worker.cpp \
    idmodform.cpp \
    dependform.cpp

HEADERS  += mainwindow.h \
    dependlistmodel.h \
    dependlistnode.h \
    worker.h \
    idmodform.h \
    dependform.h

FORMS    += mainwindow.ui \
    idmodform.ui \
    dependform.ui

DISTFILES +=

RESOURCES += \
    imgres.qrc
