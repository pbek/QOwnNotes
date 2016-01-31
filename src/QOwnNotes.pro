#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T08:31:41
#
#-------------------------------------------------

QT       += core gui sql svg network script xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QOwnNotes
TEMPLATE = app
ICON = QOwnNotes.icns
RC_FILE = QOwnNotes.rc

INCLUDEPATH += $$PWD/libraries

SOURCES += main.cpp\
        mainwindow.cpp \
    libraries/diff_match_patch/diff_match_patch.cpp \
    libraries/hoedown/html.c \
    libraries/hoedown/autolink.c \
    libraries/hoedown/buffer.c \
    libraries/hoedown/document.c \
    libraries/hoedown/escape.c \
    libraries/hoedown/html_blocks.c \
    libraries/hoedown/stack.c \
    libraries/hoedown/version.c \
    libraries/simplecrypt/simplecrypt.cpp \
    libraries/versionnumber/versionnumber.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/linkdialog.cpp \
    dialogs/notediffdialog.cpp \
    dialogs/settingsdialog.cpp \
    dialogs/tododialog.cpp \
    dialogs/trashdialog.cpp \
    dialogs/updatedialog.cpp \
    dialogs/versiondialog.cpp \
    entities/calendaritem.cpp \
    entities/note.cpp \
    entities/notehistory.cpp \
    services/owncloudservice.cpp \
    services/updateservice.cpp \
    helpers/htmlentities.cpp \
    services/databaseservice.cpp \
    libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_parser.c \
    libraries/qmarkdowntextedit/highlighter.cpp \
    libraries/qmarkdowntextedit/qmarkdowntextedit.cpp \
    libraries/qmarkdowntextedit/qtexteditsearchwidget.cpp \
    widgets/qownnotesmarkdowntextedit.cpp \
    dialogs/passworddialog.cpp \
    services/analyticsservice.cpp \
    dialogs/masterdialog.cpp

HEADERS  += mainwindow.h \
    build_number.h \
    version.h \
    libraries/diff_match_patch/diff_match_patch.h \
    libraries/hoedown/html.h \
    libraries/hoedown/autolink.h \
    libraries/hoedown/buffer.h \
    libraries/hoedown/document.h \
    libraries/hoedown/escape.h \
    libraries/hoedown/stack.h \
    libraries/hoedown/version.h \
    libraries/simplecrypt/simplecrypt.h \
    libraries/versionnumber/versionnumber.h \
    entities/notehistory.h \
    entities/note.h \
    entities/calendaritem.h \
    dialogs/aboutdialog.h \
    dialogs/linkdialog.h \
    dialogs/notediffdialog.h \
    dialogs/settingsdialog.h \
    dialogs/tododialog.h \
    dialogs/trashdialog.h \
    dialogs/updatedialog.h \
    dialogs/versiondialog.h \
    services/owncloudservice.h \
    services/updateservice.h \
    helpers/htmlentities.h \
    services/databaseservice.h \
    release.h \
    libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_definitions.h \
    libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_parser.h \
    libraries/qmarkdowntextedit/highlighter.h \
    libraries/qmarkdowntextedit/qmarkdowntextedit.h \
    libraries/qmarkdowntextedit/qtexteditsearchwidget.h \
    widgets/qownnotesmarkdowntextedit.h \
    dialogs/passworddialog.h \
    libraries/qt-google-analytics-collector/ganalytics.hpp \
    services/analyticsservice.h \
    dialogs/masterdialog.h

FORMS    += mainwindow.ui \
    dialogs/notediffdialog.ui \
    dialogs/aboutdialog.ui \
    dialogs/updatedialog.ui \
    dialogs/settingsdialog.ui \
    dialogs/versiondialog.ui \
    dialogs/trashdialog.ui \
    dialogs/linkdialog.ui \
    dialogs/tododialog.ui \
    dialogs/passworddialog.ui

RESOURCES += \
    libraries/qmarkdowntextedit/media.qrc \
    images.qrc \
    texts.qrc \
    breeze.qrc \
    demonotes.qrc \
    high-contrast.qrc \
    ubuntu-mobile.qrc \
    adwaita.qrc

CONFIG(debug, debug|release) {
#    QMAKE_CXXFLAGS_DEBUG += -g3 -O0
    message("Currently in DEBUG mode.")
} else {
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    message("Currently in RELEASE mode.")
}
