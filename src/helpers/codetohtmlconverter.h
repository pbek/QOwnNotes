#ifndef CODETOHTMLCONVERTER_H
#define CODETOHTMLCONVERTER_H

#include <QString>
#include <QMultiHash>
#include <QHash>

class CodeToHtmlConverter
{
/* ENUMS */
public:
    enum Format {
        Type,
        Keyword,
        Literal,
        String,
        Comment,
        Builtin,
        Other
    };

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
        CodeINI
    };
public:
    CodeToHtmlConverter(const QStringRef input, const QString &lang) Q_DECL_NOTHROW;
    QString process() const;

private:
    QStringRef _input;
    QString _lang;
    Lang currentLang;

    QString escape(QChar c) const;
    QString escapeString(const QStringRef s) const;
    QString setFormat(const QStringRef str, Format format) const;
    void initCodeLangs() const Q_DECL_NOTHROW;

    int highlightNumericLit(const QStringRef input, QString &output, int i) const;
    int highlightComment(QString &output, int i, bool isSingleLine = true) const;
    int highlightWord(int i, const QMultiHash<char, QLatin1String> &data,
         const QStringRef text, QString &output, Format f) const;
    QString xmlHighlighter(const QStringRef text) const;

    static QHash<QString, Lang> _langStringToEnum;

    const QString keywordTagBegin = QStringLiteral("<span class=\"code-keyword\">");
    const QString typeTagBegin = QStringLiteral("<span class=\"code-type\">");
    const QString literalTagBegin = QStringLiteral("<span class=\"code-literal\">");
    const QString commentTagBegin = QStringLiteral("<span class=\"code-comment\">");
    const QString builtinTagBegin = QStringLiteral("<span class=\"code-builtin\">");
    const QString otherTagBegin = QStringLiteral("<span class=\"code-other\">");
    const QString stringTagBegin = QStringLiteral("<span class=\"code-string\">");
    const QString spanEnd = QStringLiteral("</span>");
};

#endif // CODETOHTMLCONVERTER_H
