cmake_minimum_required(VERSION 3.5)
project(QOwnNotes)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# BEGIN options
option(QON_QT6_BUILD "Build QOwnNotes with Qt6" OFF)
option(DEV_MODE "Build QOwnNotes in developer mode" OFF)

option(BUILD_WITH_LIBGIT2 "Build using system installed libgit2 library" OFF)
# END options

if(QON_QT6_BUILD)
  set(QT_VERSION_MAJOR 6)
  set(QT_DEFAULT_MAJOR_VERSION 6)
else()
  set(QT_VERSION_MAJOR 5)
endif()

# put the QOwnNotes.exe in build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# for intellisence
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# include some Botan settings
add_subdirectory(libraries/botan)
# include sonnet core directory, required for building plugins
include_directories(libraries/sonnet/src/core)
# some hunspell settings for windows
include(libraries/sonnet/src/plugins/hunspell/hunspell/CMakeLists.txt)

if(BUILD_WITH_LIBGIT2)
  # Find libgit2
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LIBGIT2 REQUIRED libgit2)

  include_directories(${LIBGIT2_INCLUDE_DIRS})
  link_directories(${LIBGIT2_LIBRARY_DIRS})

  target_compile_definitions(botan PUBLIC USE_LIBGIT2)
endif()

add_executable(QOwnNotes "")

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build configuration detected")
  add_definitions(-DQT_DEBUG)
else()
  message("Non-debug build configuration detected")
endif()

set_target_properties(
  QOwnNotes
  PROPERTIES CXX_STANDARD 11
             CXX_STANDARD_REQUIRED ON
             AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON)

if(UNIX AND DEV_MODE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
      QOwnNotes
      PRIVATE -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments
              -Wno-error=deprecated-declarations)
  else()
    target_compile_options(QOwnNotes PRIVATE -Wall -Wextra -pedantic
                                             -Wno-error=deprecated-declarations)
  endif()

  # enable werror on linux
  if(NOT APPLE)
    message("Werror enabled on linux")
    target_compile_options(QOwnNotes PRIVATE -Werror)
  endif()
endif()

target_include_directories(QOwnNotes PRIVATE ${CMAKE_CURRENT_LIST_DIR}
                                             ${CMAKE_CURRENT_BINARY_DIR})

# Disable shared library building of libraries
set(BUILD_SHARED_LIBS OFF)
# QHotKey library Disable building tests inside QHotKey library
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(QHOTKEY_EXAMPLES OFF)
add_subdirectory(libraries/qhotkey EXCLUDE_FROM_ALL)

# FakeVim library
set(CREATE_STATIC_LIBRARY ON)
if(MSVC)
  add_compile_definitions(FAKEVIM_STATIC_DEFINE QTCREATOR_UTILS_STATIC_LIB)
endif()
add_subdirectory(libraries/fakevim EXCLUDE_FROM_ALL)
add_subdirectory(libraries/diff_match_patch EXCLUDE_FROM_ALL)

option(USE_QLITE_HTML "Build using QLiteHtml for preview" OFF)
if(USE_QLITEHTML)
  add_definitions(-DUSE_QLITEHTML=1)
  add_subdirectory(libraries/qlitehtml EXCLUDE_FROM_ALL)
endif()

message("Using Qt version ${QT_VERSION_MAJOR}")
set(QT_NO_CREATE_VERSIONLESS_FUNCTIONS ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS
  REQUIRED
  LinguistTools
  Core
  Widgets
  Gui
  Sql
  Svg
  Network
  Xml
  PrintSupport
  WebSockets
  Qml
  Concurrent)

find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
  message(STATUS "ccache found")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(RESOURCE_FILES
    breeze-qownnotes.qrc
    breeze-dark-qownnotes.qrc
    qownnotes.qrc
    demonotes.qrc
    images.qrc
    texts.qrc
    configurations.qrc
    libraries/qmarkdowntextedit/media.qrc
    libraries/qdarkstyle/style.qrc
    libraries/qkeysequencewidget/qkeysequencewidget/qkeysequencewidget.qrc)

