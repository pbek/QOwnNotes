find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS
  REQUIRED Core)

add_library(
  Sonnet STATIC
  src/core/backgroundchecker.cpp
  src/core/backgroundchecker.h
  src/core/backgroundchecker_p.h
  src/core/client.cpp
  src/core/client_p.h
  src/core/core_debug.cpp
  src/core/core_debug.h
  src/core/guesslanguage.cpp
  src/core/guesslanguage.h
  src/core/languagefilter.cpp
  src/core/languagefilter_p.h
  src/core/loader.cpp
  src/core/loader_p.h
  src/core/settings.cpp
  src/core/settings_p.h
  src/core/sonnetcore_export.h
  src/core/speller.cpp
  src/core/speller.h
  src/core/spellerplugin.cpp
  src/core/spellerplugin_p.h
  src/core/textbreaks.cpp
  src/core/textbreaks_p.h
  src/core/tokenizer.cpp
  src/core/tokenizer_p.h
  # Hunspell plugin
  src/plugins/hunspell/hunspellclient.cpp
  src/plugins/hunspell/hunspellclient.h
  src/plugins/hunspell/hunspelldebug.cpp
  src/plugins/hunspell/hunspelldebug.h
  src/plugins/hunspell/hunspelldict.cpp
  src/plugins/hunspell/hunspelldict.h)

# The hunspell library
add_subdirectory(src/plugins/hunspell/hunspell)
add_subdirectory(src/plugins/hunspell/aspell)

target_link_libraries(Sonnet PRIVATE Qt${QT_VERSION_MAJOR}::Core Hunspell
                                     Aspell)
