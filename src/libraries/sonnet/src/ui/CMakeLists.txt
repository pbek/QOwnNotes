project(sonnetui)

ecm_create_qm_loader(sonnet_QM_LOADER sonnet5_qt)

set(sonnetui_SRCS
    configdialog.cpp
    configwidget.cpp
    dialog.cpp
    dictionarycombobox.cpp
    highlighter.cpp
    spellcheckdecorator.cpp
    ${sonnet_QM_LOADER}
)

ecm_qt_declare_logging_category(sonnetui_SRCS HEADER ui_debug.h IDENTIFIER SONNET_LOG_UI CATEGORY_NAME sonnet.ui)

set(sonnetui_UI
    configui.ui
    sonnetui.ui
)

ecm_generate_headers(SonnetUi_CamelCase_HEADERS
  HEADER_NAMES
    Dialog
    Highlighter
    ConfigDialog
    ConfigWidget
    DictionaryComboBox
    SpellCheckDecorator
  PREFIX Sonnet
  REQUIRED_HEADERS SonnetUi_HEADERS
)

qt5_wrap_ui(sonnetui_SRCS ${sonnetui_UI})

add_library(KF5SonnetUi ${sonnetui_SRCS})
generate_export_header(KF5SonnetUi BASE_NAME SonnetUi EXPORT_FILE_NAME sonnetui_export.h)
add_library(KF5::SonnetUi ALIAS KF5SonnetUi)

target_link_libraries(KF5SonnetUi
    PUBLIC Qt5::Widgets
    PRIVATE KF5::SonnetCore
)

set_target_properties(KF5SonnetUi PROPERTIES VERSION ${SONNET_VERSION_STRING}
                                             SOVERSION ${SONNET_SOVERSION}
                                             EXPORT_NAME SonnetUi
)


# CMAKE_CURRENT_BINARY_DIR: for camelcase headers and lowercase forwarders
target_include_directories(KF5SonnetUi INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetUi>")
target_include_directories(KF5SonnetUi PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")

install(TARGETS KF5SonnetUi EXPORT KF5SonnetTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES ${SonnetUi_CamelCase_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetUi/Sonnet COMPONENT Devel)

install(FILES
   ${SonnetUi_HEADERS}
   ${CMAKE_CURRENT_BINARY_DIR}/sonnetui_export.h
   DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetUi/sonnet COMPONENT Devel)

if(BUILD_DESIGNERPLUGIN)
    add_subdirectory(designer)
endif()

if (BUILD_QCH)
    ecm_add_qch(
        KF5SonnetUi_QCH
        NAME SonnetUi
        BASE_NAME KF5SonnetUi
        VERSION ${KF5_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${SonnetUi_HEADERS}
        LINK_QCHS
            Qt5Core_QCH
            Qt5Gui_QCH
            Qt5Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            SONNETUI_EXPORT
            SONNETUI_DEPRECATED_EXPORT
            SONNETUI_DEPRECATED
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME SonnetUi LIB_NAME KF5SonnetUi DEPS "widgets" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/SonnetUi)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
