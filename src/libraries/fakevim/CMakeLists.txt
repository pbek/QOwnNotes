cmake_minimum_required(VERSION 3.5)
project(fakevim VERSION 0.0.1)

# Library name
set(bin fakevim)

# Public headers
set(${bin}_public_headers fakevim/fakevimactions.h fakevim/fakevimhandler.h)

# Source files common for all platforms
set(${bin}_sources
    fakevim/fakevimactions.cpp fakevim/fakevimhandler.cpp
    fakevim/utils/hostosinfo.cpp fakevim/utils/osspecificaspects.h
    fakevim/utils/qtcassert.cpp ${${bin}_public_headers})

# Required pkg-config packages
set(${bin}_pkg_config_requires)

include(cmake/cxx17.cmake)
include(cmake/library.cmake)
include(cmake/qt.cmake)
include(cmake/pkg-config.cmake)

# Files with Q_OBJECT macros to pass to moc utility
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(QT_VERSION_MAJOR EQUAL 6)
  qt6_wrap_cpp(${bin}_mocced "fakevim/fakevimhandler.h")
else()
  qt5_wrap_cpp(${bin}_mocced "fakevim/fakevimhandler.h")
endif()
target_sources(${bin} PRIVATE ${${bin}_mocced})

target_compile_definitions(
  ${bin} PRIVATE QT_NO_CAST_TO_ASCII QT_RESTRICTED_CAST_FROM_ASCII
                 QTCREATOR_UTILS_STATIC_LIB)

option(BUILD_TESTS "Build tests")
if(BUILD_TESTS)
  message(STATUS "Building tests")

  find_package(Qt5Test REQUIRED)

  add_executable(fakevim_test tests/fakevim_test.cpp tests/fakevimplugin.h
                              example/editor.cpp)
  set_property(TARGET fakevim_test PROPERTY AUTOMOC ON)
  target_link_libraries(fakevim_test fakevim Qt5::Widgets Qt5::Test)

  target_include_directories(fakevim_test PRIVATE ${CMAKE_SOURCE_DIR}/fakevim
                                                  ${CMAKE_SOURCE_DIR})

  add_test(fakevim_test fakevim_test)
  enable_testing()
endif()

option(BUILD_EXAMPLE "Build example")
if(BUILD_EXAMPLE)
  message(STATUS "Building example")

  add_executable(fakevim_example example/main.cpp example/editor.cpp)
  set_property(TARGET fakevim_example PROPERTY AUTOMOC ON)

  target_link_libraries(fakevim_example fakevim)
  target_link_libraries(fakevim_example Qt5::Widgets)
endif()
