#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T08:31:41
#
#-------------------------------------------------

QT       += core gui widgets sql svg network xml xmlpatterns printsupport qml websockets

# quick is enabled for more scripting options
# Windows and macOS seem to ignore that
#QT       += quick

# Sonnet support if we can get it to run in the future
#QT += SonnetCore SonnetUi

TARGET = QOwnNotes
TEMPLATE = app
ICON = QOwnNotes.icns
RC_FILE = QOwnNotes.rc
TRANSLATIONS = languages/QOwnNotes_en.ts \
    languages/QOwnNotes_de.ts \
    languages/QOwnNotes_fr.ts \
    languages/QOwnNotes_zh_CN.ts \
    languages/QOwnNotes_zh_TW.ts \
    languages/QOwnNotes_pl.ts \
    languages/QOwnNotes_ru.ts \
    languages/QOwnNotes_es.ts \
    languages/QOwnNotes_pt_BR.ts \
    languages/QOwnNotes_pt_PT.ts \
    languages/QOwnNotes_nl.ts \
    languages/QOwnNotes_hu.ts \
    languages/QOwnNotes_ja.ts \
    languages/QOwnNotes_it.ts \
    languages/QOwnNotes_ar.ts \
    languages/QOwnNotes_uk.ts \
    languages/QOwnNotes_cs.ts \
    languages/QOwnNotes_hr.ts \
    languages/QOwnNotes_ca.ts \
    languages/QOwnNotes_sv.ts \
    languages/QOwnNotes_id.ts \
    languages/QOwnNotes_bn.ts \
    languages/QOwnNotes_tr.ts \
    languages/QOwnNotes_tl.ts \
    languages/QOwnNotes_fil.ts \
    languages/QOwnNotes_ceb.ts \
    languages/QOwnNotes_hi.ts \
    languages/QOwnNotes_hil.ts \
    languages/QOwnNotes_ur.ts \
    languages/QOwnNotes_fi.ts \
    languages/QOwnNotes_el.ts \
    languages/QOwnNotes_gl.ts \
    languages/QOwnNotes_no.ts \
    languages/QOwnNotes_da.ts \
    languages/QOwnNotes_ro.ts \
    languages/QOwnNotes_sl.ts \
    languages/QOwnNotes_sk.ts

CODECFORTR = UTF-8
CONFIG += c++11

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
    libraries/botan/botanwrapper.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/linkdialog.cpp \
    dialogs/notediffdialog.cpp \
    dialogs/settingsdialog.cpp \
    dialogs/tododialog.cpp \
    dialogs/trashdialog.cpp \
    dialogs/localtrashdialog.cpp \
    dialogs/updatedialog.cpp \
    dialogs/versiondialog.cpp \
    entities/calendaritem.cpp \
    entities/note.cpp \
    entities/trashitem.cpp \
    entities/notesubfolder.cpp \
    entities/notehistory.cpp \
    entities/notefolder.cpp \
    entities/tag.cpp \
    entities/script.cpp \
    entities/bookmark.cpp \
    services/owncloudservice.cpp \
    services/updateservice.cpp \
    helpers/htmlentities.cpp \
    helpers/clientproxy.cpp \
    helpers/toolbarcontainer.cpp \
    helpers/qownnotesmarkdownhighlighter.cpp \
    helpers/fakevimproxy.cpp \
    services/databaseservice.cpp \
    widgets/qownnotesmarkdowntextedit.cpp \
    dialogs/passworddialog.cpp \
    services/metricsservice.cpp \
    services/cryptoservice.cpp \
    services/scriptingservice.cpp \
    services/websocketserverservice.cpp \
    dialogs/masterdialog.cpp \
    utils/misc.cpp \
    utils/git.cpp \
    utils/gui.cpp \
    utils/schema.cpp \
    dialogs/welcomedialog.cpp \
    dialogs/issueassistantdialog.cpp \
    dialogs/tagadddialog.cpp \
    widgets/navigationwidget.cpp \
    widgets/notepreviewwidget.cpp \
    api/noteapi.cpp \
    api/tagapi.cpp \
    widgets/logwidget.cpp \
    widgets/combobox.cpp \
    dialogs/sharedialog.cpp \
    widgets/fontcolorwidget.cpp \
    dialogs/evernoteimportdialog.cpp \
    dialogs/orphanedimagesdialog.cpp \
    dialogs/orphanedattachmentsdialog.cpp \
    dialogs/actiondialog.cpp \
    dialogs/tabledialog.cpp \
    libraries/qtcsv/src/sources/reader.cpp \
    dialogs/notedialog.cpp \
    dialogs/filedialog.cpp \
    dialogs/scriptrepositorydialog.cpp \
    widgets/scriptsettingwidget.cpp \
    api/scriptapi.cpp \
    widgets/label.cpp \
    widgets/lineedit.cpp \
    widgets/qtexteditsearchwidget.cpp \
    widgets/scriptlistwidget.cpp \
    widgets/notefolderlistwidget.cpp \
    widgets/notetreewidgetitem.cpp \
    widgets/layoutwidget.cpp \
    dialogs/serverbookmarksimportdialog.cpp

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
    libraries/botan/botanwrapper.h \
    entities/notehistory.h \
    entities/note.h \
    entities/trashitem.h \
    entities/notesubfolder.h \
    entities/calendaritem.h \
    entities/notefolder.h \
    entities/tag.h \
    entities/script.h \
    entities/bookmark.h \
    dialogs/aboutdialog.h \
    dialogs/linkdialog.h \
    dialogs/notediffdialog.h \
    dialogs/settingsdialog.h \
    dialogs/tododialog.h \
    dialogs/trashdialog.h \
    dialogs/localtrashdialog.h \
    dialogs/updatedialog.h \
    dialogs/versiondialog.h \
    services/owncloudservice.h \
    services/updateservice.h \
    services/scriptingservice.h \
    services/websocketserverservice.h \
    helpers/htmlentities.h \
    helpers/clientproxy.h \
    helpers/toolbarcontainer.h \
    helpers/qownnotesmarkdownhighlighter.h \
    helpers/fakevimproxy.h \
    services/databaseservice.h \
    release.h \
    widgets/qownnotesmarkdowntextedit.h \
    dialogs/passworddialog.h \
    services/metricsservice.h \
    services/cryptoservice.h \
    dialogs/masterdialog.h \
    utils/misc.h \
    utils/git.h \
    utils/gui.h \
    utils/schema.h \
    dialogs/welcomedialog.h \
    dialogs/issueassistantdialog.h \
    dialogs/tagadddialog.h \
    widgets/navigationwidget.h \
    widgets/notepreviewwidget.h \
    api/noteapi.h \
    api/tagapi.h \
    widgets/logwidget.h \
    widgets/combobox.h \
    dialogs/sharedialog.h \
    widgets/fontcolorwidget.h \
    dialogs/evernoteimportdialog.h \
    dialogs/orphanedimagesdialog.h \
    dialogs/orphanedattachmentsdialog.h \
    dialogs/actiondialog.h \
    dialogs/tabledialog.h \
    libraries/qtcsv/src/include/qtcsv_global.h \
    libraries/qtcsv/src/include/abstractdata.h \
    libraries/qtcsv/src/include/reader.h \
    libraries/qtcsv/src/sources/filechecker.h \
    libraries/qtcsv/src/sources/symbols.h \
    dialogs/notedialog.h \
    dialogs/filedialog.h \
    dialogs/scriptrepositorydialog.h \
    widgets/scriptsettingwidget.h \
    api/scriptapi.h \
    widgets/label.h \
    widgets/lineedit.h \
    widgets/qtexteditsearchwidget.h \
    widgets/scriptlistwidget.h \
    widgets/notefolderlistwidget.h \
    widgets/notetreewidgetitem.h \
    widgets/layoutwidget.h \
    dialogs/serverbookmarksimportdialog.h

