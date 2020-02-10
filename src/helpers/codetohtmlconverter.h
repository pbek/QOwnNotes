#ifndef CODETOHTMLCONVERTER_H
#define CODETOHTMLCONVERTER_H

#include <QHash>
#include <QMultiHash>
#include <QString>

typedef QMultiHash<char, QLatin1String> LangData;

class CodeToHtmlConverter {
    /* ENUMS */
   public:
    enum Format { Type, Keyword, Literal, String, Comment, Builtin, Other };

    enum Lang {
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
    CodeToHtmlConverter(const QStringRef input,
                        const QString &lang) Q_DECL_NOTHROW;
    Q_REQUIRED_RESULT QString process() const;

   private:
    const QStringRef _input;
    const QString _lang;
    Lang _currentLang;

    Q_REQUIRED_RESULT static QString escape(QChar c);
    Q_REQUIRED_RESULT static QString escapeString(const QStringRef s);
    Q_REQUIRED_RESULT static QString setFormat(const QStringRef str,
                                               Format format);
    static void initCodeLangs() Q_DECL_NOTHROW;

    Q_REQUIRED_RESULT int highlightNumericLit(QString &output, int i) const;
    Q_REQUIRED_RESULT int highlightStringLiterals(QChar strType,
                                                  QString &output, int i) const;
    Q_REQUIRED_RESULT int highlightComment(QString &output, int i,
                                           bool isSingleLine = true) const;
    Q_REQUIRED_RESULT int highlightWord(int i, const LangData &data,
                                        QString &output, Format f) const;
    Q_REQUIRED_RESULT QString xmlHighlighter() const;
    Q_REQUIRED_RESULT QString cssHighlighter(const LangData &types,
                                             const LangData &keywords) const;
    Q_REQUIRED_RESULT QString ymlHighlighter() const;
    Q_REQUIRED_RESULT QString iniHighlighter() const;

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
