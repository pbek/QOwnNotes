cmake_minimum_required(VERSION 3.11)

project(litehtml LANGUAGES C CXX)
include(CTest)
enable_testing()

# Soname MAJOR is incremented when symbols are removed or changed in an
# incompatible way MINOR is incremented when new symbols are added
set(PROJECT_MAJOR 0)
set(PROJECT_MINOR 0)

option(EXTERNAL_GUMBO
       "Link against external gumbo instead of shipping a bundled copy" OFF)
if(WIN32)
  option(EXTERNAL_XXD "Use external xxd" OFF)
else()
  option(EXTERNAL_XXD "Use external xxd" ON)
endif()

if(NOT EXTERNAL_GUMBO)
  add_subdirectory(src/gumbo)
endif()

set(SOURCE_LITEHTML
    src/background.cpp
    src/box.cpp
    src/codepoint.cpp
    src/context.cpp
    src/css_length.cpp
    src/css_selector.cpp
    src/document.cpp
    src/el_anchor.cpp
    src/el_base.cpp
    src/el_before_after.cpp
    src/el_body.cpp
    src/el_break.cpp
    src/el_cdata.cpp
    src/el_comment.cpp
    src/el_div.cpp
    src/element.cpp
    src/el_font.cpp
    src/el_image.cpp
    src/el_link.cpp
    src/el_li.cpp
    src/el_para.cpp
    src/el_script.cpp
    src/el_space.cpp
    src/el_style.cpp
    src/el_table.cpp
    src/el_td.cpp
    src/el_text.cpp
    src/el_title.cpp
    src/el_tr.cpp
    src/html.cpp
    src/html_tag.cpp
    src/iterators.cpp
    src/media_query.cpp
    src/style.cpp
    src/stylesheet.cpp
    src/table.cpp
    src/tstring_view.cpp
    src/url.cpp
    src/url_path.cpp
    src/utf8_strings.cpp
    src/web_color.cpp
    src/num_cvt.cpp
    src/strtod.cpp)

set(HEADER_LITEHTML
    include/litehtml.h
    include/litehtml/attributes.h
    include/litehtml/background.h
    include/litehtml/borders.h
    include/litehtml/box.h
    include/litehtml/codepoint.h
    include/litehtml/context.h
    include/litehtml/css_length.h
    include/litehtml/css_margins.h
    include/litehtml/css_offsets.h
    include/litehtml/css_position.h
    include/litehtml/css_selector.h
    include/litehtml/document.h
    include/litehtml/el_anchor.h
    include/litehtml/el_base.h
    include/litehtml/el_before_after.h
    include/litehtml/el_body.h
    include/litehtml/el_break.h
    include/litehtml/el_cdata.h
    include/litehtml/el_comment.h
    include/litehtml/el_div.h
    include/litehtml/el_font.h
    include/litehtml/el_image.h
    include/litehtml/el_link.h
    include/litehtml/el_para.h
    include/litehtml/el_script.h
    include/litehtml/el_space.h
    include/litehtml/el_style.h
    include/litehtml/el_table.h
    include/litehtml/el_td.h
    include/litehtml/el_text.h
    include/litehtml/el_title.h
    include/litehtml/el_tr.h
    include/litehtml/element.h
    include/litehtml/html.h
    include/litehtml/html_tag.h
    include/litehtml/iterators.h
    include/litehtml/media_query.h
    include/litehtml/os_types.h
    include/litehtml/style.h
    include/litehtml/stylesheet.h
    include/litehtml/table.h
    include/litehtml/tstring_view.h
    include/litehtml/types.h
    include/litehtml/url.h
    include/litehtml/url_path.h
    include/litehtml/utf8_strings.h
    include/litehtml/web_color.h
    include/litehtml/num_cvt.h)

set(TEST_LITEHTML
    containers/test/container_test.cpp
    test/codepoint_test.cpp
    test/contextTest.cpp
    test/cssTest.cpp
    test/documentTest.cpp
    test/layoutGlobalTest.cpp
    test/mediaQueryTest.cpp
    test/tstring_view_test.cpp
    test/url_test.cpp
    test/url_path_test.cpp
    test/webColorTest.cpp)

