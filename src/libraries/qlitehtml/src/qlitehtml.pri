exists($$PWD/3rdparty/litehtml/CMakeLists.txt) {
    message("pWD: " $$PWD);

    LH_SRC = $$PWD/3rdparty/litehtml
    LH_HDR = $$PWD/3rdparty/litehtml/include/litehtml
    GB_SRC = $$PWD/3rdparty/litehtml/src/gumbo
    GB_HDR = $$PWD/3rdparty/litehtml/src/gumbo/include/gumbo

    # gumbo
    SOURCES += \
        $$GB_SRC/attribute.c \
        $$GB_SRC/char_ref.c \
        $$GB_SRC/error.c \
        $$GB_SRC/parser.c \
        $$GB_SRC/string_buffer.c \
        $$GB_SRC/string_piece.c \
        $$GB_SRC/gb_tag.c \
        $$GB_SRC/gb_tokenizer.c \
        $$GB_SRC/utf8.c \
        $$GB_SRC/util.c \
        $$GB_SRC/vector.c

    HEADERS += \
        $$GB_SRC/include/gumbo.h \
        $$GB_HDR/attribute.h \
        $$GB_HDR/char_ref.h \
        $$GB_HDR/error.h \
        $$GB_HDR/insertion_mode.h \
        $$GB_HDR/parser.h \
        $$GB_HDR/string_buffer.h \
        $$GB_HDR/string_piece.h \
        $$GB_HDR/tag_enum.h \
        $$GB_HDR/tag_gperf.h \
        $$GB_HDR/tag_sizes.h \
        $$GB_HDR/tag_strings.h \
        $$GB_HDR/token_type.h \
        $$GB_HDR/tokenizer.h \
        $$GB_HDR/tokenizer_states.h \
        $$GB_HDR/utf8.h \
        $$GB_HDR/util.h \
        $$GB_HDR/vector.h

    INCLUDEPATH *= $$GB_SRC/include $$GB_HDR

    win32 {
        HEADERS += \
            $$GB_SRC/visualc/include/strings.h
        INCLUDEPATH *= $$GB_SRC/visualc/include
    }

    # litehtml
    SOURCES += \
        $$LH_SRC/src/background.cpp \
        $$LH_SRC/src/box.cpp \
        $$LH_SRC/src/context.cpp \
        $$LH_SRC/src/css_length.cpp \
        $$LH_SRC/src/css_selector.cpp \
        $$LH_SRC/src/document.cpp \
        $$LH_SRC/src/el_anchor.cpp \
        $$LH_SRC/src/el_base.cpp \
        $$LH_SRC/src/el_before_after.cpp \
        $$LH_SRC/src/el_body.cpp \
        $$LH_SRC/src/el_break.cpp \
        $$LH_SRC/src/el_cdata.cpp \
        $$LH_SRC/src/el_comment.cpp \
        $$LH_SRC/src/el_div.cpp \
        $$LH_SRC/src/element.cpp \
        $$LH_SRC/src/el_font.cpp \
        $$LH_SRC/src/el_image.cpp \
        $$LH_SRC/src/el_li.cpp \
        $$LH_SRC/src/el_link.cpp \
        $$LH_SRC/src/el_para.cpp \
        $$LH_SRC/src/el_script.cpp \
        $$LH_SRC/src/el_space.cpp \
        $$LH_SRC/src/el_style.cpp \
        $$LH_SRC/src/el_table.cpp \
        $$LH_SRC/src/el_td.cpp \
        $$LH_SRC/src/el_text.cpp \
        $$LH_SRC/src/el_title.cpp \
        $$LH_SRC/src/el_tr.cpp \
        $$LH_SRC/src/html.cpp \
        $$LH_SRC/src/html_tag.cpp \
        $$LH_SRC/src/iterators.cpp \
        $$LH_SRC/src/media_query.cpp \
        $$LH_SRC/src/num_cvt.cpp \
        $$LH_SRC/src/style.cpp \
        $$LH_SRC/src/stylesheet.cpp \
        $$LH_SRC/src/table.cpp \
        $$LH_SRC/src/utf8_strings.cpp \
        $$LH_SRC/src/web_color.cpp \
        $$LH_SRC/src/strtod.cpp

    HEADERS += \
        $$LH_SRC/include/litehtml.h \
        $$LH_HDR/attributes.h \
        $$LH_HDR/background.h \
        $$LH_HDR/borders.h \
        $$LH_HDR/box.h \
        $$LH_HDR/context.h \
        $$LH_HDR/css_length.h \
        $$LH_HDR/css_margins.h \
        $$LH_HDR/css_offsets.h \
        $$LH_HDR/css_position.h \
        $$LH_HDR/css_selector.h \
        $$LH_HDR/document.h \
        $$LH_HDR/el_anchor.h \
        $$LH_HDR/el_base.h \
        $$LH_HDR/el_before_after.h \
        $$LH_HDR/el_body.h \
        $$LH_HDR/el_break.h \
        $$LH_HDR/el_cdata.h \
        $$LH_HDR/el_comment.h \
        $$LH_HDR/el_div.h \
        $$LH_HDR/el_font.h \
        $$LH_HDR/el_image.h \
        $$LH_HDR/el_li.h \
        $$LH_HDR/el_link.h \
        $$LH_HDR/el_para.h \
        $$LH_HDR/el_script.h \
        $$LH_HDR/el_space.h \
        $$LH_HDR/el_style.h \
        $$LH_HDR/el_table.h \
        $$LH_HDR/el_td.h \
        $$LH_HDR/el_text.h \
        $$LH_HDR/el_title.h \
        $$LH_HDR/el_tr.h \
        $$LH_HDR/element.h \
        $$LH_HDR/html.h \
        $$LH_HDR/html_tag.h \
        $$LH_HDR/iterators.h \
        $$LH_HDR/media_query.h \
        $$LH_HDR/num_cvt.h \
        $$LH_HDR/os_types.h \
        $$LH_HDR/style.h \
        $$LH_HDR/stylesheet.h \
        $$LH_HDR/table.h \
        $$LH_HDR/types.h \
        $$LH_HDR/utf8_strings.h \
        $$LH_HDR/web_color.h

    INCLUDEPATH *= $$LH_SRC/include $$LH_HDR

    # litehtml without optimization is not fun
    QMAKE_CFLAGS_DEBUG += -O2
    QMAKE_CXXFLAGS_DEBUG += -O2
} else {
    INCLUDEPATH *= $$LITEHTML_INSTALL_DIR/include $$LITEHTML_INSTALL_DIR/include/litehtml
    LITEHTML_LIB_DIR = $$LITEHTML_INSTALL_DIR/lib
    LIBS += -L$$LITEHTML_LIB_DIR -llitehtml -lgumbo

    win32: PRE_TARGETDEPS += $$LITEHTML_LIB_DIR/litehtml.lib $$LITEHTML_LIB_DIR/gumbo.lib
    else:unix: PRE_TARGETDEPS += $$LITEHTML_LIB_DIR/liblitehtml.a $$LITEHTML_LIB_DIR/libgumbo.a
}

HEADERS += \
    $$PWD/element_checkbox.h \
    $$PWD/container_qpainter.h \
    $$PWD/container_qpainter_p.h \
    $$PWD/qlitehtmlwidget.h \
    $$PWD/qlitehtmlsearchwidget.h

SOURCES += \
    $$PWD/element_checkbox.cpp \
    $$PWD/container_qpainter.cpp \
    $$PWD/qlitehtmlwidget.cpp \
    $$PWD/qlitehtmlsearchwidget.cpp

FORMS += \
    $$PWD/qlitehtmlsearchwidget.ui

INCLUDEPATH *= $$PWD
win32: DEFINES += LITEHTML_UTF8

# DEFINES *= QLITEHTML_STATIC_LIBRARY
