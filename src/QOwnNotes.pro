#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T08:31:41
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QOwnNotes
TEMPLATE = app
ICON = QOwnNotes.icns
RC_FILE = QOwnNotes.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    note.cpp \
    libraries/markdown-highlight/pmh_parser.c \
    libraries/markdown-highlight/highlighter.cpp \
    libraries/diff_match_patch/diff_match_patch.cpp \
    notediffdialog.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    note.h \
    build_number.h \
    libraries/markdown-highlight/pmh_parser.h \
    libraries/markdown-highlight/pmh_definitions.h \
    libraries/markdown-highlight/highlighter.h \
    libraries/diff_match_patch/diff_match_patch.h \
    notediffdialog.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    notediffdialog.ui \
    aboutdialog.ui

RESOURCES += \
    images.qrc \
    texts.qrc
