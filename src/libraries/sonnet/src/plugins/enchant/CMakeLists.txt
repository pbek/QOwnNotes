include_directories( ${ENCHANT_INCLUDE_DIR} "${ENCHANT_INCLUDE_DIR}/.." )


########### next target ###############

set(kspell_enchant_PART_SRCS enchantdict.cpp enchantclient.cpp )


add_library(kspell_enchant MODULE ${kspell_enchant_PART_SRCS})

target_link_libraries(kspell_enchant PRIVATE KF5::SonnetCore ${ENCHANT_LIBRARIES})

install(TARGETS kspell_enchant DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/sonnet/)

