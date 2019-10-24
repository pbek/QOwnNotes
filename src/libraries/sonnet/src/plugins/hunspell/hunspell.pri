#TARGET = sonnet-hunspell
#TEMPLATE = lib
#CONFIG += staticlib
QT -= gui

SOURCES += $$PWD/hunspelldict.cpp \
           $$PWD/hunspellclient.cpp \
           $$PWD/hunspelldebug.cpp

HEADERS += $$PWD/hunspellclient.h \
           $$PWD/hunspelldebug.h

include($$PWD/hunspell/hunspell_lib.pri)

DEFINES += SONNETUI_EXPORT=""
DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC

INCLUDEPATH += ../../core
#INCLUDEPATH += hunspell/src/hunspell
#INCLUDEPATH += hunspell/src/


