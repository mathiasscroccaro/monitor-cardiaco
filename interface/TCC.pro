#-------------------------------------------------
#
# Project created by QtCreator 2016-08-18T07:43:08
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport

TARGET = TCC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        qcustomplot.cpp \
    uart.cpp \
    filtro.cpp

HEADERS  += mainwindow.h \
            qcustomplot.h \
    uart.h \
    filtro.h

FORMS    += mainwindow.ui
