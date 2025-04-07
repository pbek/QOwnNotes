find_package(Qt${QT_VERSION_MAJOR}Widgets REQUIRED)

# include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
target_link_libraries(${bin} Qt${QT_VERSION_MAJOR}::Widgets)

set(${bin}_pkg_config_requires ${${bin}_pkg_config_requires}
                               Qt${QT_VERSION_MAJOR}::Widgets)
