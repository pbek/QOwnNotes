import qbs.File
import qbs.FileInfo

Product {
    type: buildLib ? ["staticlibrary"] : undefined

    Depends { name: "cpp" }
    Depends { name: "qtc" }

    property bool useExternalLib: qtc.litehtmlInstallDir
    property bool buildLib: !useExternalLib && File.exists(path + "/3rdparty/litehtml/CMakeLists.txt")
    condition: useExternalLib || buildLib

    property string gumboSrcDir: path + "/3rdparty/litehtml/src/gumbo"
    property string gumboHeaderDir: gumboSrcDir + "/include/gumbo"
    property string litehtmlHeaderDir: path + "/3rdparty/litehtml/include/litehtml"
    property string mainHeaderDir: litehtmlHeaderDir + '/..'
    property stringList sharedDefines: {
        var defines = ["QLITEHTML_STATIC_LIBRARY"];
        if (qbs.targetOS.contains("windows"))
            defines.push("LITEHTML_UTF8");
        return defines;
    }

    cpp.defines: sharedDefines
    cpp.includePaths: {
        var paths = [gumboHeaderDir, gumboHeaderDir + '/..', litehtmlHeaderDir, mainHeaderDir];
        if (qbs.targetOS.contains("windows"))
            paths.push(gumboSrcDir + "/visualc/include");
        return paths;
    }
    cpp.optimization: "fast"
    cpp.warningLevel: "none"
    cpp.cxxLanguageVersion: "c++14"

    Export {
        Depends { name: "cpp" }
        Group {
            name: "litehtml/Qt glue"
            cpp.warningLevel: "none"
            files: [
                "container_qpainter.cpp",
                "container_qpainter.h",
                "container_qpainter_p.h",
                "qlitehtmlwidget.cpp",
                "qlitehtmlwidget.h",
            ]
        }

        Properties {
            condition: product.useExternalLib
            cpp.dynamicLibraries: ["litehtml", "gumbo"]
            cpp.includePaths: [
                FileInfo.joinPaths(qtc.litehtmlInstallDir, "include"),
                FileInfo.joinPaths(qtc.litehtmlInstallDir, "include", "litehtml"),
            ]
            cpp.libraryPaths: FileInfo.joinPaths(qtc.litehtmlInstallDir, "lib")
        }
        Properties {
            condition: exportingProduct.buildLib
            cpp.defines: exportingProduct.sharedDefines
            cpp.includePaths: [exportingProduct.mainHeaderDir, path]
        }
    }

    Group {
        condition: buildLib
        name: "gumbo sources"
        prefix: gumboSrcDir + '/'
        files: [
            "attribute.c",
            "char_ref.c",
            "error.c",
            "parser.c",
            "string_buffer.c",
            "string_piece.c",
            "tag.c",
            "tokenizer.c",
            "utf8.c",
            "util.c",
            "vector.c",
        ]
    }

    Group {
        condition: buildLib
        name: "gumbo headers"
        prefix: gumboHeaderDir + '/'
        files: [
            "../gumbo.h",
            "attribute.h",
            "char_ref.h",
            "error.h",
            "insertion_mode.h",
            "parser.h",
            "string_buffer.h",
            "string_piece.h",
            "tag_enum.h",
            "tag_gperf.h",
            "tag_sizes.h",
            "tag_strings.h",
            "token_type.h",
            "tokenizer.h",
            "tokenizer_states.h",
            "utf8.h",
            "util.h",
            "vector.h",
        ]

        Group {
            name: "gumbo Windows headers"
            condition: qbs.targetOS.contains("windows")
            files: "../../visualc/include/strings.h"
        }
    }

    Group {
        condition: buildLib
        name: "litehtml sources"
        prefix: litehtmlHeaderDir + "/../../src/"
        files: [
            "codepoint.cpp",
            "css_borders.cpp",
            "background.cpp",
            "css_length.cpp",
            "css_parser.cpp",
            "css_properties.cpp",
            "css_selector.cpp",
            "css_tokenizer.cpp",
            "document.cpp",
            "document_container.cpp",
            "el_anchor.cpp",
            "el_base.cpp",
            "el_before_after.cpp",
            "el_body.cpp",
            "el_break.cpp",
            "el_cdata.cpp",
            "el_comment.cpp",
            "el_div.cpp",
            "element.cpp",
            "encodings.cpp",
            "el_font.cpp",
            "flex_item.cpp",
            "flex_line.cpp",
            "formatting_context.cpp",
            "gradient.cpp",
            "el_image.cpp",
            "el_link.cpp",
            "el_para.cpp",
            "el_script.cpp",
            "el_space.cpp",
            "el_style.cpp",
            "el_table.cpp",
            "el_td.cpp",
            "el_text.cpp",
            "el_title.cpp",
            "el_tr.cpp",
            "html.cpp",
            "html_microsyntaxes.cpp",
            "html_tag.cpp",
            "iterators.cpp",
            "line_box.cpp",
            "media_query.cpp",
            "num_cvt.cpp",
            "render_block.cpp",
            "render_block_context.cpp",
            "render_flex.cpp",
            "render_image.cpp",
            "render_inline_context.cpp",
            "render_item.cpp",
            "render_table.cpp",
            "style.cpp",
            "string_id.cpp",
            "stylesheet.cpp",
            "table.cpp",
            "tstring_view.cpp",
            "url.cpp",
            "url_path.cpp",
            "utf8_strings.cpp",
            "web_color.cpp",
            "strtod.cpp",
        ]
    }

    Group {
        condition: buildLib
        name: "litehtml headers"
        prefix: litehtmlHeaderDir + '/'
        files: [
            "../litehtml.h",
            "background.h",
            "borders.h",
            "codepoint.h",
            "css_length.h",
            "css_margins.h",
            "css_offsets.h",
            "css_position.h",
            "css_selector.h",
            "css_parser.h",
            "css_properties.h",
            "css_tokenizer.h",
            "document.h",
            "document_container.h",
            "el_anchor.h",
            "el_base.h",
            "el_before_after.h",
            "el_body.h",
            "el_break.h",
            "el_cdata.h",
            "el_comment.h",
            "el_div.h",
            "el_font.h",
            "el_image.h",
            "el_link.h",
            "el_para.h",
            "el_script.h",
            "el_space.h",
            "el_style.h",
            "el_table.h",
            "el_td.h",
            "el_text.h",
            "el_title.h",
            "el_tr.h",
            "element.h",
            "encodings.h",
            "flex_item.h",
            "flex_line.h",
            "font_description.h",
            "formatting_context.h",
            "gradient.h",
            "html.h",
            "html_microsyntaxes.h",
            "html_tag.h",
            "iterators.h",
            "line_box.h",
            "master_css.h",
            "media_query.h",
            "num_cvt.h",
            "os_types.h",
            "render_block.h",
            "render_block_context.h",
            "render_flex.h",
            "render_image.h",
            "render_inline.h",
            "render_inline_context.h",
            "render_item.h",
            "render_table.h",
            "scroll_view.h",
            "style.h",
            "stylesheet.h",
            "string_id.h",
            "table.h",
            "tstring_view.h",
            "types.h",
            "url.h",
            "url_path.h",
            "utf8_strings.h",
            "web_color.h",
        ]
    }
}
