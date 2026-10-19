cmake_minimum_required(VERSION 3.16)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(QOwnNotesKdeTests LANGUAGES CXX)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets DBus Test)
find_package(KF6GlobalAccel 6.0 REQUIRED)
add_executable(
  kde-integration-tests
  test_kdeglobalshortcutmanager.cpp
  ../../src/services/kdeglobalshortcutmanager.cpp
  ../../src/services/kdeglobalshortcutmanager.h)
target_include_directories(kde-integration-tests PRIVATE ../../src)
target_link_libraries(kde-integration-tests PRIVATE Qt6::Widgets Qt6::DBus
                                                    Qt6::Test KF6::GlobalAccel)
enable_testing()
add_test(
  NAME kde-integration-tests
  COMMAND
    dbus-run-session -- ${CMAKE_COMMAND} -E env QON_KDE_TEST_BUS=1
    QT_QPA_PLATFORM=minimal QT_QPA_PLATFORMTHEME= QT_STYLE_OVERRIDE=Fusion
    QT_PLUGIN_PATH= $<TARGET_FILE:kde-integration-tests>)
