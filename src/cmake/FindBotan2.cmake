#[[
FindBotan
---------

Searching system installed Botan 2 library using PkgConfig
#]]

if(MSVC OR MINGW)
  message(FATAL_ERROR "Using system Botan 2 library in Window not supported")
endif()

if(TARGET Botan2::Botan2)
  message(STATUS "Target Botan::Botan already exists. Skipping searching")
  return()
endif()

find_package(PkgConfig REQUIRED QUIET)

pkg_check_modules(Botan2 REQUIRED IMPORTED_TARGET botan-2)

add_library(Botan2::Botan2 ALIAS PkgConfig::Botan2)
