cmake_minimum_required(VERSION 3.16)
project(QOwnNotesTests LANGUAGES C CXX)

# Unit tests for QOwnNotes
#
# Usage: cmake -S tests -B build-tests -DCMAKE_BUILD_TYPE=Debug cmake --build
# build-tests --parallel ./build-tests/bin/tests -platform minimal (or: ctest
# --test-dir build-tests --output-on-failure)

# BEGIN options
option(QON_TESTS_COVERAGE "Build the unit tests with gcov code coverage" OFF)
# END options

set(QON_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)
get_filename_component(QON_SRC_DIR ${QON_SRC_DIR} ABSOLUTE)
set(QON_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests)

# Pick the Qt major version. QON_QT6_BUILD can be used to force a version,
# otherwise Qt6 is preferred and Qt5 is used as fallback.
if(DEFINED QON_QT6_BUILD)
  if(QON_QT6_BUILD)
    set(QT_VERSION_MAJOR 6)
  else()
    set(QT_VERSION_MAJOR 5)
  endif()
else()
  find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
endif()
set(QT_DEFAULT_MAJOR_VERSION ${QT_VERSION_MAJOR})
message(STATUS "Building unit tests with Qt ${QT_VERSION_MAJOR}")

# Qt6 requires C++17, Qt5 builds are kept at C++11 to catch compatibility issues
# in the legacy Qt5 builds
if(QT_VERSION_MAJOR EQUAL 6)
  set(CMAKE_CXX_STANDARD 17)
else()
  set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message(STATUS "ccache found")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

find_package(
  Qt${QT_VERSION_MAJOR} REQUIRED
  COMPONENTS Core
             Gui
             Widgets
             Test
             Sql
             Xml
             Network
             Qml
             PrintSupport
             Concurrent)

#
# Bundled libraries (out-of-tree, so they need an explicit binary directory)
#

# Botan (the tests always use the bundled Botan, like the qmake build did)
add_subdirectory(${QON_SRC_DIR}/libraries/botan libraries/botan
                 EXCLUDE_FROM_ALL)

# qtkeychain, configured the same way as in src/CMakeLists.txt
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_TRANSLATIONS
    OFF
    CACHE BOOL "" FORCE)
set(BUILD_TEST_APPLICATION
    OFF
    CACHE BOOL "" FORCE)
set(BUILD_QTQUICK_DEMO
    OFF
    CACHE BOOL "" FORCE)
set(BUILD_TESTING
    OFF
    CACHE BOOL "" FORCE)
if(QT_VERSION_MAJOR EQUAL 6)
  set(BUILD_WITH_QT5
      OFF
      CACHE BOOL "" FORCE)
else()
  set(BUILD_WITH_QT5
      ON
      CACHE BOOL "" FORCE)
endif()
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(LIBSECRET libsecret-1)
endif()
set(LIBSECRET_SUPPORT
    ${LIBSECRET_FOUND}
    CACHE BOOL "" FORCE)
add_subdirectory(${QON_SRC_DIR}/libraries/qtkeychain libraries/qtkeychain
                 EXCLUDE_FROM_ALL)
if(QT_VERSION_MAJOR EQUAL 5 AND Qt5Core_VERSION VERSION_LESS 5.15.0)
  # qtkeychain references QDataStream::Qt_5_15, which is unavailable in legacy
  # Qt 5 builds.
  target_compile_definitions(qt5keychain PRIVATE Qt_5_15=Qt_5_0)
endif()

add_subdirectory(${QON_SRC_DIR}/libraries/diff_match_patch
                 libraries/diff_match_patch EXCLUDE_FROM_ALL)

#
# Test executable
#

