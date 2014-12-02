#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T08:31:41
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QOwnNotes
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    note.cpp

HEADERS  += mainwindow.h \
    note.h

FORMS    += mainwindow.ui
