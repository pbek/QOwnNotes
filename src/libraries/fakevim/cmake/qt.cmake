find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
target_link_libraries(${bin} Qt5::Widgets)

set(${bin}_pkg_config_requires ${${bin}_pkg_config_requires} Qt5::Widgets)
