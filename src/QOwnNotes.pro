#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T08:31:41
#
#-------------------------------------------------

QT       += core gui widgets sql svg network xml printsupport qml websockets concurrent

# quick is enabled for more scripting options
# Windows and macOS seem to ignore that
#QT       += quick

CONFIG += with_aspell

# enable pch for DEV_MODE
# put any dev specific options here
CONFIG(DEV_MODE) {
    message("[DevMode] PCH Enabled")
    CONFIG += precompile_header
    PRECOMPILED_HEADER  = pch.h
    HEADERS += pch.h
}

CONFIG(USE_QLITEHTML) {
    message("QLiteHtml usage enabled")
    DEFINES += USE_QLITEHTML=1
    include(libraries/qlitehtml/src/qlitehtml.pri)
}

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
    languages/QOwnNotes_sk.ts \
    languages/QOwnNotes_eu.ts \
    languages/QOwnNotes_sr.ts \
    languages/QOwnNotes_fa.ts \
    languages/QOwnNotes_ha.ts \
    languages/QOwnNotes_bg.ts \
    languages/QOwnNotes_is.ts \
    languages/QOwnNotes_pa.ts \
    languages/QOwnNotes_he.ts \
    languages/QOwnNotes_yi.ts \
    languages/QOwnNotes_th.ts \
    languages/QOwnNotes_sn.ts \
    languages/QOwnNotes_km.ts \
    languages/QOwnNotes_si.ts \
    languages/QOwnNotes_zu.ts \
    languages/QOwnNotes_xh.ts \
    languages/QOwnNotes_ms.ts \
    languages/QOwnNotes_mi.ts \
    languages/QOwnNotes_ga.ts \
    languages/QOwnNotes_uz.ts \
    languages/QOwnNotes_vi.ts \
    languages/QOwnNotes_lv.ts \
    languages/QOwnNotes_ku.ts \
    languages/QOwnNotes_lt.ts \
    languages/QOwnNotes_bs.ts \
    languages/QOwnNotes_mk.ts \
    languages/QOwnNotes_ko.ts \
    languages/QOwnNotes_et.ts \
    languages/QOwnNotes_sq.ts \
    languages/QOwnNotes_en_GB.ts

CODECFORTR = UTF-8
lessThan(QT_MAJOR_VERSION, 6) {
    CONFIG += c++11

    # for older Qt versions
    win32-msvc {
        QMAKE_CXXFLAGS += "/std:c++11"
    } else {
        QMAKE_CXXFLAGS += "-std=c++11"
    }
} else {
    win32:CONFIG += entrypoint
    CONFIG += c++17
}

INCLUDEPATH += $$PWD/libraries $$PWD/libraries/diff_match_patch

