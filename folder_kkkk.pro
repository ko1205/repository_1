#-------------------------------------------------
#
# Project created by QtCreator 2017-04-18T19:28:14
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mkdirTree
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    templateview.cpp \
    preview.cpp \
    propertyview.cpp\
    common.cpp \
    templatecontrol.cpp

HEADERS  += mainwindow.h \
    templateview.h \
    preview.h \
    propertyview.h\
    common.h \
    templatecontrol.h