FORMS    += mainwindow.ui \
    dialogs/notediffdialog.ui \
    dialogs/aboutdialog.ui \
    dialogs/updatedialog.ui \
    dialogs/settingsdialog.ui \
    dialogs/versiondialog.ui \
    dialogs/trashdialog.ui \
    dialogs/localtrashdialog.ui \
    dialogs/linkdialog.ui \
    dialogs/tododialog.ui \
    dialogs/passworddialog.ui \
    dialogs/welcomedialog.ui \
    dialogs/issueassistantdialog.ui \
    dialogs/tagadddialog.ui \
    widgets/logwidget.ui \
    dialogs/sharedialog.ui \
    widgets/fontcolorwidget.ui \
    dialogs/evernoteimportdialog.ui \
    dialogs/orphanedimagesdialog.ui \
    dialogs/orphanedattachmentsdialog.ui \
    dialogs/actiondialog.ui \
    dialogs/tabledialog.ui \
    dialogs/notedialog.ui \
    dialogs/scriptrepositorydialog.ui \
    widgets/qtexteditsearchwidget.ui \
    widgets/scriptsettingwidget.ui \
    widgets/notetreewidgetitem.ui \
    widgets/layoutwidget.ui \
    dialogs/serverbookmarksimportdialog.ui

RESOURCES += \
    images.qrc \
    texts.qrc \
    breeze-qownnotes.qrc \
    breeze-dark-qownnotes.qrc \
    qownnotes.qrc \
    demonotes.qrc \
    libraries/qdarkstyle/style.qrc \
    configurations.qrc

include(libraries/qmarkdowntextedit/qmarkdowntextedit.pri)
include(libraries/piwiktracker/piwiktracker.pri)
include(libraries/botan/botan.pri)
include(libraries/qkeysequencewidget/qkeysequencewidget/qkeysequencewidget.pri)
include(libraries/qttoolbareditor/toolbar_editor.pri)
include(libraries/fakevim/fakevim/fakevim.pri)
include(libraries/singleapplication/singleapplication.pri)

unix {

  isEmpty(PREFIX) {
    PREFIX = /usr
  }

  isEmpty(BINDIR) {
    BINDIR = $$PREFIX/bin
  }

  isEmpty(DATADIR) {
    DATADIR = $$PREFIX/share
  }

  INSTALLS += target desktop i18n icons

  target.path = $$INSTROOT$$BINDIR
#  target.files += QOwnNotes

  desktop.path = $$DATADIR/applications
  desktop.files += PBE.QOwnNotes.desktop

  i18n.path = $$DATADIR/QOwnNotes/languages
  i18n.files += languages/*.qm

  icons.path = $$DATADIR/icons/hicolor
  icons.files += images/icons/*
}

CONFIG(debug, debug|release) {
#    QMAKE_CXXFLAGS_DEBUG += -g3 -O0
    message("Currently in DEBUG mode.")
} else {
    DEFINES += QT_NO_DEBUG

    # We want to allow optional debug output in releases
#    DEFINES += QT_NO_DEBUG_OUTPUT
    message("Currently in RELEASE mode.")
}

DEFINES += QAPPLICATION_CLASS=QApplication

DISTFILES += \
    ../README.md \
    ../CHANGELOG.md
