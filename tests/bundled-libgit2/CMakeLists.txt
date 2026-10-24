cmake_minimum_required(VERSION 3.21)
project(BundledLibGit2Test LANGUAGES C CXX)

# Load Qt first, as QOwnNotes does before configuring bundled libgit2.
find_package(Qt6 REQUIRED COMPONENTS Core)
include(../../src/cmake/BundledLibGit2.cmake)

add_executable(bundled-libgit2-test main.c)
target_link_libraries(bundled-libgit2-test PRIVATE libgit2package)

enable_testing()
add_test(NAME bundled-libgit2 COMMAND bundled-libgit2-test
                                      "${CMAKE_CURRENT_BINARY_DIR}/repository")
