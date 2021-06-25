set(PKG_CONFIG_REQUIRES "${${bin}_pkg_config_requires}")
set(PKG_CONFIG_LIBDIR "\${prefix}/lib")
set(PKG_CONFIG_INCLUDEDIR "\${prefix}/include/${bin}")
set(PKG_CONFIG_LIBS "-L\${libdir} -l${bin}")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${bin}.pc"
    )

set(INSTALL_PKG_CONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
    CACHE PATH "Installation directory for pkg-config files")

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${bin}.pc"
    DESTINATION
        "${INSTALL_PKG_CONFIG_DIR}"
    COMPONENT
        dev
    )
