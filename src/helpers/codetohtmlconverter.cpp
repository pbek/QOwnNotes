#include "codetohtmlconverter.h"
#include <QDebug>
#include "libraries/qmarkdowntextedit/qownlanguagedata.h"
#include <QStringBuilder>

QHash<QString, CodeToHtmlConverter::Lang> CodeToHtmlConverter::_langStringToEnum;

CodeToHtmlConverter::CodeToHtmlConverter(const QStringRef input, const QString &lang) Q_DECL_NOTHROW
    : _input(input), _lang(lang)
{
    if (_langStringToEnum.isEmpty())
        initCodeLangs();

    currentLang = _langStringToEnum.value(_lang);
}

void CodeToHtmlConverter::initCodeLangs() const Q_DECL_NOTHROW
{
    CodeToHtmlConverter::_langStringToEnum =
            QHash<QString, CodeToHtmlConverter::Lang> {
        {QLatin1String("bash"),        CodeToHtmlConverter::CodeBash},
        {QLatin1String("c"),           CodeToHtmlConverter::CodeC},
        {QLatin1String("cpp"),         CodeToHtmlConverter::CodeCpp},
        {QLatin1String("cxx"),         CodeToHtmlConverter::CodeCpp},
        {QLatin1String("c++"),         CodeToHtmlConverter::CodeCpp},
        {QLatin1String("c#"),          CodeToHtmlConverter::CodeCSharp},
        {QLatin1String("csharp"),      CodeToHtmlConverter::CodeCSharp},
        {QLatin1String("css"),         CodeToHtmlConverter::CodeCSS},
        {QLatin1String("go"),          CodeToHtmlConverter::CodeCSharp},
        {QLatin1String("html"),        CodeToHtmlConverter::CodeXML},
        {QLatin1String("ini"),         CodeToHtmlConverter::CodeINI},
        {QLatin1String("java"),        CodeToHtmlConverter::CodeJava},
        {QLatin1String("javascript"),  CodeToHtmlConverter::CodeJava},
        {QLatin1String("js"),          CodeToHtmlConverter::CodeJs},
        {QLatin1String("json"),        CodeToHtmlConverter::CodeJSON},
        {QLatin1String("php"),         CodeToHtmlConverter::CodePHP},
        {QLatin1String("py"),          CodeToHtmlConverter::CodePython},
        {QLatin1String("python"),      CodeToHtmlConverter::CodePython},
        {QLatin1String("qml"),         CodeToHtmlConverter::CodeQML},
        {QLatin1String("rust"),        CodeToHtmlConverter::CodeRust},
        {QLatin1String("sh"),          CodeToHtmlConverter::CodeBash},
        {QLatin1String("sql"),         CodeToHtmlConverter::CodeSQL},
        {QLatin1String("ts"),          CodeToHtmlConverter::CodeTypeScript},
        {QLatin1String("typescript"),  CodeToHtmlConverter::CodeTypeScript},
        {QLatin1String("v"),           CodeToHtmlConverter::CodeV},
        {QLatin1String("xml"),         CodeToHtmlConverter::CodeXML},
        {QLatin1String("yml"),         CodeToHtmlConverter::CodeYAML},
        {QLatin1String("yaml"),        CodeToHtmlConverter::CodeYAML}
    };

}

