
# you can find find_package() in kdelibs/cmake/modules/OptionalFindPackage.cmake
# it is the same as FIND_PACKAGE(<name>) but additionally creates an OPTION(WITH_<name>)
# so the checking for the software can be disabled via ccmake or -DWITH_<name>=OFF
find_package(ASPELL)
set_package_properties(ASPELL PROPERTIES
    URL "http://aspell.net/"
    DESCRIPTION "Spell checking support via Aspell"
    TYPE OPTIONAL
)
if (ASPELL_FOUND)
   add_subdirectory( aspell )
endif ()


find_package(HSPELL)
set_package_properties(HSPELL PROPERTIES
    URL "http://ivrix.org.il/projects/spell-checker/"
    DESCRIPTION "Spell checking support for Hebrew"
    TYPE OPTIONAL
)
if (HSPELL_FOUND)
   add_subdirectory( hspell )
endif ()


#find_package(ENCHANT)
#set_package_properties(ENCHANT PROPERTIES
#    URL "http://www.abisource.com/projects/enchant/"
#    DESCRIPTION "Spell checking support via Enchant"
#    TYPE OPTIONAL
#    PURPOSE ""
#)
#if (ENCHANT_FOUND)
#   add_subdirectory( enchant )
#endif ()

find_package(HUNSPELL)
set_package_properties(HUNSPELL PROPERTIES
    URL "http://hunspell.sourceforge.net/"
    DESCRIPTION "Spell checking support via Hunspell"
    TYPE OPTIONAL
)
if (HUNSPELL_FOUND)
   add_subdirectory( hunspell )
endif ()


find_package(VOIKKO)
set_package_properties(VOIKKO PROPERTIES
    URL "http://voikko.puimula.org/"
    DESCRIPTION "Spell checking support via Voikko"
    TYPE OPTIONAL
)
if (VOIKKO_FOUND)
   add_subdirectory( voikko )
endif ()

if (APPLE)
    add_subdirectory( nsspellchecker )
endif ()
