project(sonnetcore)

set(sonnetcore_SRCS
   loader.cpp
   client.cpp
   spellerplugin.cpp
   speller.cpp
   settings.cpp
   backgroundchecker.cpp
   guesslanguage.cpp
   textbreaks.cpp
   tokenizer.cpp
   languagefilter.cpp
)

ecm_qt_declare_logging_category(sonnetcore_SRCS HEADER core_debug.h IDENTIFIER SONNET_LOG_CORE CATEGORY_NAME sonnet.core)

# Dear packagers, this is just used as an extra search paths for plugins. Don't get your panties in a twist.
add_definitions(-DINSTALLATION_PLUGIN_PATH="${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_PLUGINDIR}")


add_library(KF5SonnetCore ${sonnetcore_SRCS})
generate_export_header(KF5SonnetCore BASE_NAME SonnetCore EXPORT_FILE_NAME sonnetcore_export.h)
add_library(KF5::SonnetCore ALIAS KF5SonnetCore)

ecm_generate_headers(SonnetCore_CamelCase_HEADERS
  HEADER_NAMES
    BackgroundChecker
    Speller
    GuessLanguage
  PREFIX Sonnet
  REQUIRED_HEADERS SonnetCore_HEADERS
)

target_link_libraries(KF5SonnetCore PUBLIC Qt5::Core)

set_target_properties(KF5SonnetCore PROPERTIES VERSION ${SONNET_VERSION_STRING}
                                               SOVERSION ${SONNET_SOVERSION}
                                               EXPORT_NAME SonnetCore
)

# CMAKE_CURRENT_BINARY_DIR: for camelcase headers and lowercase forwarders
target_include_directories(KF5SonnetCore INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetCore>")
target_include_directories(KF5SonnetCore PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")

install(TARGETS KF5SonnetCore EXPORT KF5SonnetTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES ${SonnetCore_CamelCase_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetCore/Sonnet COMPONENT Devel)

install(FILES
   ${SonnetCore_HEADERS}
   ${CMAKE_CURRENT_BINARY_DIR}/sonnetcore_export.h
   DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetCore/sonnet COMPONENT Devel)

if (BUILD_QCH)
    ecm_add_qch(
        KF5SonnetCore_QCH
        NAME SonnetCore
        BASE_NAME KF5SonnetCore
        VERSION ${KF5_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${SonnetCore_HEADERS}
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            SONNETCORE_EXPORT
            SONNETCORE_DEPRECATED_EXPORT
            SONNETCORE_DEPRECATED
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME SonnetCore LIB_NAME KF5SonnetCore DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetCore)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
