cmake_minimum_required(VERSION 3.2.2)
project(QOwnNotes)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package( Qt5Core 5.3 REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Sql REQUIRED )
find_package( Qt5Svg REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5Xml REQUIRED )
find_package( Qt5PrintSupport REQUIRED )
find_package( Qt5Qml REQUIRED )

qt5_wrap_ui(dialogs/ui_settingsdialog.h settingsdialog.ui)
qt5_wrap_ui(dialogs/ui_tododialog.h tododialog.ui)
qt5_wrap_ui(dialogs/ui_aboutdialog.h aboutdialog.ui)
qt5_wrap_ui(dialogs/ui_logdialog.h logdialog.ui)
qt5_wrap_ui(dialogs/ui_trashdialog.h trashdialog.ui)
qt5_wrap_ui(dialogs/ui_linkdialog.h linkdialog.ui)
qt5_wrap_ui(dialogs/ui_updatedialog.h updatedialog.ui)
qt5_wrap_ui(dialogs/ui_notediffdialog.h notediffdialog.ui)
qt5_wrap_ui(dialogs/ui_versiondialog.h versiondialog.ui)
qt5_wrap_ui(dialogs/ui_welcomedialog.h welcomedialog.ui)
qt5_wrap_ui(dialogs/ui_tagadddialog.h tagadddialog.ui)
qt5_wrap_ui(libraries/qmarkdowntextedit/ui_qtexteditsearchwidget.h
        libraries/qmarkdowntextedit/qtexteditsearchwidget.ui)

set(RESOURCE_FILES
        breeze-qownnotes.qrc
        breeze-dark-qownnotes.qrc
        qownnotes.qrc
        demonotes.qrc
        images.qrc
        texts.qrc
        libraries/qmarkdowntextedit/media.qrc
        libraries/qdarkstyle/style.qrc
        )

qt5_add_resources(RESOURCE_ADDED ${RESOURCE_FILES})

set(SOURCE_FILES
    dialogs/welcomedialog.cpp
    dialogs/welcomedialog.h
    dialogs/welcomedialog.ui
    dialogs/aboutdialog.cpp
    dialogs/aboutdialog.h
    dialogs/aboutdialog.ui
    dialogs/logdialog.cpp
    dialogs/logdialog.h
    dialogs/logdialog.ui
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
    entities/calendaritem.cpp
    entities/calendaritem.h
    entities/note.cpp
    entities/note.h
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
    api/noteapi.cpp
    api/noteapi.h
    api/tagapi.cpp
    api/tagapi.h
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
    libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_definitions.h
    libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_parser.c
    libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_parser.h
    libraries/qmarkdowntextedit/markdownhighlighter.cpp
    libraries/qmarkdowntextedit/markdownhighlighter.h
    libraries/qmarkdowntextedit/qmarkdowntextedit.cpp
    libraries/qmarkdowntextedit/qmarkdowntextedit.h
    libraries/qmarkdowntextedit/qtexteditsearchwidget.cpp
    libraries/qmarkdowntextedit/qtexteditsearchwidget.h
    libraries/qmarkdowntextedit/qtexteditsearchwidget.ui
    libraries/simplecrypt/simplecrypt.cpp
    libraries/simplecrypt/simplecrypt.h
    libraries/versionnumber/versionnumber.cpp
    libraries/versionnumber/versionnumber.h
    libraries/singleapplication/singleapplication.h
    libraries/singleapplication/singleapplication.cpp
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
    widgets/qownnotesmarkdowntextedit.cpp
    widgets/qownnotesmarkdowntextedit.h
    widgets/navigationwidget.cpp
    widgets/navigationwidget.h
    widgets/notepreviewwidget.cpp
    widgets/notepreviewwidget.h
    widgets/combobox.cpp
    widgets/combobox.h
    utils/misc.cpp
    utils/misc.h
    build_number.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    release.h
    version.h
)

# Translation files
SET(TRANSLATION_FILES
    languages/QOwnNotes_en.ts
    languages/QOwnNotes_de.ts
    languages/QOwnNotes_fr.ts
    languages/QOwnNotes_pl.ts
    languages/QOwnNotes_zh.ts
    languages/QOwnNotes_ru.ts
    languages/QOwnNotes_pt.ts
    languages/QOwnNotes_es.ts
    languages/QOwnNotes_nl.ts
    languages/QOwnNotes_hu.ts
    languages/QOwnNotes_ja.ts
    languages/QOwnNotes_it.ts
)

add_executable(QOwnNotes ${SOURCE_FILES} ${RESOURCE_ADDED})


# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES} ${Qt5Sql_INCLUDES}
        ${Qt5Svg_INCLUDES} ${Qt5Network_INCLUDES}
        ${Qt5Xml_INCLUDES} ${Qt5PrintSupport_INCLUDES} ${Qt5Qml_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Sql_DEFINITIONS}
        ${Qt5Svg_DEFINITIONS} ${Qt5Network_DEFINITIONS}
        ${Qt5Xml_DEFINITIONS}
        ${Qt5PrintSupport_DEFINITIONS} ${Qt5Qml_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Sql_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Svg_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Network_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Xml_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5PrintSupport_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++0x")

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
target_link_libraries(QOwnNotes ${Qt5Widgets_LIBRARIES} ${Qt5Sql_LIBRARIES}
        ${Qt5Svg_LIBRARIES} ${Qt5Network_LIBRARIES}
        ${Qt5Xml_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} ${Qt5Qml_LIBRARIES})

#qt5_use_modules(QOwnNotes Widgets Core Gui Sql Svg Network Script Xml PrintSupport)
