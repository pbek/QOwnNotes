#TARGET = sonnet-hunspell
#TEMPLATE = lib
#CONFIG += staticlib
QT -= gui

#include($$PWD/hunspell/hunspell_lib.pri)

SOURCES += $$PWD/aspelldict.cpp \
           $$PWD/aspellclient.cpp \

HEADERS += $$PWD/aspellclient.h \
           $$PWD/aspelldict.h

DEFINES += SONNETUI_EXPORT=""
#DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC

INCLUDEPATH += ../../core
INCLUDEPATH += $$PWD
#INCLUDEPATH += hunspell/src/hunspell
#INCLUDEPATH += hunspell/src/

#DEFINES += HUNSPELL_DEBUG_ON

LIBS += -laspell