set(SOURCE_FILES
    dialogs/welcomedialog.cpp
    dialogs/welcomedialog.h
    dialogs/welcomedialog.ui
    dialogs/issueassistantdialog.cpp
    dialogs/issueassistantdialog.h
    dialogs/issueassistantdialog.ui
    dialogs/aboutdialog.cpp
    dialogs/aboutdialog.h
    dialogs/aboutdialog.ui
    dialogs/linkdialog.cpp
    dialogs/linkdialog.h
    dialogs/linkdialog.ui
    dialogs/notediffdialog.cpp
    dialogs/notediffdialog.h
    dialogs/notediffdialog.ui
    dialogs/textdiffdialog.cpp
    dialogs/textdiffdialog.h
    dialogs/textdiffdialog.ui
    dialogs/settingsdialog.cpp
    dialogs/settingsdialog.h
    dialogs/settingsdialog.ui
    dialogs/tododialog.cpp
    dialogs/tododialog.h
    dialogs/tododialog.ui
    dialogs/trashdialog.cpp
    dialogs/trashdialog.h
    dialogs/trashdialog.ui
    dialogs/localtrashdialog.cpp
    dialogs/localtrashdialog.h
    dialogs/localtrashdialog.ui
    dialogs/nextclouddeckdialog.cpp
    dialogs/nextclouddeckdialog.h
    dialogs/nextclouddeckdialog.ui
    dialogs/updatedialog.cpp
    dialogs/updatedialog.h
    dialogs/updatedialog.ui
    dialogs/versiondialog.cpp
    dialogs/versiondialog.h
    dialogs/versiondialog.ui
    dialogs/passworddialog.cpp
    dialogs/passworddialog.h
    dialogs/passworddialog.ui
    dialogs/masterdialog.cpp
    dialogs/masterdialog.h
    dialogs/tagadddialog.cpp
    dialogs/tagadddialog.h
    dialogs/tagadddialog.ui
    dialogs/sharedialog.cpp
    dialogs/sharedialog.h
    dialogs/sharedialog.ui
    dialogs/evernoteimportdialog.cpp
    dialogs/evernoteimportdialog.h
    dialogs/evernoteimportdialog.ui
    dialogs/joplinimportdialog.cpp
    dialogs/joplinimportdialog.h
    dialogs/joplinimportdialog.ui
    dialogs/serverbookmarksimportdialog.cpp
    dialogs/serverbookmarksimportdialog.h
    dialogs/serverbookmarksimportdialog.ui
    dialogs/storedimagesdialog.cpp
    dialogs/storedimagesdialog.h
    dialogs/storedimagesdialog.ui
    dialogs/storedattachmentsdialog.cpp
    dialogs/storedattachmentsdialog.h
    dialogs/storedattachmentsdialog.ui
    dialogs/scriptrepositorydialog.cpp
    dialogs/scriptrepositorydialog.h
    dialogs/scriptrepositorydialog.ui
    dialogs/dictionarymanagerdialog.cpp
    dialogs/dictionarymanagerdialog.h
    dialogs/dictionarymanagerdialog.ui
    dialogs/actiondialog.cpp
    dialogs/actiondialog.h
    dialogs/actiondialog.ui
    dialogs/tabledialog.cpp
    dialogs/tabledialog.h
    dialogs/tabledialog.ui
    dialogs/notedialog.cpp
    dialogs/notedialog.h
    dialogs/notedialog.ui
    dialogs/websockettokendialog.cpp
    dialogs/websockettokendialog.h
    dialogs/websockettokendialog.ui
    dialogs/imagedialog.cpp
    dialogs/imagedialog.h
    dialogs/imagedialog.ui
    dialogs/commandbar.cpp
    dialogs/commandbar.h
    dialogs/attachmentdialog.cpp
    dialogs/attachmentdialog.h
    dialogs/attachmentdialog.ui
    dialogs/filedialog.cpp
    dialogs/filedialog.h
    models/commandmodel.cpp
    models/commandmodel.h
    entities/calendaritem.cpp
    entities/calendaritem.h
    entities/note.cpp
    entities/note.h
    entities/trashitem.cpp
    entities/trashitem.h
    entities/notesubfolder.cpp
    entities/notesubfolder.h
    entities/notehistory.cpp
    entities/notehistory.h
    entities/notefolder.cpp
    entities/notefolder.h
    entities/cloudconnection.cpp
    entities/cloudconnection.h
    entities/tag.cpp
    entities/tag.h
    entities/script.cpp
    entities/script.h
    helpers/flowlayout.cpp
    helpers/flowlayout.h
    helpers/htmlentities.cpp
    helpers/htmlentities.h
    helpers/clientproxy.cpp
    helpers/clientproxy.h
    helpers/toolbarcontainer.cpp
    helpers/toolbarcontainer.h
    helpers/qownnotesmarkdownhighlighter.cpp
    helpers/qownnotesmarkdownhighlighter.h
    helpers/LanguageCache.h
    helpers/qownspellchecker.cpp
    helpers/qownspellchecker.h
    helpers/codetohtmlconverter.cpp
    helpers/codetohtmlconverter.h
    helpers/nomenuiconstyle.cpp
    helpers/nomenuiconstyle.h
    api/noteapi.cpp
    api/noteapi.h
    api/notesubfolderapi.cpp
    api/notesubfolderapi.h
    api/tagapi.cpp
    api/tagapi.h
    api/scriptapi.cpp
    api/scriptapi.h
    libraries/qmarkdowntextedit/linenumberarea.h
    libraries/qmarkdowntextedit/qownlanguagedata.cpp
    libraries/qmarkdowntextedit/qownlanguagedata.h
    libraries/qmarkdowntextedit/markdownhighlighter.cpp
    libraries/qmarkdowntextedit/markdownhighlighter.h
    libraries/qmarkdowntextedit/qmarkdowntextedit.cpp
    libraries/qmarkdowntextedit/qmarkdowntextedit.h
    libraries/qmarkdowntextedit/qplaintexteditsearchwidget.cpp
    libraries/qmarkdowntextedit/qplaintexteditsearchwidget.h
    libraries/qmarkdowntextedit/qplaintexteditsearchwidget.ui
    libraries/simplecrypt/simplecrypt.cpp
    libraries/simplecrypt/simplecrypt.h
    libraries/versionnumber/versionnumber.cpp
    libraries/versionnumber/versionnumber.h
    libraries/singleapplication/singleapplication.h
    libraries/singleapplication/singleapplication.cpp
    libraries/singleapplication/singleapplication_p.h
    libraries/singleapplication/singleapplication_p.cpp
    libraries/qttoolbareditor/src/toolbar_editor.cpp
    libraries/qttoolbareditor/src/toolbar_editor.hpp
    libraries/qttoolbareditor/src/toolbar_editor.ui
    libraries/qtcsv/src/include/qtcsv_global.h
    libraries/qtcsv/src/include/abstractdata.h
    libraries/qtcsv/src/include/reader.h
    libraries/qtcsv/src/sources/reader.cpp
    libraries/qtcsv/src/sources/filechecker.h
    libraries/qtcsv/src/sources/symbols.h
    libraries/qtwaitingspinner/waitingspinnerwidget.h
    libraries/qtwaitingspinner/waitingspinnerwidget.cpp
    services/databaseservice.cpp
    services/databaseservice.h
    services/owncloudservice.cpp
    services/owncloudservice.h
    services/nextclouddeckservice.cpp
    services/nextclouddeckservice.h
    services/updateservice.cpp
    services/updateservice.h
    services/metricsservice.cpp
    services/metricsservice.h
    services/cryptoservice.cpp
    services/cryptoservice.h
    services/scriptingservice.cpp
    services/scriptingservice.h
    services/openaiservice.cpp
    services/openaiservice.h
    services/settingsservice.cpp
    services/settingsservice.h
    libraries/piwiktracker/piwiktracker.h
    libraries/piwiktracker/piwiktracker.cpp
    libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget_p.h
    libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget.h
    libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget.cpp
    libraries/sonnet/src/core/backgroundchecker.cpp
    libraries/sonnet/src/core/backgroundchecker.h
    libraries/sonnet/src/core/backgroundchecker_p.h
    libraries/sonnet/src/core/client.cpp
    libraries/sonnet/src/core/client_p.h
    libraries/sonnet/src/core/core_debug.cpp
    libraries/sonnet/src/core/core_debug.h
    libraries/sonnet/src/core/guesslanguage.cpp
    libraries/sonnet/src/core/guesslanguage.h
    libraries/sonnet/src/core/languagefilter.cpp
    libraries/sonnet/src/core/languagefilter_p.h
    libraries/sonnet/src/core/loader.cpp
    libraries/sonnet/src/core/loader_p.h
    libraries/sonnet/src/core/settings.cpp
    libraries/sonnet/src/core/settings_p.h
    libraries/sonnet/src/core/sonnetcore_export.h
    libraries/sonnet/src/core/speller.cpp
    libraries/sonnet/src/core/speller.h
    libraries/sonnet/src/core/spellerplugin.cpp
    libraries/sonnet/src/core/spellerplugin_p.h
    libraries/sonnet/src/core/textbreaks.cpp
    libraries/sonnet/src/core/textbreaks_p.h
    libraries/sonnet/src/core/tokenizer.cpp
    libraries/sonnet/src/core/tokenizer_p.h
    libraries/fuzzy/kfuzzymatcher.cpp
    libraries/qr-code-generator/QrCode.hpp
    libraries/qr-code-generator/QrCode.cpp
    threads/scriptthread.cpp
    threads/scriptthread.h
    widgets/qownnotesmarkdowntextedit.cpp
    widgets/qownnotesmarkdowntextedit.h
    widgets/navigationwidget.cpp
    widgets/navigationwidget.h
    widgets/backlinkwidget.cpp
    widgets/backlinkwidget.h
    widgets/notepreviewwidget.cpp
    widgets/notepreviewwidget.h
    widgets/combobox.cpp
    widgets/combobox.h
    widgets/graphicsview.cpp
    widgets/graphicsview.h
    widgets/scriptlistwidget.cpp
    widgets/scriptlistwidget.h
    widgets/notefolderlistwidget.cpp
    widgets/notefolderlistwidget.h
    widgets/fontcolorwidget.ui
    widgets/fontcolorwidget.cpp
    widgets/fontcolorwidget.h
    widgets/logwidget.cpp
    widgets/logwidget.h
    widgets/logwidget.ui
    widgets/label.cpp
    widgets/label.h
    widgets/qrcodewidget.cpp
    widgets/qrcodewidget.h
    widgets/lineedit.cpp
    widgets/lineedit.h
    widgets/passwordlineedit.cpp
    widgets/passwordlineedit.h
    widgets/scriptsettingwidget.cpp
    widgets/scriptsettingwidget.h
    widgets/scriptsettingwidget.ui
    widgets/qtexteditsearchwidget.cpp
    widgets/qtexteditsearchwidget.h
    widgets/qtexteditsearchwidget.ui
    widgets/notetreewidgetitem.cpp
    widgets/notetreewidgetitem.h
    widgets/notetreewidgetitem.ui
    widgets/notefilepathlabel.cpp
    widgets/notefilepathlabel.h
    widgets/layoutwidget.cpp
    widgets/layoutwidget.h
    widgets/layoutwidget.ui
    widgets/todoitemtreewidget.cpp
    widgets/todoitemtreewidget.h
    widgets/htmlpreviewwidget.h
    widgets/htmlpreviewwidget.cpp
    widgets/notesubfoldertree.cpp
    widgets/notesubfoldertree.h
    widgets/noterelationscene.cpp
    widgets/noterelationscene.h
    utils/misc.cpp
    utils/misc.h
    utils/git.cpp
    utils/git.h
    utils/cli.cpp
    utils/cli.h
    utils/gui.cpp
    utils/gui.h
    utils/schema.cpp
    utils/schema.h
    utils/urlhandler.h
    utils/urlhandler.cpp
    build_number.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    release.h
    version.h
    helpers/fakevimproxy.h
    helpers/fakevimproxy.cpp
    services/websocketserverservice.cpp
    services/websocketserverservice.h
    services/webappclientservice.cpp
    services/webappclientservice.h
    entities/bookmark.h
    entities/bookmark.cpp
    entities/commandsnippet.h
    entities/commandsnippet.cpp)

