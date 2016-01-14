#cmake_minimum_required(VERSION 2.8.11)
cmake_minimum_required(VERSION 3.2.2)
project(QOwnNotes)

#[[
include_directories("libraries")
include_directories("dialogs")
include_directories("widgets")
include_directories("entities")
]]

#include_directories("dialogs")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package( Qt5Core 5.3 REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Sql REQUIRED )
find_package( Qt5Svg REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5Script REQUIRED )
find_package( Qt5Xml REQUIRED )
find_package( Qt5PrintSupport REQUIRED )

qt5_wrap_ui(dialogs/ui_settingsdialog.h settingsdialog.ui)
qt5_wrap_ui(dialogs/ui_tododialog.h tododialog.ui)
qt5_wrap_ui(dialogs/ui_aboutdialog.h aboutdialog.ui)
qt5_wrap_ui(dialogs/ui_trashdialog.h trashdialog.ui)
qt5_wrap_ui(dialogs/ui_linkdialog.h linkdialog.ui)
qt5_wrap_ui(dialogs/ui_updatedialog.h updatedialog.ui)
qt5_wrap_ui(dialogs/ui_notediffdialog.h notediffdialog.ui)
qt5_wrap_ui(dialogs/ui_versiondialog.h versiondialog.ui)

#file (GLOB Resources *.qrc)
#qt5_add_resources (RESOURCE_ADDED ${Resources})

#set(RESOURCE ${CMAKE_SOURCE_DIR}/adwaita.qrc)
#qt5_add_resources(RESOURCE_ADDED ${RESOURCE})
set(RESOURCE_FILES
        adwaita.qrc
        breeze.qrc
        demonotes.qrc
        high-contrast.qrc
        images.qrc
        texts.qrc
        ubuntu-mobile.qrc
        libraries/qmarkdowntextedit/media.qrc
        )
qt5_add_resources(RESOURCE_ADDED ${RESOURCE_FILES})


set(SOURCE_FILES
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
    dialogs/updatedialog.cpp
    dialogs/updatedialog.h
    dialogs/updatedialog.ui
    dialogs/versiondialog.cpp
    dialogs/versiondialog.h
    dialogs/versiondialog.ui
    entities/calendaritem.cpp
    entities/calendaritem.h
    entities/note.cpp
    entities/note.h
    entities/notehistory.cpp
    entities/notehistory.h
    helpers/htmlentities.cpp
    helpers/htmlentities.h
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
    libraries/qmarkdowntextedit/highlighter.cpp
    libraries/qmarkdowntextedit/highlighter.h
    libraries/qmarkdowntextedit/qmarkdowntextedit.cpp
    libraries/qmarkdowntextedit/qmarkdowntextedit.h
    libraries/qmarkdowntextedit/qtexteditsearchwidget.cpp
    libraries/qmarkdowntextedit/qtexteditsearchwidget.h
    libraries/simplecrypt/simplecrypt.cpp
    libraries/simplecrypt/simplecrypt.h
    libraries/versionnumber/versionnumber.cpp
    libraries/versionnumber/versionnumber.h
    services/databaseservice.cpp
    services/databaseservice.h
    services/owncloudservice.cpp
    services/owncloudservice.h
    services/updateservice.cpp
    services/updateservice.h
    widgets/qownnotesmarkdowntextedit.cpp
    widgets/qownnotesmarkdowntextedit.h
    build_number.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    release.h
    version.h
)

add_executable(QOwnNotes ${SOURCE_FILES} ${RESOURCE_ADDED})


# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES} ${Qt5Sql_INCLUDES} ${Qt5Svg_INCLUDES} ${Qt5Network_INCLUDES} ${Qt5Script_INCLUDES} ${Qt5Xml_INCLUDES} ${Qt5PrintSupport_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Sql_DEFINITIONS} ${Qt5Svg_DEFINITIONS} ${Qt5Network_DEFINITIONS} ${Qt5Script_DEFINITIONS} ${Qt5Xml_DEFINITIONS} ${Qt5PrintSupport_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Sql_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Svg_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Network_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Script_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5Xml_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${Qt5PrintSupport_EXECUTABLE_COMPILE_FLAGS}")

#add_executable(QOwnNotes main.cpp mainwindow.cpp)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
target_link_libraries(QOwnNotes ${Qt5Widgets_LIBRARIES} ${Qt5Sql_LIBRARIES} ${Qt5Svg_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Script_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})

#qt5_use_modules(QOwnNotes Widgets Core Gui Sql Svg Network Script Xml PrintSupport)
