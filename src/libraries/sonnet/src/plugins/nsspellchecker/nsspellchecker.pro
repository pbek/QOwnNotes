TARGET = sonnet-nsspellchecker
TEMPLATE = lib
CONFIG += staticlib
QT -= gui

OBJECTIVE_SOURCES += nsspellcheckerdict.mm \
           nsspellcheckerclient.mm
HEADERS += nsspellcheckerclient.h

DEFINES += SONNETUI_EXPORT=""
DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC

INCLUDEPATH += ../../core


