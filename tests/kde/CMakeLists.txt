cmake_minimum_required(VERSION 3.16)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(QOwnNotesKdeTests LANGUAGES CXX)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets DBus Test)
find_package(KF6GlobalAccel 6.0 REQUIRED)
find_package(KF6Notifications 6.0 REQUIRED)
add_executable(
  kde-integration-tests
  test_kdeglobalshortcutmanager.cpp
  ../../src/services/kdeglobalshortcutmanager.cpp
  ../../src/services/kdeglobalshortcutmanager.h)
target_include_directories(kde-integration-tests PRIVATE ../../src)
target_link_libraries(kde-integration-tests PRIVATE Qt6::Widgets Qt6::DBus
                                                    Qt6::Test KF6::GlobalAccel)
add_executable(
  kde-notification-tests
  test_notificationservice.cpp ../../src/services/notificationservice.cpp
  ../../src/services/notificationservice.h ../../src/kde/kde.qrc)
target_include_directories(kde-notification-tests PRIVATE ../../src)
target_compile_definitions(kde-notification-tests PRIVATE QON_KDE_INTEGRATION)
target_link_libraries(
  kde-notification-tests PRIVATE Qt6::Widgets Qt6::DBus Qt6::Test
                                 KF6::Notifications)
find_package(PlasmaActivities 6.0 REQUIRED)
add_executable(
  kde-activities-tests
  test_plasmaactivitieslibrary.cpp
  ../../src/services/plasmaactivitieslibrary.cpp
  ../../src/services/plasmaactivitieslibrary.h)
target_include_directories(kde-activities-tests PRIVATE ../../src)
target_link_libraries(kde-activities-tests PRIVATE Qt6::DBus Qt6::Test
                                                   Plasma::Activities)
find_package(KF6ColorScheme 6.0 REQUIRED)
add_executable(
  kde-colorscheme-tests
  test_kdecolorschemeservice.cpp ../../src/services/kdecolorschemeservice.cpp
  ../../src/services/kdecolorschemeservice.h)
target_include_directories(kde-colorscheme-tests PRIVATE ../../src)
target_link_libraries(kde-colorscheme-tests PRIVATE Qt6::Widgets Qt6::Test
                                                    KF6::ColorScheme)
find_package(KF6WindowSystem 6.0 REQUIRED)
add_executable(
  kde-windowactivation-tests
  test_windowactivationservice.cpp
  ../../src/services/windowactivationservice.cpp
  ../../src/services/windowactivationservice.h)
target_include_directories(kde-windowactivation-tests PRIVATE ../../src)
target_compile_definitions(kde-windowactivation-tests
                           PRIVATE QON_KDE_INTEGRATION)
target_link_libraries(kde-windowactivation-tests PRIVATE Qt6::Widgets Qt6::Test
                                                         KF6::WindowSystem)
find_package(KF6Purpose 6.0 REQUIRED)
add_executable(
  kde-share-tests test_kdesharemenu.cpp ../../src/services/kdesharemenu.cpp
                  ../../src/services/kdesharemenu.h)
target_include_directories(kde-share-tests PRIVATE ../../src)
target_link_libraries(kde-share-tests PRIVATE Qt6::Widgets Qt6::Test
                                              KF6::PurposeWidgets)
# KRunner support only needs Qt D-Bus
add_executable(
  kde-krunner-tests
  test_krunnerservice.cpp ../../src/services/krunnerservice.cpp
  ../../src/services/krunnerservice.h)
target_include_directories(kde-krunner-tests PRIVATE ../../src)
target_compile_definitions(
  kde-krunner-tests
  PRIVATE
    KRUNNER_PLUGIN_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../../src/krunner/org.qownnotes.QOwnNotes.desktop"
)
target_link_libraries(kde-krunner-tests PRIVATE Qt6::DBus Qt6::Test)
enable_testing()
foreach(
  test
  kde-integration-tests
  kde-notification-tests
  kde-activities-tests
  kde-colorscheme-tests
  kde-windowactivation-tests
  kde-share-tests
  kde-krunner-tests)
  add_test(
    NAME ${test}
    COMMAND
      dbus-run-session -- ${CMAKE_COMMAND} -E env QON_KDE_TEST_BUS=1
      QT_QPA_PLATFORM=minimal QT_QPA_PLATFORMTHEME= QT_STYLE_OVERRIDE=Fusion
      QT_PLUGIN_PATH= $<TARGET_FILE:${test}>)
endforeach()
