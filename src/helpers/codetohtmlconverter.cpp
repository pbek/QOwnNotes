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
        return xmlHighlighter();
    case CodeCSS :
        loadCSSData(types, keywords, builtin, literals, others);
        return cssHighlighter(types, keywords);
    case CodeTypeScript:
        loadTypescriptData(types, keywords, builtin, literals, others);
        break;
    case CodeYAML:
        loadYAMLData(types, keywords, builtin, literals, others);
        return ymlHighlighter();
//    case CodeINI:
//        iniHighlighter(text);
//        return;
    default:
        output += escapeString(_input);
        return output;
    }

    const auto textLen = _input.length();
    //reserve extra space
    output.reserve(_input.size() + 100);

    for (int i = 0; i < textLen; ++i) {
        if (_input.at(i) == QLatin1Char(' ')) {
            output += (QLatin1Char(' '));
        } else if (_input.at(i) == QLatin1Char('\'') || _input.at(i) == QLatin1Char('"')) {
            i = highlightStringLiterals(_input.at(i), output, i);
            //escape text at i, otherwise it gets skipped and won't be present in output
            if (i < textLen )
                output += escape(_input.at(i));
        } else if (_input.at(i).isDigit()) {
            i = highlightNumericLit(output, i);
        } else if (comment.isNull() && _input.at(i) == QLatin1Char('/')) {
            if(_input.at(i + 1) == QLatin1Char('/')) {
                i = highlightComment(output, i);
            }
            //Multiline comment i.e /* */
            else if (_input.at(i + 1) == QLatin1Char('*')) {
                i = highlightComment(output, i, false);
            }
        } else if (_input.at(i) == comment) {
            i = highlightComment(output, i);
        } else if (_input.at(i) == QLatin1Char('<') || _input.at(i) == QLatin1Char('>') ||
                   _input.at(i) == QLatin1Char('&')) {
            output += escape(_input.at(i));
        }
        else if(_input.at(i).isLetter()) {
            int pos = i;
            i = highlightWord(i, types, output, Format::Type);
            if (i < textLen && !_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, keywords, output, Format::Keyword);
            if (i < textLen && !_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, literals, output, Format::Literal);
            if (i < textLen && !_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, builtin, output, Format::Builtin);
            if (i < textLen && !_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, others, output, Format::Other);
            if (i < textLen && !_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            if (pos == i) {
                int cnt = i;
                while (cnt < textLen) {
                    if (!_input.at(cnt).isLetter())
                        break;
                    output += _input.at(cnt);
                    ++cnt;
                }
                i = cnt;
                if (i < textLen)
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
        case '\n':
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
        case ':':
            if (currentLang == CodeCSS) {
                isPreAllowed = true;
            }
        }
    }

    if (!isPreAllowed) {
        output += escape(_input.at(i));
        return i;
    }

    int start = i;

    if ((i+1) >= _input.length()) {
        output += setFormat(_input.mid(i, 1), Format::Literal);
        return ++i;
    }

    ++i;

    //hex numbers highlighting (only if there's a preceding zero)
    if (_input.at(i) == QLatin1Char('x') && _input.at(i - 1) == QLatin1Char('0'))
        ++i;

    //unroll till we find a non number
    while (i < _input.length()) {
        if (!_input.at(i).isNumber() && _input.at(i) != QLatin1Char('.')) break;
        ++i;
    }

    //decrement to go back to the last number in sequence
    i--;

    bool isPostAllowed = false;
    if (i+1 == _input.length()) isPostAllowed = true;
    else {
        //these values are allowed after a number
        switch(_input.at(i + 1).toLatin1()) {
        case '\n':
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
        case 'p':
            if (currentLang == CodeCSS && _input.at(i+2) == QLatin1Char('x'))
                if ( (i+3 < _input.length()) &&
                    (_input.at(i+3) == QLatin1Char(';') || _input.at(i+3) == QLatin1Char('\n')) )
                    isPostAllowed = true;
            break;
        case 'e':
            if (currentLang == CodeCSS && _input.at(i+2) == QLatin1Char('m'))
                if ( (i+3 < _input.length()) &&
                    (_input.at(i+3) == QLatin1Char(';') || _input.at(i+3) == QLatin1Char('\n')) )
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
        return --i;
    }
    //this is not a number, add this to the output but no formatting
    //no escaping necessary as we know these are only numbers
    else {
        //inc to capture full number
        ++i;
        output += _input.mid(start, i - start);
        //decrement, so that the current index can be used in the main loop.
        //otherwise it will be skipped. We can add it to output right here
        //but in some cases like px or em after a number 100px, p will be added to
        //output but the main loop will not be able to get px highlighted properly
        return --i;
    }
}

int CodeToHtmlConverter::highlightStringLiterals(QChar strType, QString &output, int i) const {
    int start = i;
    ++i;
    bool foundEnd = false;

    while (i < _input.length()) {
        //look for string end
        //make sure it's not an escape seq
        if (_input.at(i) == strType && _input.at(i-1) != '\\') {
            foundEnd = true;
            ++i;
            break;
        }
        //look for escape sequence
        if (_input.at(i) == '\\' && (i+1) < _input.length()) {
            int len = 0;
            switch(_input.at(i+1).toLatin1()) {
            case 'a':
            case 'b':
            case 'e':
            case 'f':
            case 'n':
            case 'r':
            case 't':
            case 'v':
            case '\'':
            case '"':
            case '\\':
            case '\?':
                //2 because we have to highlight \ as well as the following char
                len = 2;
                break;
            //octal esc sequence \123
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            {
                if (i + 4 <= _input.length()) {
                    bool isCurrentOctal = true;
                    if (!isOctal(_input.at(i+2).toLatin1())) {
                        isCurrentOctal = false;
                        break;
                    }
                    if (!isOctal(_input.at(i+3).toLatin1())) {
                        isCurrentOctal = false;
                        break;
                    }
                    len = isCurrentOctal ? 4 : 0;
                }
                break;
            }
            //hex numbers \xFA
            case 'x':
            {
                if (i + 3 <= _input.length()) {
                    bool isCurrentHex = true;
                    if (!isHex(_input.at(i+2).toLatin1())) {
                        isCurrentHex = false;
                        break;
                    }
                    if (!isHex(_input.at(i+3).toLatin1())) {
                        isCurrentHex = false;
                        break;
                    }
                    len = isCurrentHex ? 4 : 0;
                }
                break;
            }
            //TODO: implement unicode code point escaping
            default:
                break;
            }

            //if len is zero, that means this wasn't an esc seq
            //increment i so that we skip this backslash
            if (len == 0) {
                ++i;
                continue;
            }

            //first highlight everything till this point as string
            output += setFormat(_input.mid(start, i - start), Format::String);
            //then highlight the escape sequence
            output += setFormat(_input.mid(i, len), Format::Literal);
            i += len;
            //set start to current pos
            start = i;
            continue;
        }
        ++i;
    }
    if (!foundEnd)
        --i;
    output += setFormat(_input.mid(start, i - start), Format::String);
    return i;
}

int CodeToHtmlConverter::highlightComment(QString &output, int i, bool isSingleLine) const
{
    int endPos = -1;
    if (isSingleLine) {
        endPos = _input.indexOf(QLatin1Char('\n'), i);
    } else {
        endPos = _input.indexOf(QLatin1String("*/"), i);
        if (endPos == -1) {
            //look for the last endline
            endPos = _input.lastIndexOf(QLatin1Char('\n'));
        } else {
            endPos += 2;
        }
    }
    output += setFormat(_input.mid(i, endPos - i), Format::Comment);
    i = endPos;
    //escape the endline
    if (endPos < _input.length())
        output += escape(_input.at(endPos));

    return i;
}

int CodeToHtmlConverter::highlightWord(int i, const QMultiHash<char, QLatin1String> &data,
                                       QString &output, CodeToHtmlConverter::Format f) const
{
    QList<QLatin1String> wordList;
    // check if we are at the beginning OR if this is the start of a word
    // AND the current char is present in the data structure
    if ( ( i == 0 || !_input.at(i-1).isLetter()) && data.contains(_input.at(i).toLatin1())) {
        wordList = data.values(_input.at(i).toLatin1());
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

QString CodeToHtmlConverter::xmlHighlighter() const {
    if (_input.isEmpty()) return QLatin1String("");
    const auto textLen = _input.length();
    QString output = QLatin1String("");

    for (int i = 0; i < textLen; ++i) {
        if (_input.at(i) == QLatin1Char('<') && _input.at(i+1) != QLatin1Char('!')) {

            int found = _input.indexOf(QLatin1Char('>'), i);
            if (found > 0) {
                output += escape(_input.at(i));
                ++i;
                if (_input.at(i) == QLatin1Char('/')) {
                    output += escape(_input.at(i));
                    ++i;
                }
                QStringRef tag = _input.mid(i, found - i);
                bool hasEqual = false;
                int equalPos = -1;
                if (tag.contains(QLatin1Char('='))) {
                    hasEqual = true;
                    equalPos = _input.indexOf(QLatin1Char('='), i);
                }
                if (hasEqual) {
                    int spacePos = _input.indexOf(QLatin1Char(' '), i);
                    output += setFormat(_input.mid(i, spacePos - i), Format::Keyword);

                    //add the space
                    output += escape(_input.at(spacePos));
                    i = spacePos + 1;

                    //highlight everything from space to equal
                    output += setFormat(_input.mid(i, equalPos - i), Format::Builtin);
                    //set i to equal
                    i = equalPos;
                }
                else {
                    output += setFormat(_input.mid(i, found - i), Format::Keyword);
                    i = found;
                }
                output += escape(_input.at(i));
            }
        }

        else if (_input.at(i) == QLatin1Char('=')) {
            int lastSpace = _input.lastIndexOf(QLatin1Char(' '), i);
            if (lastSpace == i-1) {
                lastSpace = _input.lastIndexOf(QLatin1Char(' '), i-2);
            }
            if (lastSpace > 0) {
                output += setFormat(_input.mid(lastSpace, i - lastSpace), Format::Builtin);
                output += escape(_input.at(i));
            }
        }

        else if (_input.at(i) == QLatin1Char('\"') || _input.at(i) == QLatin1Char('\'')) {
            //find the next end of string
            int next = _input.indexOf(_input.at(i), i + 1);
            bool isEndline = false;
            //if not found
            if (next == -1) {
                //search for endline
                next = _input.indexOf(QLatin1Char('\n'), i);
                isEndline = true;
            }
            //extract it
            //next + 1 because we have to include the ' or "
            QStringRef str = _input.mid(i, (next + 1) - i);
            output += setFormat(str, Format::String);
            if (isEndline)
                output += QLatin1Char('\n');
            i = next;
        }
        else {
            output += escape(_input.at(i));
        }
    }
    return output;
}

/**
 * @brief CSS highlighter
 * @return
 */
QString CodeToHtmlConverter::cssHighlighter(const QMultiHash<char, QLatin1String> &types,
                                            const QMultiHash<char, QLatin1String> &keywords) const {
    if (_input.isEmpty())
        return QLatin1String("");
    const auto textLen = _input.length();
    QString output = QLatin1String("");
    //reserve extra space
    output.reserve(textLen + 100);

    for (int i = 0; i < textLen; ++i) {
        if (_input.at(i) == QLatin1Char(' ')) {
            output += (QLatin1Char(' '));
        } else if (_input.at(i) == QLatin1Char('.') || _input.at(i) == QLatin1Char('#')) {
            if (_input.at(i+1).isSpace() || _input.at(i+1).isNumber()) {
                output += _input.at(i);
                continue;
            }
            int bracketOpen = _input.indexOf(QLatin1Char('{'), i);
            if (bracketOpen < 0) {
                bracketOpen = _input.indexOf(QLatin1Char('\n'), i);
            }
            output += setFormat(_input.mid(i, bracketOpen - i), Format::Keyword);
            i = bracketOpen;
            output += _input.at(i);
        }  else if (_input.at(i) == QLatin1Char('\'') || _input.at(i) == QLatin1Char('"')) {
            i = highlightStringLiterals(_input.at(i), output, i);
            //escape text at i, otherwise it gets skipped and won't be present in output
            if (i < textLen )
                output += escape(_input.at(i));
        } else if (_input.at(i).isDigit()) {
            i = highlightNumericLit(output, i);
        } else if (_input.at(i) == QLatin1Char('/')) {
            if(_input.at(i + 1) == QLatin1Char('/')) {
                i = highlightComment(output, i);
            }
            //Multiline comment i.e /* */
            else if (_input.at(i + 1) == QLatin1Char('*')) {
                i = highlightComment(output, i, false);
            }
        } else if (_input.at(i) == QLatin1Char('<') || _input.at(i) == QLatin1Char('>') ||
                   _input.at(i) == QLatin1Char('&')) {
            output += escape(_input.at(i));
        } else if(_input.at(i).isLetter()) {
            int pos = i;
            i = highlightWord(i, types, output, Format::Type);
            if (i < textLen && !_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            i = highlightWord(i, keywords, output, Format::Keyword);
            if (i < textLen && !_input.at(i).isLetter()) {
                output += escape(_input.at(i));
                continue;
            }

            if (pos == i) {
                int cnt = i;
                while (cnt < textLen) {
                    if (!_input.at(cnt).isLetter())
                        break;
                    output += _input.at(cnt);
                    ++cnt;
                }
                i = cnt;
                if (i < textLen)
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

/**
 * @brief YAML highlighter
 * @return
 */
QString CodeToHtmlConverter::ymlHighlighter() const {
    if (_input.isEmpty()) return QLatin1String("");
    const auto textLen = _input.length();

    QString output = QLatin1String("");
    output.reserve(textLen + 100);

    for (int i = 0; i < textLen; ++i) {
        //we found a string literal
        if (_input.at(i) == QLatin1Char('"') || _input.at(i) == QLatin1Char('\'')) {
            i = highlightStringLiterals(_input.at(i), output, i);
        } else if (_input.at(i) == QLatin1Char('#')) {
            i = highlightComment(output, i);
        } else {
            int colon = _input.indexOf(QLatin1Char(':'), i);
            if (colon > 0) {
                //move i to the beginning of the word
                int cursor = i;
                while(cursor < textLen && !_input.at(cursor).isLetter())
                    ++cursor;
                //add this to output
                output += escapeString(_input.mid(i, cursor - i));
                i = cursor;

                output += setFormat(_input.mid(i, colon - i), Format::Keyword);
                i = colon;
                int endLine = _input.indexOf(QLatin1Char('\n'), i);
                if (endLine > 0) {
                    QStringRef line = _input.mid(i, endLine - i);
                    if (line.contains(QLatin1Char('#'))) {
                        int hashPos = _input.indexOf(QLatin1Char('#'), i);
                        //first add everything till the # into output
                        output += _input.mid(i, hashPos - i);
                        //advance i
                        i = hashPos;
                        //then add the comment
                        output += setFormat(_input.mid(i, endLine - i), Format::Comment);
                        i = endLine;
                    }

                    if (i != endLine)
                        output += escapeString(_input.mid(i, endLine - i));
                    i = endLine;
                }
            }
            output += escape(_input.at(i));
        }
    }

    output.squeeze();
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

    return str.toString();
}
