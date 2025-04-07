cmake_minimum_required(VERSION 3.5)
project(DiffMatchPatchLib)

add_library(diff_match_patch STATIC diff_match_patch.h diff_match_patch.cpp)

add_library(Google::DiffMatchPatch ALIAS diff_match_patch)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

target_include_directories(diff_match_patch PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(diff_match_patch PUBLIC Qt${QT_VERSION_MAJOR}::Core)
