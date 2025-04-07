include_directories(${ASPELL_INCLUDE_DIR})

# ########## next target ###############

set(sonnet_aspell_PART_SRCS aspellclient.cpp aspelldict.cpp)
ecm_qt_declare_logging_category(
  sonnet_aspell_PART_SRCS
  HEADER
  aspell_debug.h
  IDENTIFIER
  SONNET_LOG_ASPELL
  CATEGORY_NAME
  sonnet.plugins.aspell)

add_library(sonnet_aspell MODULE ${sonnet_aspell_PART_SRCS})

target_link_libraries(sonnet_aspell PRIVATE KF5::SonnetCore ${ASPELL_LIBRARIES})

install(TARGETS sonnet_aspell DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/sonnet/)
