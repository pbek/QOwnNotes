TARGET = sonnet-core
TEMPLATE = lib
CONFIG += staticlib c++11
QT -= gui

SOURCES += loader.cpp \
           client.cpp \
           spellerplugin.cpp \
           speller.cpp \
           settings.cpp \
           backgroundchecker.cpp \
           guesslanguage.cpp \
           textbreaks.cpp \
           tokenizer.cpp \
           languagefilter.cpp

HEADERS += client_p.h \
           loader_p.h

DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC

unix:system("touch sonnetcore_export.h")
win32:system("type nul > sonnetcore_export.h")

