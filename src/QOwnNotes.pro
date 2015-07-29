#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T08:31:41
#
#-------------------------------------------------

QT       += core gui sql svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QOwnNotes
TEMPLATE = app
ICON = QOwnNotes.icns
RC_FILE = QOwnNotes.rc

INCLUDEPATH += $$PWD/libraries

SOURCES += main.cpp\
        mainwindow.cpp \
    note.cpp \
    libraries/markdown-highlight/pmh_parser.c \
    libraries/markdown-highlight/highlighter.cpp \
    libraries/diff_match_patch/diff_match_patch.cpp \
    libraries/hoedown/html.c \
    libraries/hoedown/autolink.c \
    libraries/hoedown/buffer.c \
    libraries/hoedown/document.c \
    libraries/hoedown/escape.c \
    libraries/hoedown/html_blocks.c \
    libraries/hoedown/stack.c \
    libraries/hoedown/version.c \
    notediffdialog.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    note.h \
    build_number.h \
    version.h \
    libraries/markdown-highlight/pmh_parser.h \
    libraries/markdown-highlight/pmh_definitions.h \
    libraries/markdown-highlight/highlighter.h \
    libraries/diff_match_patch/diff_match_patch.h \
    libraries/hoedown/html.h \
    libraries/hoedown/autolink.h \
    libraries/hoedown/buffer.h \
    libraries/hoedown/document.h \
    libraries/hoedown/escape.h \
    libraries/hoedown/stack.h \
    libraries/hoedown/version.h \
    notediffdialog.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    notediffdialog.ui \
    aboutdialog.ui

RESOURCES += \
    images.qrc \
    texts.qrc \
    breeze.qrc

CONFIG(debug, debug|release) {
#    QMAKE_CXXFLAGS_DEBUG += -g3 -O0
    message("Currently in DEBUG mode.")
} else {
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    message("Currently in RELEASE mode.")
}
