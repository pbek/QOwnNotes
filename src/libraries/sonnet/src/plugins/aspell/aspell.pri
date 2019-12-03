QT -= gui

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

DEFINES += ASPELL_DEBUG_ON

