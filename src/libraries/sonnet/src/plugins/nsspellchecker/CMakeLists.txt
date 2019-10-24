set(sonnet_nsspellchecker_PART_SRCS
    nsspellcheckerclient.mm
    nsspellcheckerdict.mm
)
ecm_qt_declare_logging_category(sonnet_nsspellchecker_PART_SRCS
    HEADER nsspellcheckerdebug.h
    IDENTIFIER SONNET_NSSPELLCHECKER
    CATEGORY_NAME sonnet.plugins.nsspellchecker)

add_library(sonnet_nsspellchecker MODULE ${sonnet_nsspellchecker_PART_SRCS})

target_link_libraries(sonnet_nsspellchecker PRIVATE KF5::SonnetCore "-framework AppKit")

install(TARGETS sonnet_nsspellchecker  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/sonnet/)

