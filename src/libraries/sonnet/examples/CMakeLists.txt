add_executable(dialog dialogexample.cpp)
target_link_libraries(dialog KF5::SonnetCore KF5::SonnetUi)

add_executable(highlighter highlighterexample.cpp)
target_link_libraries(highlighter KF5::SonnetCore KF5::SonnetUi)

add_executable(configdialog configdialog.cpp)
target_link_libraries(configdialog KF5::SonnetCore KF5::SonnetUi)

add_executable(dictionarycombobox dictionarycombobox.cpp)
target_link_libraries(dictionarycombobox KF5::SonnetCore KF5::SonnetUi)

add_executable(textedit textedit.cpp)
target_link_libraries(textedit KF5::SonnetCore KF5::SonnetUi)

add_executable(plaintextedit plaintextedit.cpp)
target_link_libraries(plaintextedit KF5::SonnetCore KF5::SonnetUi)

