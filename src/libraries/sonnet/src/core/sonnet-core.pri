INCLUDEPATH += $$PWD
#TARGET = sonnet-core
#TEMPLATE = lib
#CONFIG += staticlib c++11
CONFIG += c++11
QT -= gui

#Sonnet Core
SOURCES += $$PWD/loader.cpp \
           $$PWD/client.cpp \
           $$PWD/spellerplugin.cpp \
           $$PWD/speller.cpp \
           $$PWD/settings.cpp \
           $$PWD/backgroundchecker.cpp \
           $$PWD/guesslanguage.cpp \
           $$PWD/textbreaks.cpp \
           $$PWD/tokenizer.cpp \
           $$PWD/languagefilter.cpp \
           $$PWD/core_debug.cpp

HEADERS += $$PWD/client_p.h \
           $$PWD/guesslanguage.h \
           $$PWD/languagefilter_p.h \
           $$PWD/loader_p.h \
           $$PWD/core_debug.h \
           $$PWD/backgroundchecker.h \
           $$PWD/backgroundchecker_p.h \
           $$PWD/settings_p.h \
           $$PWD/speller.h \
           $$PWD/spellerplugin_p.h \
           $$PWD/textbreaks_p.h \
           $$PWD/tokenizer_p.h
#           $$PWD/sonnetcore_export.h

# Sonnet Plugins
include($$PWD/../plugins/hunspell/hunspell.pri)

unix:!macx {
    include($$PWD/../plugins/aspell/aspell.pri)
}

#macx {
#    include($$PWD/../plugins/nsspellchecker/nsspellchecker.pri)
#}


#DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
DEFINES += SONNET_STATIC