QString CodeToHtmlConverter::process() const
{
    if (_input.isEmpty()) {
        return QLatin1String("");
    }

    QMultiHash<char, QLatin1String> keywords{};
    QMultiHash<char, QLatin1String> others{};
    QMultiHash<char, QLatin1String> types{};
    QMultiHash<char, QLatin1String> builtin{};
    QMultiHash<char, QLatin1String> literals{};

    QChar comment;

    QList<QLatin1String> wordList;

    QString output = QLatin1String("");

    switch(currentLang) {
    case CodeCpp:
        loadCppData(types, keywords, builtin, literals, others);
        break;
    case CodeJs :
        loadJSData(types, keywords, builtin, literals, others);
        break;
    case CodeC :
        loadCppData(types, keywords, builtin, literals, others);
        break;
    case CodeBash :
        loadShellData(types, keywords, builtin, literals, others);
        comment = QLatin1Char('#');
        break;
    case CodePHP :
        loadPHPData(types, keywords, builtin, literals, others);
        break;
    case CodeQML :
        loadQMLData(types, keywords, builtin, literals, others);
        break;
    case CodePython :
        loadPythonData(types, keywords, builtin, literals, others);
        comment = QLatin1Char('#');
        break;
    case CodeRust :
        loadRustData(types, keywords, builtin, literals, others);
        break;
    case CodeJava :
        loadJavaData(types, keywords, builtin, literals, others);
        break;
    case CodeCSharp :
        loadCSharpData(types, keywords, builtin, literals, others);
        break;
    case CodeGo :
        loadGoData(types, keywords, builtin, literals, others);
        break;
    case CodeV :
        loadVData(types, keywords, builtin, literals, others);
        break;
    case CodeSQL :
        loadSQLData(types, keywords, builtin, literals, others);
        break;
    case CodeJSON :
        loadJSONData(types, keywords, builtin, literals, others);
        break;
    case CodeXML :
        return xmlHighlighter(_input);
//    case CodeCSS :
//        isCSS = true;
//        loadCSSData(types, keywords, builtin, literals, others);
//        break;
    case CodeTypeScript:
        loadTypescriptData(types, keywords, builtin, literals, others);
        break;
//    case CodeYAML:
//        isYAML = true;
//        comment = '#';
//        loadYAMLData(types, keywords, builtin, literals, others);
//        break;
//    case CodeINI:
//        iniHighlighter(text);
//        return;
    }

    const auto textLen = _input.length();
    //reserve extra space
    output.reserve(_input.size() + 100);

    for (int i = 0; i < textLen; ++i) {
        if (_input[i] == QChar(' ')) {
            output += (QChar(' '));
        } else if (_input.at(i) == QChar('\'') || _input.at(i) == QChar('"')) {
            //find the next end of string
            int next = _input.indexOf(_input.at(i), i + 1);
            bool isEndline = false;
            //if not found
            if (next == -1) {
                //search for endline
                next = _input.indexOf(QChar('\n'), i);
                isEndline = true;
            }
            //extract it
            //next + 1 because we have to include the ' or "
            QStringRef str = _input.mid(i, (next + 1) - i);
            output += setFormat(str, Format::String);
            if (isEndline)
                output += QChar('\n');
            i = next;
        } else if (_input.at(i).isDigit()) {
            i = highlightNumericLit(output, i);
            output += escape(_input.at(i));
        } else if (comment.isNull() && _input.at(i) == QChar('/')) {
            if(_input.at(i + 1) == QChar('/')) {
                i = highlightComment(output, i);
            }
            //Multiline comment i.e /* */
            else if (_input.at(i + 1) == QChar('*')) {
                i = highlightComment(output, i, false);
            }
        } else if (_input.at(i) == comment) {
            i = highlightComment(output, i);
        } else if (_input.at(i) == QChar('<') || _input.at(i) == QChar('>') ||
                   _input.at(i) == QChar('&')) {
            output += escape(_input.at(i));
        }
        else if(_input.at(i).isLetter()) {
            int pos = i;
            i = highlightWord(i, types, output, Format::Type);
            if (!_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, keywords, output, Format::Keyword);
            if (!_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, literals, output, Format::Literal);
            if (!_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, builtin, output, Format::Builtin);
            if (!_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, others, output, Format::Other);
            if (!_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            if (pos == i) {
                int cnt = i;
                while (cnt < textLen) {
                    if (!_input[cnt].isLetter())
                        break;
                    output += _input.at(cnt);
                    ++cnt;
                }
                i = cnt;
                output += escape(_input.at(i));
            }
        }
        else {
            output += _input.at(i);
        }
    }
    //release extra memory
    output.squeeze();
    return output;
}

int CodeToHtmlConverter::highlightNumericLit(QString &output, int i) const
{
    bool isPreAllowed = false;
    if (i == 0) isPreAllowed = true;
    else {
        //these values are allowed before a number
        switch(_input.at(i - 1).toLatin1()) {
        case '[':
        case '(':
        case '{':
        case ' ':
        case ',':
        case '=':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '<':
        case '>':
            isPreAllowed = true;
            break;
        }
    }

    if (!isPreAllowed) {
        output += escape(_input.at(i));
        return ++i;
    }

    int start = i;

    if ((i+1) >= _input.length()) {
        output += setFormat(_input.mid(i, 1), Format::Literal);
        return ++i;
    }

    ++i;
    //hex numbers highlighting (only if there's a preceding zero)
    if (_input.at(i) == QChar('x') && _input.at(i - 1) == QChar('0'))
        ++i;

    while (i < _input.length()) {
        if (!_input.at(i).isNumber() && _input.at(i) != QChar('.')) break;
        ++i;
    }

    i--;

    bool isPostAllowed = false;
    if (i+1 == _input.length()) isPostAllowed = true;
    else {
        //these values are allowed after a number
        switch(_input.at(i + 1).toLatin1()) {
        case ']':
        case ')':
        case '}':
        case ' ':
        case ',':
        case '=':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '>':
        case '<':
        case ';':
            isPostAllowed = true;
            break;
        // for 100u, 1.0F
        case 'u':
        case 'l':
        case 'f':
        case 'U':
        case 'L':
        case 'F':
            isPostAllowed = true;
            ++i;
            break;
        }
    }
    if (isPostAllowed) {
        int end = ++i;
        output += setFormat(_input.mid(start, end - start), Format::Literal);
    }
    return i;
}

int CodeToHtmlConverter::highlightComment(QString &output, int i, bool isSingleLine) const
{
    int endPos = -1;
    if (isSingleLine) {
        endPos = _input.indexOf(QChar('\n'), i);
    } else {
        endPos = _input.indexOf(QLatin1String("*/"), i);
        if (endPos == -1) {
            endPos = _input.length();
        } else {
            endPos += 2;
        }
    }
    output += setFormat(_input.mid(i, endPos - i), Format::Comment);
    i = endPos;
    //escape the endline
    output += escape(_input.at(endPos));

    return i;
}

int CodeToHtmlConverter::highlightWord(int i, const QMultiHash<char, QLatin1String> &data,
                                       QString &output, CodeToHtmlConverter::Format f) const
{
    QList<QLatin1String> wordList;
    // check if we are at the beginning OR if this is the start of a word
    // AND the current char is present in the data structure
    if ( ( i == 0 || !_input[i-1].isLetter()) && data.contains(_input[i].toLatin1())) {
        wordList = data.values(_input[i].toLatin1());
#if QT_VERSION >= 0x050700
        for(const QLatin1String &word : qAsConst(wordList)) {
#else
        for(const QLatin1String &word : wordList) {
#endif
            if (word == _input.mid(i, word.size())) {
                //check if we are at the end of text OR if we have a complete word
                if ( i + word.size() == _input.length() || !_input.at(i + word.size()).isLetter()) {
                    output += setFormat(_input.mid(i, word.size()), f);
                    i += word.size();
                }
            }
        }
    }
    return i;
}

QString CodeToHtmlConverter::escape(QChar c) const
{
    switch (c.toLatin1()) {
    case '\'':
        return QStringLiteral("&#39;");
    case '"':
        return QStringLiteral("&quot;");
    case '&':
        return QStringLiteral("&amp;");
    case '<':
        return QStringLiteral("&lt;");
    case '>':
        return QStringLiteral("&gt;");
    case '/':
        return QStringLiteral("&#47;");
    }
    return c;
}

QString CodeToHtmlConverter::xmlHighlighter(const QStringRef text) const {
    if (text.isEmpty()) return QLatin1String("");
    const auto textLen = text.length();
    QString output = QLatin1String("");

    for (int i = 0; i < textLen; ++i) {
        if (text[i] == QLatin1Char('<') && text[i+1] != QLatin1Char('!')) {

            int found = text.indexOf(QLatin1Char('>'), i);
            if (found > 0) {
                output += escape(text.at(i));
                ++i;
                if (text[i] == QLatin1Char('/')) {
                    output += escape(text.at(i));
                    ++i;
                }
                QStringRef tag = text.mid(i, found - i);
                bool hasEqual = false;
                int equalPos = -1;
                if (tag.contains(QChar('='))) {
                    hasEqual = true;
                    equalPos = text.indexOf(QChar('='), i);
                }
                if (hasEqual) {
                    int spacePos = text.indexOf(QChar(' '), i);
                    output += setFormat(text.mid(i, spacePos - i), Format::Keyword);

                    //add the space
                    output += escape(text.at(spacePos));
                    i = spacePos + 1;

                    //highlight everything from space to equal
                    output += setFormat(text.mid(i, equalPos - i), Format::Builtin);
                    //set i to equal
                    i = equalPos;
                }
                else {
                    output += setFormat(text.mid(i, found - i), Format::Keyword);
                    i = found;
                }
                output += escape(text.at(i));
            }
        }

        else if (text[i] == QLatin1Char('=')) {
            int lastSpace = text.lastIndexOf(QLatin1Char(' '), i);
            if (lastSpace == i-1) {
                lastSpace = text.lastIndexOf(QLatin1Char(' '), i-2);
            }
            if (lastSpace > 0) {
                output += setFormat(text.mid(lastSpace, i - lastSpace), Format::Builtin);
                output += escape(text.at(i));
            }
        }

        else if (text[i] == QLatin1Char('\"') || text[i] == QLatin1Char('\'')) {
            //find the next end of string
            int next = text.indexOf(text.at(i), i + 1);
            bool isEndline = false;
            //if not found
            if (next == -1) {
                //search for endline
                next = text.indexOf(QChar('\n'), i);
                isEndline = true;
            }
            //extract it
            //next + 1 because we have to include the ' or "
            QStringRef str = text.mid(i, (next + 1) - i);
            output += setFormat(str, Format::String);
            if (isEndline)
                output += QChar('\n');
            i = next;
        }
        else {
            output += escape(text.at(i));
        }
    }
    return output;
}

QString CodeToHtmlConverter::escapeString(const QStringRef s) const
{
    QString ret = QLatin1String("");
    ret.reserve(s.length());
    for (int i = 0; i < s.length(); ++i) {
        ret += escape(s.at(i));
    }
    return ret;
}

QString CodeToHtmlConverter::setFormat(const QStringRef str, CodeToHtmlConverter::Format format) const
{
    switch(format) {
    case Type:
        return typeTagBegin % escapeString(str) % spanEnd;
    case Keyword:
        return keywordTagBegin % escapeString(str) % spanEnd;
    case Literal:
        return literalTagBegin % escapeString(str) % spanEnd;
    case String:
        return stringTagBegin % escapeString(str) % spanEnd;
    case Builtin:
        return builtinTagBegin % escapeString(str) % spanEnd;
    case Other:
        return otherTagBegin % escapeString(str) % spanEnd;
    case Comment:
        return commentTagBegin % escapeString(str) % spanEnd;
    }
}
