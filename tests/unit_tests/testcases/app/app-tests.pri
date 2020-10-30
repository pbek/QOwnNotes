 
APP_SRC_DIR = $$PWD/../../../../src/

INCLUDEPATH += $$APP_SRC_DIR

HEADERS  += \
    $$PWD/test_*.h \
    $$APP_SRC_DIR/version.h \
    $$APP_SRC_DIR/release.h \
    $$APP_SRC_DIR/services/databaseservice.h \
    $$APP_SRC_DIR/api/noteapi.h \
    $$APP_SRC_DIR/api/notesubfolderapi.h \
    $$APP_SRC_DIR/api/tagapi.h \
    $$APP_SRC_DIR/api/scriptapi.h \
    $$APP_SRC_DIR/services/scriptingservice.h \
    $$APP_SRC_DIR/utils/misc.h \
    $$APP_SRC_DIR/utils/gui.h \
    $$APP_SRC_DIR/utils/schema.h \
    $$APP_SRC_DIR/libraries/piwiktracker/piwiktracker.h \
    $$APP_SRC_DIR/services/metricsservice.h \
    $$APP_SRC_DIR/services/cryptoservice.h \
    $$APP_SRC_DIR/services/owncloudservice.h \
    $$APP_SRC_DIR/dialogs/masterdialog.h \
    $$APP_SRC_DIR/widgets/logwidget.h \
    $$APP_SRC_DIR/entities/notefolder.h \
    $$APP_SRC_DIR/entities/cloudconnection.h \
    $$APP_SRC_DIR/entities/notesubfolder.h \
    $$APP_SRC_DIR/entities/note.h \
    $$APP_SRC_DIR/entities/trashitem.h \
    $$APP_SRC_DIR/entities/tag.h \
    $$APP_SRC_DIR/entities/calendaritem.h \
    $$APP_SRC_DIR/entities/bookmark.h \
    $$APP_SRC_DIR/entities/script.h \
    $$APP_SRC_DIR/helpers/htmlentities.h \
    $$APP_SRC_DIR/helpers/clientproxy.h \
    $$APP_SRC_DIR/helpers/codetohtmlconverter.h \
    $$APP_SRC_DIR/libraries/diff_match_patch/diff_match_patch.h \
    $$APP_SRC_DIR/libraries/md4c/md4c/md4c.h \
    $$APP_SRC_DIR/libraries/md4c/md2html/render_html.h \
    $$APP_SRC_DIR/libraries/md4c/md2html/entity.h \
    $$APP_SRC_DIR/libraries/simplecrypt/simplecrypt.h \
    $$APP_SRC_DIR/libraries/botan/botanwrapper.h \
    $$APP_SRC_DIR/libraries/versionnumber/versionnumber.h \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/markdownhighlighter.h \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/qownlanguagedata.h \
    $$APP_SRC_DIR/threads/scriptthread.h \

SOURCES += \
    $$PWD/test_*.cpp \
    $$APP_SRC_DIR/services/databaseservice.cpp \
    $$APP_SRC_DIR/api/noteapi.cpp \
    $$APP_SRC_DIR/api/notesubfolderapi.cpp \
    $$APP_SRC_DIR/api/tagapi.cpp \
    $$APP_SRC_DIR/api/scriptapi.cpp \
    $$APP_SRC_DIR/services/scriptingservice.cpp \
    $$APP_SRC_DIR/utils/misc.cpp \
    $$APP_SRC_DIR/utils/gui.cpp \
    $$APP_SRC_DIR/utils/schema.cpp \
    $$APP_SRC_DIR/libraries/piwiktracker/piwiktracker.cpp \
    $$APP_SRC_DIR/services/metricsservice.cpp \
    $$APP_SRC_DIR/services/cryptoservice.cpp \
    $$APP_SRC_DIR/services/owncloudservice.cpp \
    $$APP_SRC_DIR/dialogs/masterdialog.cpp \
    $$APP_SRC_DIR/widgets/logwidget.cpp \
    $$APP_SRC_DIR/entities/notefolder.cpp \
    $$APP_SRC_DIR/entities/cloudconnection.cpp \
    $$APP_SRC_DIR/entities/notesubfolder.cpp \
    $$APP_SRC_DIR/entities/note.cpp \
    $$APP_SRC_DIR/entities/trashitem.cpp \
    $$APP_SRC_DIR/entities/tag.cpp \
    $$APP_SRC_DIR/entities/calendaritem.cpp \
    $$APP_SRC_DIR/entities/bookmark.cpp \
    $$APP_SRC_DIR/entities/script.cpp \
    $$APP_SRC_DIR/helpers/htmlentities.cpp \
    $$APP_SRC_DIR/helpers/clientproxy.cpp \
    $$APP_SRC_DIR/helpers/codetohtmlconverter.cpp \
    $$APP_SRC_DIR/libraries/diff_match_patch/diff_match_patch.cpp \
    $$APP_SRC_DIR/libraries/md4c/md4c/md4c.c \
    $$APP_SRC_DIR/libraries/md4c/md2html/render_html.c \
    $$APP_SRC_DIR/libraries/md4c/md2html/entity.c \
    $$APP_SRC_DIR/libraries/simplecrypt/simplecrypt.cpp \
    $$APP_SRC_DIR/libraries/botan/botanwrapper.cpp \
    $$APP_SRC_DIR/libraries/versionnumber/versionnumber.cpp \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/markdownhighlighter.cpp \
    $$APP_SRC_DIR/libraries/qmarkdowntextedit/qownlanguagedata.cpp \
    $$APP_SRC_DIR/threads/scriptthread.cpp \

include($$APP_SRC_DIR/libraries/botan/botan.pri)

#OTHER_FILES += \
#    connections.xml

#FORMS += \
#    $$APP_SRC_DIR/widgets/logwidget.ui \
