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
            "background.cpp",
            "box.cpp",
            "context.cpp",
            "css_length.cpp",
            "css_selector.cpp",
            "document.cpp",
            "el_anchor.cpp",
            "el_base.cpp",
            "el_before_after.cpp",
            "el_body.cpp",
            "el_break.cpp",
            "el_cdata.cpp",
            "el_comment.cpp",
            "el_div.cpp",
            "element.cpp",
            "el_font.cpp",
            "el_image.cpp",
            "el_li.cpp",
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
            "html_tag.cpp",
            "iterators.cpp",
            "media_query.cpp",
            "num_cvt.cpp",
            "style.cpp",
            "stylesheet.cpp",
            "table.cpp",
            "utf8_strings.cpp",
            "web_color.cpp",
        ]
    }

    Group {
        condition: buildLib
        name: "litehtml headers"
        prefix: litehtmlHeaderDir + '/'
        files: [
            "../litehtml.h",
            "attributes.h",
            "background.h",
            "borders.h",
            "box.h",
            "context.h",
            "css_length.h",
            "css_margins.h",
            "css_offsets.h",
            "css_position.h",
            "css_selector.h",
            "document.h",
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
            "el_li.h",
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
            "html.h",
            "html_tag.h",
            "iterators.h",
            "media_query.h",
            "num_cvt.h",
            "os_types.h",
            "style.h",
            "stylesheet.h",
            "table.h",
            "types.h",
            "utf8_strings.h",
            "web_color.h",
        ]
    }
}
