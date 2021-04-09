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
    Q_REQUIRED_RESULT QString process(const QString &input) const;

   private:
    Lang _currentLang;

    Q_REQUIRED_RESULT static QString escape(QChar c);
    Q_REQUIRED_RESULT static QString escapeString(const QString &s);
    Q_REQUIRED_RESULT static QString setFormat(const QString &str,
                                               Format format);
    static void initCodeLangs() Q_DECL_NOTHROW;

    Q_REQUIRED_RESULT int highlightNumericLit(const QString &input,
                                              QString &output, int i) const;
    Q_REQUIRED_RESULT static int highlightStringLiterals(
        const QString &input, QChar strType, QString &output, int i);
    Q_REQUIRED_RESULT static int highlightComment(const QString &input,
                                                  QString &output, int i,
                                                  bool isSingleLine = true);
    Q_REQUIRED_RESULT static int highlightWord(int i, const LangData &data,
                                               const QString &input,
                                               QString &output, Format f);
    Q_REQUIRED_RESULT QString xmlHighlighter(const QString &input) const;
    Q_REQUIRED_RESULT QString cssHighlighter(const QString &input,
                                             const LangData &types,
                                             const LangData &keywords) const;
    Q_REQUIRED_RESULT QString ymlHighlighter(const QString &input) const;
    Q_REQUIRED_RESULT QString iniHighlighter(const QString &input) const;

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