SOURCES += main.cpp\
    helpers/nomenuiconstyle.cpp \
    dialogs/attachmentdialog.cpp \
    dialogs/nextclouddeckdialog.cpp \
    entities/cloudconnection.cpp \
    helpers/codetohtmlconverter.cpp \
    helpers/qownspellchecker.cpp \
        mainwindow.cpp \
    libraries/diff_match_patch/diff_match_patch.cpp \
    libraries/simplecrypt/simplecrypt.cpp \
    libraries/versionnumber/versionnumber.cpp \
    libraries/botan/botanwrapper.cpp \
    libraries/md4c/src/md4c.c \
    libraries/md4c/src/md4c-html.c \
    libraries/md4c/src/entity.c \
    libraries/qtwaitingspinner/waitingspinnerwidget.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/linkdialog.cpp \
    dialogs/notediffdialog.cpp \
    dialogs/textdiffdialog.cpp \
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
    entities/commandsnippet.cpp \
    services/owncloudservice.cpp \
    services/nextclouddeckservice.cpp \
    services/updateservice.cpp \
    helpers/htmlentities.cpp \
    helpers/clientproxy.cpp \
    helpers/toolbarcontainer.cpp \
    helpers/qownnotesmarkdownhighlighter.cpp \
    helpers/fakevimproxy.cpp \
    helpers/flowlayout.cpp \
    services/databaseservice.cpp \
    threads/scriptthread.cpp \
    widgets/graphicsview.cpp \
    widgets/qownnotesmarkdowntextedit.cpp \
    dialogs/passworddialog.cpp \
    services/metricsservice.cpp \
    services/cryptoservice.cpp \
    services/scriptingservice.cpp \
    services/websocketserverservice.cpp \
    services/webappclientservice.cpp \
    services/openaiservice.cpp \
    services/settingsservice.cpp \
    dialogs/masterdialog.cpp \
    utils/misc.cpp \
    utils/git.cpp \
    utils/gui.cpp \
    utils/cli.cpp \
    utils/schema.cpp \
    dialogs/welcomedialog.cpp \
    dialogs/issueassistantdialog.cpp \
    dialogs/tagadddialog.cpp \
    widgets/navigationwidget.cpp \
    widgets/backlinkwidget.cpp \
    widgets/notepreviewwidget.cpp \
    api/noteapi.cpp \
    api/notesubfolderapi.cpp \
    api/tagapi.cpp \
    widgets/logwidget.cpp \
    widgets/combobox.cpp \
    dialogs/sharedialog.cpp \
    widgets/fontcolorwidget.cpp \
    dialogs/evernoteimportdialog.cpp \
    dialogs/joplinimportdialog.cpp \
    dialogs/storedimagesdialog.cpp \
    dialogs/storedattachmentsdialog.cpp \
    dialogs/actiondialog.cpp \
    dialogs/tabledialog.cpp \
    libraries/qtcsv/src/sources/reader.cpp \
    dialogs/notedialog.cpp \
    dialogs/filedialog.cpp \
    dialogs/scriptrepositorydialog.cpp \
    dialogs/dictionarymanagerdialog.cpp \
    widgets/scriptsettingwidget.cpp \
    api/scriptapi.cpp \
    widgets/label.cpp \
    widgets/notefilepathlabel.cpp \
    widgets/qrcodewidget.cpp \
    widgets/lineedit.cpp \
    widgets/passwordlineedit.cpp \
    widgets/qtexteditsearchwidget.cpp \
    widgets/scriptlistwidget.cpp \
    widgets/notefolderlistwidget.cpp \
    widgets/notetreewidgetitem.cpp \
    widgets/todoitemtreewidget.cpp \
    widgets/layoutwidget.cpp \
    widgets/htmlpreviewwidget.cpp \
    dialogs/serverbookmarksimportdialog.cpp \
    dialogs/websockettokendialog.cpp \
    dialogs/imagedialog.cpp \
    dialogs/commandbar.cpp \
    models/commandmodel.cpp \
    libraries/fuzzy/kfuzzymatcher.cpp \
    libraries/qr-code-generator/QrCode.cpp \
    widgets/notesubfoldertree.cpp \
    widgets/noterelationscene.cpp \
    utils/urlhandler.cpp

