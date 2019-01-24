#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T13:43:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lab20
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    graphicsscene.cpp \
    sender.cpp \
    receiver.cpp \
    interface.cpp

HEADERS  += window.h \
    graphicsscene.h \
    sender.h \
    receiver.h \
    interface.h

FORMS    += window.ui

LIBS += -L/usr/local/lib -lwiringPi -lpthread