# Don't use globbing, so the list of tested sources stays explicit
set(TEST_SOURCE_FILES
    ${QON_TESTS_DIR}/main.cpp
    ${QON_TESTS_DIR}/testcases/app/test_cryptoservice.cpp
    ${QON_TESTS_DIR}/testcases/app/test_cryptoservice.h
    ${QON_TESTS_DIR}/testcases/app/test_htmlentities.cpp
    ${QON_TESTS_DIR}/testcases/app/test_htmlentities.h
    ${QON_TESTS_DIR}/testcases/app/test_languagetool.cpp
    ${QON_TESTS_DIR}/testcases/app/test_languagetool.h
    ${QON_TESTS_DIR}/testcases/app/test_metricsservice.cpp
    ${QON_TESTS_DIR}/testcases/app/test_metricsservice.h
    ${QON_TESTS_DIR}/testcases/app/test_navigationwidget.cpp
    ${QON_TESTS_DIR}/testcases/app/test_navigationwidget.h
    ${QON_TESTS_DIR}/testcases/app/test_network.cpp
    ${QON_TESTS_DIR}/testcases/app/test_network.h
    ${QON_TESTS_DIR}/testcases/app/test_notes.cpp
    ${QON_TESTS_DIR}/testcases/app/test_notes.h
    ${QON_TESTS_DIR}/testcases/app/test_qmarkdowntextedit.cpp
    ${QON_TESTS_DIR}/testcases/app/test_qmarkdowntextedit.h
    ${QON_TESTS_DIR}/testcases/app/test_script.cpp
    ${QON_TESTS_DIR}/testcases/app/test_script.h
    ${QON_TESTS_DIR}/testcases/app/test_settingsservice.cpp
    ${QON_TESTS_DIR}/testcases/app/test_settingsservice.h
    ${QON_TESTS_DIR}/testcases/app/test_utilsmisc.cpp
    ${QON_TESTS_DIR}/testcases/app/test_utilsmisc.h
    ${QON_TESTS_DIR}/testcases/app/mocklanguagetoolserver.cpp
    ${QON_TESTS_DIR}/testcases/app/mocklanguagetoolserver.h)

# Application sources that are needed by the tests
set(APP_SOURCE_FILES
    version.h
    release.h
    api/noteapi.cpp
    api/noteapi.h
    api/notesubfolderapi.cpp
    api/notesubfolderapi.h
    api/scriptapi.cpp
    api/scriptapi.h
    api/tagapi.cpp
    api/tagapi.h
    dialogs/filedialog.cpp
    dialogs/filedialog.h
    dialogs/joplinimportdialog.cpp
    dialogs/joplinimportdialog.h
    dialogs/joplinimportdialog.ui
    dialogs/masterdialog.cpp
    dialogs/masterdialog.h
    entities/bookmark.cpp
    entities/bookmark.h
    entities/calendaritem.cpp
    entities/calendaritem.h
    entities/cloudconnection.cpp
    entities/cloudconnection.h
    entities/colormode.cpp
    entities/colormode.h
    entities/commandsnippet.cpp
    entities/commandsnippet.h
    entities/note.cpp
    entities/note.h
    entities/notefolder.cpp
    entities/notefolder.h
    entities/notesubfolder.cpp
    entities/notesubfolder.h
    entities/script.cpp
    entities/script.h
    entities/tag.cpp
    entities/tag.h
    entities/trashitem.cpp
    entities/trashitem.h
    helpers/clientproxy.cpp
    helpers/clientproxy.h
    helpers/codetohtmlconverter.cpp
    helpers/codetohtmlconverter.h
    helpers/htmlentities.cpp
    helpers/htmlentities.h
    helpers/nomenuiconstyle.cpp
    helpers/nomenuiconstyle.h
    libraries/md4c/src/entity.c
    libraries/md4c/src/entity.h
    libraries/md4c/src/md4c-html.c
    libraries/md4c/src/md4c-html.h
    libraries/md4c/src/md4c.c
    libraries/md4c/src/md4c.h
    libraries/piwiktracker/piwiktracker.cpp
    libraries/piwiktracker/piwiktracker.h
    libraries/qmarkdowntextedit/linenumberarea.h
    libraries/qmarkdowntextedit/markdownhighlighter.cpp
    libraries/qmarkdowntextedit/markdownhighlighter.h
    libraries/qmarkdowntextedit/qmarkdowntextedit.cpp
    libraries/qmarkdowntextedit/qmarkdowntextedit.h
    libraries/qmarkdowntextedit/qownlanguagedata.cpp
    libraries/qmarkdowntextedit/qownlanguagedata.h
    libraries/qmarkdowntextedit/qplaintexteditsearchwidget.cpp
    libraries/qmarkdowntextedit/qplaintexteditsearchwidget.h
    libraries/qmarkdowntextedit/qplaintexteditsearchwidget.ui
    libraries/simplecrypt/simplecrypt.cpp
    libraries/simplecrypt/simplecrypt.h
    libraries/versionnumber/versionnumber.cpp
    libraries/versionnumber/versionnumber.h
    services/cloudservice.cpp
    services/cloudservice.h
    services/cryptoservice.cpp
    services/cryptoservice.h
    services/databaseservice.cpp
    services/databaseservice.h
    services/languagetoolclient.cpp
    services/languagetoolclient.h
    services/languagetooltypes.h
    services/metricsservice.cpp
    services/metricsservice.h
    services/scriptingservice.cpp
    services/scriptingservice.h
    services/settingsservice.cpp
    services/settingsservice.h
    threads/scriptthread.cpp
    threads/scriptthread.h
    utils/gui.cpp
    utils/gui.h
    utils/listutils.cpp
    utils/listutils.h
    utils/misc.cpp
    utils/misc.h
    utils/schema.cpp
    utils/schema.h
    widgets/logwidget.cpp
    widgets/logwidget.h
    widgets/navigationwidget.cpp
    widgets/navigationwidget.h)
