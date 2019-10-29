HEADERS += \
    $$PWD/src/hunspell/affentry.hxx \
    $$PWD/src/hunspell/affixmgr.hxx \
    $$PWD/src/hunspell/atypes.hxx \
    $$PWD/src/hunspell/baseaffix.hxx \
    $$PWD/src/hunspell/csutil.hxx \
    $$PWD/src/hunspell/filemgr.hxx \
    $$PWD/src/hunspell/hashmgr.hxx \
    $$PWD/src/hunspell/htypes.hxx \
    $$PWD/src/hunspell/hunspell.h \
    $$PWD/src/hunspell/hunspell.hxx \
    $$PWD/src/hunspell/hunvisapi.h \
    $$PWD/src/hunspell/hunvisapi.h.in \
    $$PWD/src/hunspell/hunzip.hxx \
    $$PWD/src/hunspell/langnum.hxx \
    $$PWD/src/hunspell/phonet.hxx \
    $$PWD/src/hunspell/replist.hxx \
    $$PWD/src/hunspell/suggestmgr.hxx \
    $$PWD/src/hunspell/utf_info.hxx \
    $$PWD/src/hunspell/w_char.hxx \
    $$PWD/src/parsers/firstparser.hxx \
    $$PWD/src/parsers/htmlparser.hxx \
    $$PWD/src/parsers/latexparser.hxx \
    $$PWD/src/parsers/manparser.hxx \
    $$PWD/src/parsers/odfparser.hxx \
    $$PWD/src/parsers/textparser.hxx \
    $$PWD/src/parsers/xmlparser.hxx

SOURCES += \
    $$PWD/src/hunspell/affentry.cxx \
    $$PWD/src/hunspell/affixmgr.cxx \
    $$PWD/src/hunspell/csutil.cxx \
    $$PWD/src/hunspell/filemgr.cxx \
    $$PWD/src/hunspell/hashmgr.cxx \
    $$PWD/src/hunspell/hunspell.cxx \
    $$PWD/src/hunspell/hunzip.cxx \
    $$PWD/src/hunspell/phonet.cxx \
    $$PWD/src/hunspell/replist.cxx \
    $$PWD/src/hunspell/suggestmgr.cxx \
    $$PWD/src/parsers/firstparser.cxx \
    $$PWD/src/parsers/htmlparser.cxx \
    $$PWD/src/parsers/latexparser.cxx \
    $$PWD/src/parsers/manparser.cxx \
    $$PWD/src/parsers/odfparser.cxx \
  #  $$PWD/src/parsers/testparser.cxx \
    $$PWD/src/parsers/textparser.cxx \
    $$PWD/src/parsers/xmlparser.cxx

win32 {
    message("Hunspell Windows Build")
    DEFINES += BUILDING_LIBHUNSPELL
}

INCLUDEPATH += $$PWD/src/hunspell
INCLUDEPATH += $$PWD/src/parsers
