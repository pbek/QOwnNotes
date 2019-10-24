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
           $$PWD/loader_p.h \
           $$PWD/core_debug.h \
           $$PWD/backgroundchecker.h \
           $$PWD/backgroundchecker_p.h \
           $$PWD/sonnetcore_export.h

# Sonnet Plugins
include($$PWD/../plugins/hunspell/hunspell.pri)
macx: include($$PWD/../plugins/nsspellchecker/nsspellchecker.pri)


DEFINES += SONNETCORE_EXPORT=""
DEFINES += INSTALLATION_PLUGIN_PATH=""
#DEFINES += SONNET_STATIC

unix:system("touch sonnetcore_export.h")
win32:system("type nul > sonnetcore_export.h")

# Copy trigrams.map to build dir
defineTest(copyToDestDir) {
    files = $$1
    message
    for(FILE, files) {
        DDIR = $$OUT_PWD
                    FILE = $$absolute_path($$FILE)
        message($$FILE)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

copyToDestDir(trigrams.map)

