#[[
FindBotan
---------

Searching system installed Botan 3 library using PkgConfig
#]]

if(MSVC OR MINGW)
  message(FATAL_ERROR "Using system Botan 3 library in Window not supported")
endif()

if(TARGET Botan3::Botan3)
  message(STATUS "Target Botan::Botan already exists. Skipping searching")
  return()
endif()

find_package(PkgConfig REQUIRED QUIET)

pkg_check_modules(Botan3 REQUIRED IMPORTED_TARGET botan-3)

add_library(Botan3::Botan3 ALIAS PkgConfig::Botan3)
