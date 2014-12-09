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
    note.cpp \
    libraries/peg-markdown-highlight/example_qt/qthighlight/pmh_parser.c \
    libraries/peg-markdown-highlight/example_qt/qthighlight/highlighter.cpp

HEADERS  += mainwindow.h \
    note.h \
    libraries/peg-markdown-highlight/example_qt/qthighlight/pmh_parser.h \
    libraries/peg-markdown-highlight/example_qt/qthighlight/pmh_definitions.h \
    libraries/peg-markdown-highlight/example_qt/qthighlight/highlighter.h

FORMS    += mainwindow.ui