# spellchecker
list(
  APPEND
  SOURCE_FILES
  libraries/sonnet/src/plugins/hunspell/hunspellclient.cpp
  libraries/sonnet/src/plugins/hunspell/hunspellclient.h
  libraries/sonnet/src/plugins/hunspell/hunspelldebug.cpp
  libraries/sonnet/src/plugins/hunspell/hunspelldebug.h
  libraries/sonnet/src/plugins/hunspell/hunspelldict.cpp
  libraries/sonnet/src/plugins/hunspell/hunspelldict.h
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/affentry.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/affentry.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/affixmgr.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/affixmgr.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/atypes.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/baseaffix.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/csutil.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/csutil.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/filemgr.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/filemgr.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hashmgr.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hashmgr.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/htypes.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hunspell.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hunspell.h
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hunspell.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hunvisapi.h
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hunvisapi.h.in
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hunzip.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/hunzip.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/langnum.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/phonet.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/phonet.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/replist.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/replist.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/suggestmgr.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/suggestmgr.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/utf_info.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/hunspell/w_char.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/firstparser.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/firstparser.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/htmlparser.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/htmlparser.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/latexparser.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/latexparser.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/manparser.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/manparser.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/odfparser.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/odfparser.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/textparser.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/textparser.hxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/xmlparser.cxx
  libraries/sonnet/src/plugins/hunspell/hunspell/src/parsers/xmlparser.hxx)