HEADERS  += mainwindow.h \
    helpers/nomenuiconstyle.h \
    build_number.h \
    dialogs/attachmentdialog.h \
    dialogs/nextclouddeckdialog.h \
    entities/cloudconnection.h \
    helpers/LanguageCache.h \
    helpers/codetohtmlconverter.h \
    helpers/qownspellchecker.h \
    threads/scriptthread.h \
    version.h \
    libraries/diff_match_patch/diff_match_patch.h \
    libraries/simplecrypt/simplecrypt.h \
    libraries/versionnumber/versionnumber.h \
    libraries/botan/botanwrapper.h \
    libraries/md4c/src/md4c.h \
    libraries/md4c/src/md4c-html.h \
    libraries/md4c/src/entity.h \
    entities/notehistory.h \
    entities/note.h \
    entities/trashitem.h \
    entities/notesubfolder.h \
    entities/calendaritem.h \
    entities/notefolder.h \
    entities/tag.h \
    entities/script.h \
    entities/bookmark.h \
    entities/commandsnippet.h \
    dialogs/aboutdialog.h \
    dialogs/linkdialog.h \
    dialogs/notediffdialog.h \
    dialogs/textdiffdialog.h \
    dialogs/settingsdialog.h \
    dialogs/tododialog.h \
    dialogs/trashdialog.h \
    dialogs/localtrashdialog.h \
    dialogs/updatedialog.h \
    dialogs/versiondialog.h \
    services/owncloudservice.h \
    services/nextclouddeckservice.h \
    services/updateservice.h \
    services/scriptingservice.h \
    services/websocketserverservice.h \
    services/webappclientservice.h \
    helpers/htmlentities.h \
    helpers/clientproxy.h \
    helpers/toolbarcontainer.h \
    helpers/qownnotesmarkdownhighlighter.h \
    helpers/fakevimproxy.h \
    helpers/flowlayout.h \
    services/databaseservice.h \
    release.h \
    widgets/graphicsview.h \
    widgets/qownnotesmarkdowntextedit.h \
    dialogs/passworddialog.h \
    services/metricsservice.h \
    services/cryptoservice.h \
    services/openaiservice.h \
    services/settingsservice.h \
    dialogs/masterdialog.h \
    utils/misc.h \
    utils/git.h \
    utils/gui.h \
    utils/cli.h \
    utils/schema.h \
    dialogs/welcomedialog.h \
    dialogs/issueassistantdialog.h \
    dialogs/tagadddialog.h \
    widgets/navigationwidget.h \
    widgets/backlinkwidget.h \
    widgets/notepreviewwidget.h \
    api/noteapi.h \
    api/notesubfolderapi.h \
    api/tagapi.h \
    widgets/logwidget.h \
    widgets/combobox.h \
    dialogs/sharedialog.h \
    widgets/fontcolorwidget.h \
    dialogs/evernoteimportdialog.h \
    dialogs/joplinimportdialog.h \
    dialogs/storedimagesdialog.h \
    dialogs/storedattachmentsdialog.h \
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
    dialogs/dictionarymanagerdialog.h \
    widgets/scriptsettingwidget.h \
    api/scriptapi.h \
    widgets/label.h \
    widgets/notefilepathlabel.h \
    widgets/qrcodewidget.h \
    widgets/lineedit.h \
    widgets/passwordlineedit.h \
    widgets/qtexteditsearchwidget.h \
    widgets/scriptlistwidget.h \
    widgets/notefolderlistwidget.h \
    widgets/notetreewidgetitem.h \
    widgets/todoitemtreewidget.h \
    widgets/layoutwidget.h \
    widgets/htmlpreviewwidget.h \
    dialogs/serverbookmarksimportdialog.h \
    dialogs/websockettokendialog.h \
    dialogs/imagedialog.h \
    dialogs/commandbar.h \
    models/commandmodel.h \
    libraries/fuzzy/kfuzzymatcher.h \
    libraries/qr-code-generator/QrCode.hpp \
    libraries/qtwaitingspinner/waitingspinnerwidget.h \
    widgets/notesubfoldertree.h \
    widgets/noterelationscene.h \
    utils/urlhandler.h \

FORMS    += mainwindow.ui \
    dialogs/attachmentdialog.ui \
    dialogs/imagedialog.ui \
    dialogs/nextclouddeckdialog.ui \
    dialogs/notediffdialog.ui \
    dialogs/textdiffdialog.ui \
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
    dialogs/joplinimportdialog.ui \
    dialogs/storedimagesdialog.ui \
    dialogs/storedattachmentsdialog.ui \
    dialogs/actiondialog.ui \
    dialogs/tabledialog.ui \
    dialogs/notedialog.ui \
    dialogs/scriptrepositorydialog.ui \
    dialogs/dictionarymanagerdialog.ui \
    widgets/qtexteditsearchwidget.ui \
    widgets/scriptsettingwidget.ui \
    widgets/notetreewidgetitem.ui \
    widgets/layoutwidget.ui \
    dialogs/serverbookmarksimportdialog.ui \
    dialogs/websockettokendialog.ui

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
include(libraries/fakevim/fakevim.pri)
include(libraries/singleapplication/singleapplication.pri)
include(libraries/sonnet/src/core/sonnet-core.pri)
include(libraries/qhotkey/qhotkey.pri)

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

  # Don't add desktop file when building snap
  !CONFIG(snapcraft, snapcraft) {
      desktop.files += PBE.QOwnNotes.desktop
  }

  # The qt5/qt6 paths qre needed by the Fedora and openSUSE builds on OBS
  # Keep in mind that Debian and Ubuntu don't work with those paths with cmake and Qt6
  lessThan(QT_MAJOR_VERSION, 6) {
      i18n.path = $$DATADIR/qt5/translations
  } else {
      i18n.path = $$DATADIR/qt6/translations
  }
  i18n.files += languages/*.qm

  icons.path = $$DATADIR/icons/hicolor
  icons.files += images/icons/*
}

!win32-msvc: QMAKE_CXXFLAGS += "-Wall -Wextra -Wundef"

# Enable Werror on unixes except mac
CONFIG(DEV_MODE) {
    unix:!mac {
        message("Werror enabled")
        QMAKE_CXXFLAGS += "-Wno-error=deprecated-declarations -Werror -pedantic"
    }
}

CONFIG(debug, debug|release) {
#    QMAKE_CXXFLAGS_DEBUG += -g3 -O0
    message("Currently in DEBUG mode.")
    DEFINES += QT_DEBUG
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
