# set the following variables before adding as subdirectory into a project
# QLITEHTML_BIN_PATH - relative install path for result DLLs
# QLITEHTML_LIBRARY_PATH - relative install path for result dynamic libraries
# QLITEHTML_LIBRARY_ARCHIVE_PATH - relative install path for result archives
# (.lib, static lib), defaults to QLITEHTML_LIBRARY_PATH QLITEHTML_EXPORT -
# export name for qlitehtml QLITEHTML_DEVEL_COMPONENT - component name for
# development installation QLITEHTML_DEVEL_EXCLUDE_FROM_ALL - if development
# component should not be installed by default QLITEHTML_HEADER_PATH - relative
# install path for development headers QLITEHTML_LIBRARY_TYPE - SHARED or
# STATIC, defaults to SHARED
set(QLITEHTML_VERSION ${PROJECT_VERSION})
set(QLITEHTML_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(QLITEHTML_VERSION_COMPAT
    ${QLITEHTML_VERSION}
    CACHE STRING "qlitehtml compat version number.")

find_package(litehtml QUIET)
message("Trying to find " litehtml)
if(NOT TARGET litehtml
   AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/litehtml/CMakeLists.txt)
  set(ORIG_FPIC ${CMAKE_POSITION_INDEPENDENT_CODE})
  set(ORIG_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
  if(WIN32)
    set(LITEHTML_UTF8
        ON
        CACHE BOOL "")
  endif()
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  set(BUILD_SHARED_LIBS OFF)

  message("Adding 3rdparty/litehtml")
  add_subdirectory(3rdparty/litehtml EXCLUDE_FROM_ALL)

  set(CMAKE_POSITION_INDEPENDENT_CODE "${ORIG_FPIC}")
  set(BUILD_SHARED_LIBS ${ORIG_BUILD_SHARED_LIBS})
  # force optimized litehtml even in debug
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # except for windows
    if(NOT WIN32)
      target_compile_options(gumbo PRIVATE -O2)
      target_compile_options(litehtml PRIVATE -O2)
    endif()
  endif()
endif()

# TODO error if litehtml was not found?

if(TARGET litehtml)
  set(PUBLIC_HEADERS
      element_checkbox.h
      element_checkbox.cpp
      container_qpainter.h
      container_qpainter_p.h
      qlitehtml_global.h
      qlitehtmlwidget.h
      qlitehtmlsearchwidget.h)
  set(QLITEHTML_SOURCES container_qpainter.cpp qlitehtmlwidget.cpp
                        qlitehtmlsearchwidget.cpp qlitehtmlsearchwidget.ui)
  if(NOT QLITEHTML_LIBRARY_TYPE)
    set(QLITEHTML_LIBRARY_TYPE SHARED)
  endif()
  if(NOT QLITEHTML_LIBRARY_ARCHIVE_PATH)
    set(QLITEHTML_LIBRARY_ARCHIVE_PATH "${QLITEHTML_LIBRARY_PATH}")
  endif()

  add_library(qlitehtml ${QLITEHTML_LIBRARY_TYPE} ${PUBLIC_HEADERS}
                        ${QLITEHTML_SOURCES})

  target_include_directories(
    qlitehtml PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
  target_link_libraries(
    qlitehtml
    PUBLIC Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE litehtml)
  target_compile_definitions(
    qlitehtml PRIVATE QT_NO_JAVA_STYLE_ITERATORS QT_NO_CAST_TO_ASCII
                      QT_RESTRICTED_CAST_FROM_ASCII QT_USE_QSTRINGBUILDER)
  if("${QLITEHTML_LIBRARY_TYPE}" STREQUAL "SHARED")
    target_compile_definitions(qlitehtml PRIVATE QLITEHTML_LIBRARY)
  else()
    target_compile_definitions(qlitehtml PUBLIC QLITEHTML_STATIC_LIBRARY)
  endif()
  if(WIN32)
    target_compile_definitions(qlitehtml PRIVATE UNICODE _UNICODE
                                                 _CRT_SECURE_NO_WARNINGS)
    if(NOT BUILD_WITH_PCH)
      # Windows 8 0x0602
      target_compile_definitions(
        qlitehtml PRIVATE WINVER=0x0602 _WIN32_WINNT=0x0602 WIN32_LEAN_AND_MEAN)
    endif()
  endif()

  set_target_properties(
    qlitehtml
    PROPERTIES SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
               VERSION "${QLITEHTML_VERSION}"
               SOVERSION "${QLITEHTML_VERSION_MAJOR}"
               MACHO_CURRENT_VERSION "${QLITEHTML_VERSION}"
               MACHO_COMPATIBILITY_VERSION "${QLITEHTML_VERSION_COMPAT}"
               CXX_EXTENSIONS OFF
               CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN ON
               POSITION_INDEPENDENT_CODE ON)

  if(WIN32)
    set_target_properties(
      qlitehtml
      PROPERTIES
        SUFFIX
        "${QLITEHTML_VERSION_MAJOR}${CMAKE_${QLITEHTML_LIBRARY_TYPE}_LIBRARY_SUFFIX}"
        PREFIX ""
        IMPORT_SUFFIX "${QLITEHTML_VERSION_MAJOR}${CMAKE_IMPORT_LIBRARY_SUFFIX}"
        IMPORT_PREFIX "")
  endif()

  if(DEFINED QLITEHTML_BIN_PATH AND DEFINED QLITEHTML_LIBRARY_PATH)
    set(_EXPORT)
    if(DEFINED QLITEHTML_EXPORT)
      set(_EXPORT EXPORT ${QLITEHTML_EXPORT})
      add_library(${QLITEHTML_EXPORT}::qlitehtml ALIAS qlitehtml)
    endif()

    set(_DEVEL_COMPONENT)
    set(_NAMELINK)
    if(DEFINED QLITEHTML_DEVEL_COMPONENT)
      set(_EXCLUDE)
      if(QLITEHTML_DEVEL_EXCLUDE_FROM_ALL)
        set(_EXCLUDE EXCLUDE_FROM_ALL)
      endif()
      set(_DEVEL_COMPONENT COMPONENT ${QLITEHTML_DEVEL_COMPONENT} ${_EXCLUDE})
      set(_NAMELINK NAMELINK_SKIP)
      install(TARGETS qlitehtml LIBRARY DESTINATION "${QLITEHTML_LIBRARY_PATH}"
                                        NAMELINK_ONLY ${_DEVEL_COMPONENT})
      if(DEFINED QLITEHTML_HEADER_PATH)
        install(
          FILES ${PUBLIC_HEADERS}
          DESTINATION ${QLITEHTML_HEADER_PATH}
          ${_DEVEL_COMPONENT})
        target_include_directories(
          qlitehtml PUBLIC $<INSTALL_INTERFACE:${QLITEHTML_HEADER_PATH}>)
      endif()
    endif()

    install(
      TARGETS qlitehtml ${_EXPORT}
      RUNTIME DESTINATION "${QLITEHTML_BIN_PATH}" OPTIONAL
      LIBRARY DESTINATION "${QLITEHTML_LIBRARY_PATH}" ${_NAMELINK} OPTIONAL
      OBJECTS DESTINATION "${QLITEHTML_LIBRARY_PATH}" ${_DEVEL_COMPONENT}
                                                      OPTIONAL
      ARCHIVE DESTINATION "${QLITEHTML_LIBRARY_ARCHIVE_PATH}"
              ${_DEVEL_COMPONENT} OPTIONAL)
  endif()

endif()