# md4c
list(APPEND SOURCE_FILES libraries/md4c/src/md4c.c
     libraries/md4c/src/md4c-html.c libraries/md4c/src/entity.c)

# Translation files
set(QON_TS_FILES
    languages/QOwnNotes_en.ts
    languages/QOwnNotes_de.ts
    languages/QOwnNotes_fr.ts
    languages/QOwnNotes_pl.ts
    languages/QOwnNotes_zh_CN.ts
    languages/QOwnNotes_zh_TW.ts
    languages/QOwnNotes_ru.ts
    languages/QOwnNotes_pt_BR.ts
    languages/QOwnNotes_pt_PT.ts
    languages/QOwnNotes_es.ts
    languages/QOwnNotes_nl.ts
    languages/QOwnNotes_hu.ts
    languages/QOwnNotes_ja.ts
    languages/QOwnNotes_it.ts
    languages/QOwnNotes_ar.ts
    languages/QOwnNotes_uk.ts
    languages/QOwnNotes_cs.ts
    languages/QOwnNotes_hr.ts
    languages/QOwnNotes_ca.ts
    languages/QOwnNotes_sv.ts
    languages/QOwnNotes_id.ts
    languages/QOwnNotes_bn.ts
    languages/QOwnNotes_tr.ts
    languages/QOwnNotes_tl.ts
    languages/QOwnNotes_fil.ts
    languages/QOwnNotes_ceb.ts
    languages/QOwnNotes_hi.ts
    languages/QOwnNotes_hil.ts
    languages/QOwnNotes_ur.ts
    languages/QOwnNotes_fi.ts
    languages/QOwnNotes_el.ts
    languages/QOwnNotes_gl.ts
    languages/QOwnNotes_no.ts
    languages/QOwnNotes_da.ts
    languages/QOwnNotes_ro.ts
    languages/QOwnNotes_sl.ts
    languages/QOwnNotes_sk.ts
    languages/QOwnNotes_eu.ts
    languages/QOwnNotes_sr.ts
    languages/QOwnNotes_fa.ts
    languages/QOwnNotes_ha.ts
    languages/QOwnNotes_bg.ts
    languages/QOwnNotes_is.ts
    languages/QOwnNotes_pa.ts
    languages/QOwnNotes_sq.ts
    languages/QOwnNotes_he.ts
    languages/QOwnNotes_yi.ts
    languages/QOwnNotes_th.ts
    languages/QOwnNotes_sn.ts
    languages/QOwnNotes_km.ts
    languages/QOwnNotes_si.ts
    languages/QOwnNotes_zu.ts
    languages/QOwnNotes_xh.ts
    languages/QOwnNotes_ms.ts
    languages/QOwnNotes_mi.ts
    languages/QOwnNotes_ga.ts
    languages/QOwnNotes_uz.ts
    languages/QOwnNotes_vi.ts
    languages/QOwnNotes_lv.ts
    languages/QOwnNotes_ku.ts
    languages/QOwnNotes_lt.ts
    languages/QOwnNotes_bs.ts
    languages/QOwnNotes_mk.ts
    languages/QOwnNotes_ko.ts
    languages/QOwnNotes_et.ts
    languages/QOwnNotes_en_GB.ts)

