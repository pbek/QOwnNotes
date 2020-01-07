INCLUDEPATH += $$PWD/md4c
QT -= gui

SOURCES += $$PWD/md4c/md4c.c \
           $$PWD/md2html/render_html.c \
           $$PWD/md2html/entity.c

HEADERS += $$PWD/md4c/md4c.h \
           $$PWD/md2html/render_html.h \
           $$PWD/md2html/entity.h
