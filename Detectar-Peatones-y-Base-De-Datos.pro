#-------------------------------------------------
#
# Project created by QtCreator - Riclab.com
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Detectar-Peatones-y-Base-De-Datos
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv-3.1.0/

INCLUDEPATH += /usr/local/include/bsoncxx/v_noabi/
INCLUDEPATH += /usr/local/include/mongocxx/v_noabi/

LIBS += `pkg-config opencv libmongocxx --libs  --cflags` -L/usr/local/lib

SOURCES +=  main.cpp\
            mainwindow.cpp \
            mythread.cpp

HEADERS  += mainwindow.h \
            mythread.h

FORMS    += mainwindow.ui