set(PROJECT_LIB_VERSION ${PROJECT_MAJOR}.${PROJECT_MINOR}.0)
set(PROJECT_SO_VERSION ${PROJECT_MAJOR})

add_library(${PROJECT_NAME} ${SOURCE_LITEHTML})
set_target_properties(
  ${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_LIB_VERSION}
                             SOVERSION ${PROJECT_SO_VERSION})

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES CXX_STANDARD 11
             C_STANDARD 99
             PUBLIC_HEADER "${HEADER_LITEHTML}")

# Export litehtml includes.
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_include_directories(${PROJECT_NAME} PRIVATE include/${PROJECT_NAME})

option(LITEHTML_UTF8 "Build litehtml with UTF-8 text conversion functions." OFF)
if(LITEHTML_UTF8)
  target_compile_definitions(${PROJECT_NAME} PUBLIC LITEHTML_UTF8)
endif()

# Gumbo
target_link_libraries(${PROJECT_NAME} PUBLIC gumbo)

# install and export
install(
  TARGETS ${PROJECT_NAME}
  EXPORT litehtmlTargets
  RUNTIME DESTINATION bin COMPONENT libraries
  ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT libraries
  LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT libraries
  PUBLIC_HEADER DESTINATION include/litehtml)
install(FILES cmake/litehtmlConfig.cmake
        DESTINATION lib${LIB_SUFFIX}/cmake/litehtml)
install(
  EXPORT litehtmlTargets
  FILE litehtmlTargets.cmake
  DESTINATION lib${LIB_SUFFIX}/cmake/litehtml)

# Binary Master.css
if(NOT EXTERNAL_XXD)
  add_executable(xxd xxd/xxd.c)
  set(XXD_COMMAND $<TARGET_FILE:xxd>)
else()
  find_program(XXD_COMMAND xxd)
endif()
if(WIN32)
  file(TO_NATIVE_PATH ${XXD_COMMAND} XXD_COMMAND)
  file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css
       MASTER_FILE)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/master.css.inc
    COMMAND type ${MASTER_FILE} | "${XXD_COMMAND}" -i >
            ${CMAKE_CURRENT_SOURCE_DIR}/src/master.css.inc)
else()
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/master.css.inc
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css | xxd -i >
            ${CMAKE_CURRENT_SOURCE_DIR}/src/master.css.inc)
endif()
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/master.css.inc
                            PROPERTIES GENERATED TRUE)

# Tests

set(BUILD_TESTING OFF)
if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker
  # settings
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  endif()

  enable_testing()

  set(TEST_NAME ${PROJECT_NAME}_tests)

  add_executable(${TEST_NAME} ${TEST_LITEHTML}
                              ${CMAKE_CURRENT_SOURCE_DIR}/src/master.css.inc)

  set_target_properties(
    ${TEST_NAME}
    PROPERTIES CXX_STANDARD 11
               C_STANDARD 99
               PUBLIC_HEADER "${HEADER_LITEHTML}")

  target_include_directories(${TEST_NAME}
                             PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/containers)

  target_link_libraries(${TEST_NAME} ${PROJECT_NAME} gtest_main)

  include(GoogleTest)
  gtest_discover_tests(${TEST_NAME})
endif()

# set(TEST_NAME ${PROJECT_NAME}_tests) add_executable(${TEST_NAME}
# ${TEST_LITEHTML} ${CMAKE_CURRENT_SOURCE_DIR}/src/master.css.inc)
# set_target_properties(${TEST_NAME} PROPERTIES CXX_STANDARD 11 C_STANDARD 99
# PUBLIC_HEADER "${HEADER_LITEHTML}" ) target_include_directories(${TEST_NAME}
# PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/containers)
# target_link_libraries(${TEST_NAME} PRIVATE ${PROJECT_NAME}) # tests
# add_test(NAME contextTest COMMAND ${TEST_NAME} 1) add_test(NAME cssTest
# COMMAND ${TEST_NAME} 2) add_test(NAME documentTest COMMAND ${TEST_NAME} 3)
# add_test(NAME layoutGlobalTest COMMAND ${TEST_NAME} 4) add_test(NAME
# mediaQueryTest COMMAND ${TEST_NAME} 5) add_test(NAME webColorTest COMMAND
# ${TEST_NAME} 6) endif()
