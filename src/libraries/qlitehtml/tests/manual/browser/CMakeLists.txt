find_package(
  ${Qt}
  COMPONENTS Widgets Network
  REQUIRED)

add_executable(qlitehtmlbrowser MACOSX_BUNDLE main.cpp)

target_include_directories(qlitehtmlbrowser PUBLIC ../../../src)
target_link_libraries(qlitehtmlbrowser qlitehtml ${Qt}::Widgets ${Qt}::Network)
