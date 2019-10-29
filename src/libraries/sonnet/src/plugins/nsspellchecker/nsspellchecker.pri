#TARGET = sonnet-nsspellchecker
#TEMPLATE = lib
#CONFIG += staticlib
QT -= gui

LIBS += -framework AppKit

OBJECTIVE_SOURCES += $$PWD/nsspellcheckerdict.mm \
           $$PWD/nsspellcheckerclient.mm
HEADERS += $$PWD/nsspellcheckerclient.h \
           $$PWD/nsspellcheckerdebug.h \
           $$PWD/nsspellcheckerdict.h 

DEFINES += SONNETUI_EXPORT=""
#DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC

INCLUDEPATH += ../../core

SOURCES += \
    $$PWD/nsspellcheckerdebug.cpp