if(QON_QT6_BUILD)
  qt6_add_translation(QON_QM_FILES ${QON_TS_FILES})
else()
  qt5_add_translation(QON_QM_FILES ${QON_TS_FILES})
endif()

add_custom_target(translations DEPENDS ${QON_QM_FILES})

target_sources(QOwnNotes PRIVATE ${SOURCE_FILES} ${RESOURCE_FILES}
                                 ${QON_QM_FILES})

# The Qt5::Widgets target will also link Qt::Gui and Qt::Core targets
target_link_libraries(
  QOwnNotes
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::Sql
          Qt${QT_VERSION_MAJOR}::Svg
          Qt${QT_VERSION_MAJOR}::Network
          Qt${QT_VERSION_MAJOR}::Xml
          Qt${QT_VERSION_MAJOR}::PrintSupport
          Qt${QT_VERSION_MAJOR}::WebSockets
          Qt${QT_VERSION_MAJOR}::Qml
          Qt${QT_VERSION_MAJOR}::Concurrent
          QHotkey::QHotkey
          Botan::Botan
          # ${LIBGIT2_LIBRARIES}
          fakevim
          Google::DiffMatchPatch)

if(USE_QLITEHTML)
  target_link_libraries(QOwnNotes PRIVATE qlitehtml::qlitehtml)
endif()

if(BUILD_WITH_LIBGIT2)
  target_link_libraries(QOwnNotes PRIVATE ${LIBGIT2_LIBRARIES})
endif()

if(MINGW OR MSVC)
  # Qt 5 does not define UNICODE for CMake only qmake automatically define
  # UNICODE
  target_compile_definitions(QOwnNotes PRIVATE UNICODE _UNICODE)
endif()

if(MSVC)
  target_compile_definitions(QOwnNotes PRIVATE NOMINMAX)
endif()

add_compile_definitions(QAPPLICATION_CLASS=QApplication)

# Installation

install(TARGETS QOwnNotes DESTINATION bin)
foreach(
  SIZE
  16x16
  24x24
  32x32
  48x48
  64x64
  96x96
  128x128
  256x256
  512x512
  scalable)
  install(DIRECTORY images/icons/${SIZE} DESTINATION share/icons/hicolor)
endforeach()
install(FILES PBE.QOwnNotes.desktop DESTINATION share/applications)
install(FILES ${QON_QM_FILES} DESTINATION share/QOwnNotes/translations)
