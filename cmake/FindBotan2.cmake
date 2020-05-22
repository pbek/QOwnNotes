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
find_package(PackageHandleStandardArgs REQUIRED QUIET)


pkg_check_modules(Botan2
    botan-2
)

find_library(Botan2_FullLibraryPath
    ${Botan2_LIBRARIES}
    PATHS ${Botan2_LIBRARY_DIRS}
    NO_DEFAULT_PATH
)

find_package_handle_standard_args(Botan2
    REQUIRED_VARS Botan2_LIBRARIES Botan2_INCLUDE_DIRS
    VERSION_VAR Botan2_VERSION
)
message("Botan2_INCLUDE_DIRS ${Botan2_INCLUDE_DIRS}")

if(Botan2_FOUND)
    if(NOT TARGET Botan2::Botan2)
        add_library(Botan2::Botan2
            UNKNOWN IMPORTED GLOBAL
        )
        set_target_properties(Botan2::Botan2 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${Botan2_INCLUDE_DIRS}
            IMPORTED_LOCATION ${Botan2_FullLibraryPath}
            LINK_FLAGS ${Botan2_LDFLAGS_OTHER}
        )
    endif()
endif()
