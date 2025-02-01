#ifndef CODETOHTMLCONVERTER_H
#define CODETOHTMLCONVERTER_H

#include <QHash>
#include <QMultiHash>
#include <QString>

using LangData = QMultiHash<char, QLatin1String>;

/**
 *  The api we need start for qt 5.15.2
 */
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
using StringView = QStringView;
#else
using StringView = const QStringRef &;
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
#define TO_QSTRING(s) s.toString()
#else
#define TO_QSTRING(s) s
#endif

class CodeToHtmlConverter {
    enum Format { Type, Keyword, Literal, String, Comment, Builtin, Other };

    enum Lang {
        Invalid,
        CodeCpp,
        CodeJs,
        CodeC,
        CodeBash,
        CodePHP,
        CodeQML,
        CodePython,
        CodeRust,
        CodeJava,
        CodeCSharp,
        CodeGo,
        CodeV,
        CodeSQL,
        CodeJSON,
        CodeXML,
        CodeCSS,
        CodeTypeScript,
        CodeYAML,
        CodeINI,
        CodeVex,
        CodeCMake,
        CodeMake,
        CodeNix,
        CodeForth,
        CodeSystemVerilog,
        CodeGDScript
    };

   public:
    CodeToHtmlConverter(const QString &lang) Q_DECL_NOTHROW;
    Q_REQUIRED_RESULT QString process(StringView input) const;

   private:
    Lang _currentLang;

    Q_REQUIRED_RESULT static QString escape(QChar c);
    Q_REQUIRED_RESULT static QString escapeString(StringView s);
    Q_REQUIRED_RESULT static QString setFormat(StringView str, Format format);
    Q_REQUIRED_RESULT static QString setFormat(const QString &str, Format format);
    static void initCodeLangs() Q_DECL_NOTHROW;

    Q_REQUIRED_RESULT int highlightNumericLit(StringView input, QString &output, int i) const;
    Q_REQUIRED_RESULT static int highlightStringLiterals(StringView input, QChar strType,
                                                         QString &output, int i);
    Q_REQUIRED_RESULT static int highlightComment(StringView input, QString &output, int i,
                                                  bool isSingleLine = true);
    Q_REQUIRED_RESULT static int highlightWord(int i, const LangData &data, StringView input,
                                               QString &output, Format f);
    Q_REQUIRED_RESULT QString xmlHighlighter(StringView input) const;
    Q_REQUIRED_RESULT QString cssHighlighter(StringView input, const LangData &types,
                                             const LangData &keywords) const;
    Q_REQUIRED_RESULT QString ymlHighlighter(StringView input) const;
    Q_REQUIRED_RESULT QString iniHighlighter(StringView input) const;

    /**
     * @brief returns true if c is octal
     */
    Q_REQUIRED_RESULT static inline bool isOctal(const char c) { return (c >= '0' && c <= '7'); }

    /**
     * @brief returns true if c is hex
     */
    Q_REQUIRED_RESULT static inline bool isHex(const char c) {
        return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
    }

    static QHash<QString, Lang> _langStringToEnum;

    /** For Test Cases Only **/
    Q_REQUIRED_RESULT QString process(const QString &input) const;

    friend class TestNotes;
};

#endif    // CODETOHTMLCONVERTER_H
