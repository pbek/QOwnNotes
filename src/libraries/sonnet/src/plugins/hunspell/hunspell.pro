TARGET = sonnet-hunspell
TEMPLATE = lib
CONFIG += staticlib
QT -= gui

SOURCES += hunspelldict.cpp \
           hunspellclient.cpp \
           hunspelldebug.cpp
HEADERS += hunspellclient.h

DEFINES += SONNETUI_EXPORT=""
DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC

INCLUDEPATH += ../../core
INCLUDEPATH += ../../../../hunspell/src


