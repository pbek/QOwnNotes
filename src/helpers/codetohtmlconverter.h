#ifndef CODETOHTMLCONVERTER_H
#define CODETOHTMLCONVERTER_H

#include <QHash>
#include <QMultiHash>
#include <QString>

using LangData = QMultiHash<char, QLatin1String>;

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
        CodeMake
    };

   public:
    CodeToHtmlConverter(const QString &lang) Q_DECL_NOTHROW;
    Q_REQUIRED_RESULT QString process(const QStringRef &input) const;

   private:
    Lang _currentLang;

    Q_REQUIRED_RESULT static QString escape(QChar c);
    Q_REQUIRED_RESULT static QString escapeString(const QStringRef &s);
    Q_REQUIRED_RESULT static QString setFormat(const QStringRef &str,
                                               Format format);
    static void initCodeLangs() Q_DECL_NOTHROW;

    Q_REQUIRED_RESULT int highlightNumericLit(const QStringRef &input,
                                              QString &output, int i) const;
    Q_REQUIRED_RESULT static int highlightStringLiterals(
        const QStringRef &input, QChar strType, QString &output, int i);
    Q_REQUIRED_RESULT static int highlightComment(const QStringRef &input,
                                                  QString &output, int i,
                                                  bool isSingleLine = true);
    Q_REQUIRED_RESULT static int highlightWord(int i, const LangData &data,
                                               const QStringRef &input,
                                               QString &output, Format f);
    Q_REQUIRED_RESULT QString xmlHighlighter(const QStringRef &input) const;
    Q_REQUIRED_RESULT QString cssHighlighter(const QStringRef &input,
                                             const LangData &types,
                                             const LangData &keywords) const;
    Q_REQUIRED_RESULT QString ymlHighlighter(const QStringRef &input) const;
    Q_REQUIRED_RESULT QString iniHighlighter(const QStringRef &input) const;

    /**
     * @brief returns true if c is octal
     */
    Q_REQUIRED_RESULT static inline bool isOctal(const char c) {
        return (c >= '0' && c <= '7');
    }

    /**
     * @brief returns true if c is hex
     */
    Q_REQUIRED_RESULT static inline bool isHex(const char c) {
        return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
                (c >= 'A' && c <= 'F'));
    }

    static QHash<QString, Lang> _langStringToEnum;
};

#endif    // CODETOHTMLCONVERTER_H
