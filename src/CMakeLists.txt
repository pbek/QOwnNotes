cmake_minimum_required(VERSION 3.2.2)
project(QOwnNotes)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package( Qt5LinguistTools )
find_package( Qt5Core 5.3 REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Sql REQUIRED )
find_package( Qt5Svg REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5Xml REQUIRED )
find_package( Qt5XmlPatterns REQUIRED )
find_package( Qt5PrintSupport REQUIRED )
find_package( Qt5WebSockets REQUIRED )
find_package( Qt5Qml REQUIRED )

qt5_wrap_ui(dialogs/ui_settingsdialog.h settingsdialog.ui)
qt5_wrap_ui(dialogs/ui_tododialog.h tododialog.ui)
qt5_wrap_ui(dialogs/ui_aboutdialog.h aboutdialog.ui)
qt5_wrap_ui(widgets/ui_logwidget.h logwidget.ui)
qt5_wrap_ui(dialogs/ui_trashdialog.h trashdialog.ui)
qt5_wrap_ui(dialogs/ui_localtrashdialog.h localtrashdialog.ui)
qt5_wrap_ui(dialogs/ui_linkdialog.h linkdialog.ui)
qt5_wrap_ui(dialogs/ui_updatedialog.h updatedialog.ui)
qt5_wrap_ui(dialogs/ui_notediffdialog.h notediffdialog.ui)
qt5_wrap_ui(dialogs/ui_versiondialog.h versiondialog.ui)
qt5_wrap_ui(dialogs/ui_welcomedialog.h welcomedialog.ui)
qt5_wrap_ui(dialogs/ui_issueassistantdialog.h issueassistantdialog.ui)
qt5_wrap_ui(dialogs/ui_tagadddialog.h tagadddialog.ui)
qt5_wrap_ui(widgets/ui_fontcolorwidget.h fontcolorwidget.ui)
qt5_wrap_ui(widgets/ui_evernoteimportdialog.h ui_evernoteimportdialog.ui)
qt5_wrap_ui(widgets/ui_serverbookmarksimportdialog.h ui_serverbookmarksimportdialog.ui)
qt5_wrap_ui(widgets/ui_layoutwidget.ui ui_layoutwidget.ui)
qt5_wrap_ui(dialogs/ui_orphanedimagesdialog.h orphanedimagesdialog.ui)
qt5_wrap_ui(dialogs/ui_orphanedattachmentsdialog.h orphanedattachmentsdialog.ui)
qt5_wrap_ui(dialogs/ui_actiondialog.h actiondialog.ui)
qt5_wrap_ui(dialogs/ui_tabledialog.h tabledialog.ui)
qt5_wrap_ui(dialogs/ui_notedialog.h notedialog.ui)
qt5_wrap_ui(libraries/qmarkdowntextedit/ui_qplaintexteditsearchwidget.h
        libraries/qmarkdowntextedit/qplaintexteditsearchwidget.ui)
qt5_wrap_ui(libraries/qttoolbareditor/src/toolbar_editor.hpp
        libraries/qttoolbareditor/src/toolbar_editor.ui)

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
        libraries/qkeysequencewidget/qkeysequencewidget/qkeysequencewidget.qrc
        )

