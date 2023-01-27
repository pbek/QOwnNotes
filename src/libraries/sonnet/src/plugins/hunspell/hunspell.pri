#TARGET = sonnet-hunspell
#TEMPLATE = lib
#CONFIG += staticlib
QT -= gui
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += core5compat
}

include($$PWD/hunspell/hunspell_lib.pri)

SOURCES += $$PWD/hunspelldict.cpp \
           $$PWD/hunspellclient.cpp \
           $$PWD/hunspelldebug.cpp

HEADERS += $$PWD/hunspellclient.h \
           $$PWD/hunspelldebug.h \
           $$PWD/hunspelldict.h

DEFINES += SONNETUI_EXPORT=""
#DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC

INCLUDEPATH += ../../core
INCLUDEPATH += hunspell/src/hunspell
INCLUDEPATH += hunspell/src/

DEFINES += HUNSPELL_DEBUG_ON


