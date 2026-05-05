
QT       += core gui testlib sql widgets xml network qml printsupport concurrent

#QT       -= gui

TARGET = tests
TEMPLATE = app

CONFIG += debug
CONFIG += c++11

PROJECT_ROOT = $$PWD/../..//
SRC_DIR = $$PROJECT_ROOT/src//

include($$SRC_DIR/libraries/qtkeychain/qtkeychain.pri)
lessThan(QT_VERSION, 5.15.0) {
    # qtkeychain references QDataStream::Qt_5_15, which is unavailable in legacy Qt 5 builds.
    DEFINES += Qt_5_15=Qt_5_0
}

#HEADERS += \
#    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/basetestcase.h \
#    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/mocks/*.h \

SOURCES += \
#    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/basetestcase.cpp \
    $$PWD/main.cpp \


#RESOURCES += \
#    $$SRC_DIR/resources/rdm.qrc

# doesn't seem to work on AppVeyor
#include($$PWD/../../src/libraries/sonnet/src/core/sonnet-core.pri)

INCLUDEPATH += \
#    $$SRC_DIR/modules/ \
    $$SRC_DIR/ \
    $$SRC_DIR/libraries/qtkeychain \
    $$PWD/ \
#    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/

DEFINES += INTEGRATION_TESTS
DEFINES += ELPP_STL_LOGGING ELPP_DISABLE_DEFAULT_CRASH_HANDLING

#TEST CASES
include($$PWD/testcases/app/app-tests.pri)
#############
#LIBS += -lz
#include($$PROJECT_ROOT/3rdparty/3rdparty.pri)

release: DESTDIR = $$PROJECT_ROOT/bin/tests
debug:   DESTDIR = $$PROJECT_ROOT/bin/tests

unix:!mac {
    #code coverage
    QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0
    QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0
    LIBS += -lgcov
}

UI_DIR = $$DESTDIR/ui
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/obj
RCC_DIR = $$DESTDIR/obj
