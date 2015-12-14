 
APP_SRC_DIR = $$PWD/../../../../src/

INCLUDEPATH += $$APP_SRC_DIR

HEADERS  += \
    $$PWD/test_*.h \
    $$APP_SRC_DIR/entities/note.h \
    $$APP_SRC_DIR/htmlentities.h \
    $$APP_SRC_DIR/libraries/markdown-highlight/*.h \
    $$APP_SRC_DIR/libraries/diff_match_patch/diff_match_patch.h \
    $$APP_SRC_DIR/libraries/hoedown/*.h \

SOURCES += \
    $$PWD/test_*.cpp \
    $$APP_SRC_DIR/entities/note.cpp \
    $$APP_SRC_DIR/htmlentities.cpp \
    $$APP_SRC_DIR/libraries/markdown-highlight/*.c \
    $$APP_SRC_DIR/libraries/markdown-highlight/*.cpp \
    $$APP_SRC_DIR/libraries/diff_match_patch/diff_match_patch.cpp \
    $$APP_SRC_DIR/libraries/hoedown/*.c \

#OTHER_FILES += \
#    connections.xml

#FORMS += \
#    $$APP_SRC_DIR/forms/connection.ui \
