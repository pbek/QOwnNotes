set(INSTALL_LIB_DIR "lib${LIB_SUFFIX}" CACHE PATH
    "Installation directory for libraries")
set(INSTALL_BIN_DIR "bin" CACHE PATH
    "Installation directory for executables")
set(INSTALL_INCLUDE_DIR "include/${bin}" CACHE PATH
    "Installation directory for header files")
set(INSTALL_CMAKE_DIR "lib${LIB_SUFFIX}/cmake/${bin}" CACHE PATH
    "Installation directory for CMake files")

# Shared or static library
option(CREATE_STATIC_LIBRARY "Create static library" OFF)
if (CREATE_STATIC_LIBRARY)
    set(libtype STATIC)
else()
    set(libtype SHARED)
endif()

add_library(${bin} ${libtype} ${${bin}_sources})
set_target_properties(${bin} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    )

# Headers
set(export_header "${CMAKE_CURRENT_BINARY_DIR}/private/${bin}_export.h")
set_target_properties(${bin} PROPERTIES
    PUBLIC_HEADER "${${bin}_public_headers}"
    PRIVATE_HEADER "${export_header}")

target_include_directories(${bin}
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${bin}>
    INTERFACE $<INSTALL_INTERFACE:include/${bin}>
    )

install(
    TARGETS ${bin} EXPORT ${bin}Targets
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev
    PRIVATE_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/private" COMPONENT dev
    )

# Generate and install CMake files for the library so `find_package(<Library>)` can be used with CMake.
# For more info: https://cmake.org/cmake/help/v3.0/manual/cmake-packages.7.html#creating-packages
include(GenerateExportHeader)
generate_export_header(${bin} EXPORT_FILE_NAME "${export_header}")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${bin}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

export(
    EXPORT ${bin}Targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${bin}Targets.cmake"
    )
configure_file(
    cmake/${bin}Config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/${bin}Config.cmake"
    COPYONLY
    )

install(
    EXPORT ${bin}Targets
    FILE
        ${bin}Targets.cmake
    DESTINATION
        "${INSTALL_CMAKE_DIR}"
    COMPONENT
        dev
    )
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${bin}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${bin}ConfigVersion.cmake"
    DESTINATION
        "${INSTALL_CMAKE_DIR}"
    COMPONENT
        dev
    )
