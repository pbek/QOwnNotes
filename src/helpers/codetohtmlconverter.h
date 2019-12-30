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
    CodeToHtmlConverter(const QString &input, const QString &lang);
    QString process();

private:
    QString _input;
    QString _lang;
    Lang currentLang;

    QString escape(QChar c);
    QString escapeString(const QString &s);
    QString setFormat(const QString &str, Format format);
    void initCodeLangs();

    int highlightNumericLit(const QString &input, QString &output, int i);
    int highlightWord(int i, const QMultiHash<char, QLatin1String> &data,
         const QString &text, QString &output, Format f);
    QString xmlHighlighter(const QString &text);

    static QHash<QString, Lang> _langStringToEnum;

    const QString keywordTagBegin = "<span class=\"code-keyword\">";
    const QString typeTagBegin = "<span class=\"code-type\">";
    const QString literalTagBegin = "<span class=\"code-literal\">";
    const QString commentTagBegin = "<span class=\"code-comment\">";
    const QString builtinTagBegin = "<span class=\"code-builtin\">";
    const QString otherTagBegin = "<span class=\"code-other\">";
    const QString stringTagBegin = "<span class=\"code-string\">";
    const QString spanEnd = "</span>";
};

#endif // CODETOHTMLCONVERTER_H
