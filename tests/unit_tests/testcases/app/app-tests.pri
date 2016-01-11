 
APP_SRC_DIR = $$PWD/../../../../src/

INCLUDEPATH += $$APP_SRC_DIR

HEADERS  += \
    $$PWD/test_*.h \
    $$APP_SRC_DIR/services/databaseservice.h \
    $$APP_SRC_DIR/entities/note.h \
    $$APP_SRC_DIR/entities/calendaritem.h \
    $$APP_SRC_DIR/helpers/htmlentities.h \
    $$APP_SRC_DIR/libraries/diff_match_patch/diff_match_patch.h \
    $$APP_SRC_DIR/libraries/hoedown/*.h \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_definitions.h \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_parser.h \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/highlighter.h \

SOURCES += \
    $$PWD/test_*.cpp \
    $$APP_SRC_DIR/services/databaseservice.cpp \
    $$APP_SRC_DIR/entities/note.cpp \
    $$APP_SRC_DIR/entities/calendaritem.cpp \
    $$APP_SRC_DIR/helpers/htmlentities.cpp \
    $$APP_SRC_DIR/libraries/diff_match_patch/diff_match_patch.cpp \
    $$APP_SRC_DIR/libraries/hoedown/*.c \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_parser.c \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/highlighter.cpp \

#OTHER_FILES += \
#    connections.xml

#FORMS += \
#    $$APP_SRC_DIR/forms/connection.ui \