qt5_add_resources(RESOURCE_ADDED ${RESOURCE_FILES})

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
    dialogs/serverbookmarksimportdialog.cpp
    dialogs/serverbookmarksimportdialog.h
    dialogs/serverbookmarksimportdialog.ui
    dialogs/orphanedimagesdialog.cpp
    dialogs/orphanedimagesdialog.h
    dialogs/orphanedimagesdialog.ui
    dialogs/orphanedattachmentsdialog.cpp
    dialogs/orphanedattachmentsdialog.h
    dialogs/orphanedattachmentsdialog.ui
    dialogs/scriptrepositorydialog.cpp
    dialogs/scriptrepositorydialog.h
    dialogs/scriptrepositorydialog.ui
    dialogs/actiondialog.cpp
    dialogs/actiondialog.h
    dialogs/actiondialog.ui
    dialogs/tabledialog.cpp
    dialogs/tabledialog.h
    dialogs/tabledialog.ui
    dialogs/notedialog.cpp
    dialogs/notedialog.h
    dialogs/notedialog.ui
    dialogs/filedialog.cpp
    dialogs/filedialog.h
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
    entities/tag.cpp
    entities/tag.h
    entities/script.cpp
    entities/script.h
    helpers/htmlentities.cpp
    helpers/htmlentities.h
    helpers/clientproxy.cpp
    helpers/clientproxy.h
    helpers/toolbarcontainer.cpp
    helpers/toolbarcontainer.h
    helpers/qownnotesmarkdownhighlighter.cpp
    helpers/qownnotesmarkdownhighlighter.h
    api/noteapi.cpp
    api/noteapi.h
    api/tagapi.cpp
    api/tagapi.h
    api/scriptapi.cpp
    api/scriptapi.h
    libraries/diff_match_patch/diff_match_patch.cpp
    libraries/diff_match_patch/diff_match_patch.h
    libraries/hoedown/autolink.c
    libraries/hoedown/autolink.h
    libraries/hoedown/buffer.c
    libraries/hoedown/buffer.h
    libraries/hoedown/document.c
    libraries/hoedown/document.h
    libraries/hoedown/escape.c
    libraries/hoedown/escape.h
    libraries/hoedown/html.c
    libraries/hoedown/html.h
    libraries/hoedown/html_blocks.c
    libraries/hoedown/html_smartypants.c
    libraries/hoedown/stack.c
    libraries/hoedown/stack.h
    libraries/hoedown/version.c
    libraries/hoedown/version.h
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
    libraries/fakevim/fakevim/fakevimactions.h
    libraries/fakevim/fakevim/fakevimhandler.h
    libraries/fakevim/fakevim/fakevimactions.cpp
    libraries/fakevim/fakevim/fakevimhandler.cpp
    libraries/fakevim/fakevim/utils/qtcassert.h
    libraries/fakevim/fakevim/utils/qtcassert.cpp
    services/databaseservice.cpp
    services/databaseservice.h
    services/owncloudservice.cpp
    services/owncloudservice.h
    services/updateservice.cpp
    services/updateservice.h
    services/metricsservice.cpp
    services/metricsservice.h
    services/cryptoservice.cpp
    services/cryptoservice.h
    services/scriptingservice.cpp
    services/scriptingservice.h
    libraries/piwiktracker/piwiktracker.h
    libraries/piwiktracker/piwiktracker.cpp
    libraries/botan/botan.h
    libraries/botan/botan.cpp
    libraries/botan/botanwrapper.h
    libraries/botan/botanwrapper.cpp
    libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget_p.h
    libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget.h
    libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget.cpp
    widgets/qownnotesmarkdowntextedit.cpp
    widgets/qownnotesmarkdowntextedit.h
    widgets/navigationwidget.cpp
    widgets/navigationwidget.h
    widgets/notepreviewwidget.cpp
    widgets/notepreviewwidget.h
    widgets/combobox.cpp
    widgets/combobox.h
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
    widgets/lineedit.cpp
    widgets/lineedit.h
    widgets/scriptsettingwidget.cpp
    widgets/scriptsettingwidget.h
    widgets/scriptsettingwidget.ui
    widgets/qtexteditsearchwidget.cpp
    widgets/qtexteditsearchwidget.h
    widgets/qtexteditsearchwidget.ui
    widgets/notetreewidgetitem.cpp
    widgets/notetreewidgetitem.h
    widgets/notetreewidgetitem.ui
    widgets/layoutwidget.cpp
    widgets/layoutwidget.h
    widgets/layoutwidget.ui
    utils/misc.cpp
    utils/misc.h
    utils/git.cpp
    utils/git.h
    utils/gui.cpp
    utils/gui.h
    utils/schema.cpp
    utils/schema.h
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
    entities/bookmark.h entities/bookmark.cpp)

# Translation files
SET(QON_TS_FILES
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
)

qt5_add_translation(QON_QM_FILES ${QON_TS_FILES})
add_custom_target(translations DEPENDS ${QON_QM_FILES})

if(NOT QT_TRANSLATIONS_DIR)
    # If this directory is missing, we are in a Qt5 environment.
    # Extract the qmake executable location
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    # Ask Qt5 where to put the translations
    execute_process( COMMAND ${QT5_QMAKE_EXECUTABLE} -query QT_INSTALL_TRANSLATIONS
            OUTPUT_VARIABLE qt_translations_dir OUTPUT_STRIP_TRAILING_WHITESPACE )
    # make sure we have / and not \ as qmake gives on windows
    file( TO_CMAKE_PATH "${qt_translations_dir}" qt_translations_dir)
    set( QT_TRANSLATIONS_DIR ${qt_translations_dir} CACHE PATH
            "The location of the Qt translations" FORCE)
endif()

install(FILES ${QON_QM_FILES}
        DESTINATION ${QT_TRANSLATIONS_DIR})

add_executable(QOwnNotes ${SOURCE_FILES} ${RESOURCE_ADDED} ${QON_QM_FILES})


# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES} ${Qt5Sql_INCLUDES}
        ${Qt5Svg_INCLUDES} ${Qt5Network_INCLUDES}
        ${Qt5Xml_INCLUDES} ${Qt5XmlPatterns_INCLUDES}
        ${Qt5PrintSupport_INCLUDES} ${Qt5WebSockets_INCLUDES} ${Qt5Qml_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Sql_DEFINITIONS}
        ${Qt5Svg_DEFINITIONS} ${Qt5Network_DEFINITIONS}
        ${Qt5Xml_DEFINITIONS} ${Qt5XmlPatterns_DEFINITIONS}
        ${Qt5PrintSupport_DEFINITIONS} ${Qt5WebSockets_DEFINITIONS} ${Qt5Qml_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Sql_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Svg_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Network_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Xml_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5XmlPatterns_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5PrintSupport_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5WebSockets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Qml_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++0x")

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
target_link_libraries(QOwnNotes ${Qt5Widgets_LIBRARIES} ${Qt5Sql_LIBRARIES}
        ${Qt5Svg_LIBRARIES} ${Qt5Network_LIBRARIES}
        ${Qt5Xml_LIBRARIES} ${Qt5XmlPatterns_LIBRARIES}
        ${Qt5PrintSupport_LIBRARIES} ${Qt5WebSockets_LIBRARIES} ${Qt5Qml_LIBRARIES})

#qt5_use_modules(QOwnNotes Widgets Core Gui Sql Svg Network Script Xml PrintSupport)

# Sonnet support if we can get it to run in the future
#find_package(KF5Sonnet)
#target_link_libraries(KF5::SonnetCore KF5::SonnetUi)

add_compile_definitions(QAPPLICATION_CLASS=QApplication)