list(TRANSFORM APP_SOURCE_FILES PREPEND ${QON_SRC_DIR}/)

add_executable(tests ${TEST_SOURCE_FILES} ${APP_SOURCE_FILES})

set_target_properties(
  tests
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON
             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(tests PRIVATE ${QON_SRC_DIR} ${QON_TESTS_DIR})

target_compile_definitions(
  tests PRIVATE INTEGRATION_TESTS ELPP_STL_LOGGING
                ELPP_DISABLE_DEFAULT_CRASH_HANDLING LANGUAGETOOL_ENABLED)

if(MINGW OR MSVC)
  # Qt 5 does not define UNICODE for CMake, only qmake does that automatically
  target_compile_definitions(tests PRIVATE UNICODE _UNICODE)
endif()

if(MSVC)
  target_compile_definitions(tests PRIVATE NOMINMAX)
endif()

target_link_libraries(
  tests
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Gui
          Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::Test
          Qt${QT_VERSION_MAJOR}::Sql
          Qt${QT_VERSION_MAJOR}::Xml
          Qt${QT_VERSION_MAJOR}::Network
          Qt${QT_VERSION_MAJOR}::Qml
          Qt${QT_VERSION_MAJOR}::PrintSupport
          Qt${QT_VERSION_MAJOR}::Concurrent
          Botan::Botan
          Google::DiffMatchPatch
          qt${QT_VERSION_MAJOR}keychain)

# CryptoService checks via D-Bus if a secret service is available on Linux
if(UNIX AND NOT APPLE)
  find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DBus)
  target_link_libraries(tests PRIVATE Qt${QT_VERSION_MAJOR}::DBus)
endif()

if(QON_TESTS_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(tests PRIVATE -g -O0 --coverage)
    target_link_options(tests PRIVATE --coverage)
  else()
    message(WARNING "Code coverage is only supported with GCC and Clang")
  endif()
endif()

#
# CTest integration
#

enable_testing()
add_test(NAME unit_tests COMMAND tests -platform minimal)
