include($$PWD/fakevim/utils/utils.pri)

INCLUDEPATH += $$PWD

SOURCES += $$PWD/fakevim/fakevimhandler.cpp \
           $$PWD/fakevim/fakevimactions.cpp

HEADERS += $$PWD/fakevim/fakevimhandler.h \
           $$PWD/fakevim/fakevimactions.h

CONFIG += qt
QT += widgets
