#include "codetohtmlconverter.h"

#include <QDebug>
#include <QStringBuilder>
#include <QtCore/QRegularExpression>

#include "libraries/qmarkdowntextedit/qownlanguagedata.h"

QHash<QString, CodeToHtmlConverter::Lang> CodeToHtmlConverter::_langStringToEnum;

CodeToHtmlConverter::CodeToHtmlConverter(const QString &lang) Q_DECL_NOTHROW {
    if (_langStringToEnum.isEmpty()) initCodeLangs();

    _currentLang = _langStringToEnum.value(lang.trimmed().toLower());
    qDebug() << "Code block of lang detected:" << lang << _currentLang;
}

void CodeToHtmlConverter::initCodeLangs() Q_DECL_NOTHROW {
    if (!_langStringToEnum.isEmpty()) return;
    _langStringToEnum = QHash<QString, CodeToHtmlConverter::Lang>{
        {QStringLiteral("bash"), CodeToHtmlConverter::CodeBash},
        {QStringLiteral("c"), CodeToHtmlConverter::CodeC},
        {QStringLiteral("cpp"), CodeToHtmlConverter::CodeCpp},
        {QStringLiteral("cxx"), CodeToHtmlConverter::CodeCpp},
        {QStringLiteral("c++"), CodeToHtmlConverter::CodeCpp},
        {QStringLiteral("c#"), CodeToHtmlConverter::CodeCSharp},
        {QStringLiteral("cmake"), CodeToHtmlConverter::CodeCMake},
        {QStringLiteral("csharp"), CodeToHtmlConverter::CodeCSharp},
        {QStringLiteral("css"), CodeToHtmlConverter::CodeCSS},
        {QStringLiteral("go"), CodeToHtmlConverter::CodeGo},
        {QStringLiteral("html"), CodeToHtmlConverter::CodeXML},
        {QStringLiteral("ini"), CodeToHtmlConverter::CodeINI},
        {QStringLiteral("java"), CodeToHtmlConverter::CodeJava},
        {QStringLiteral("javascript"), CodeToHtmlConverter::CodeJs},
        {QStringLiteral("js"), CodeToHtmlConverter::CodeJs},
        {QStringLiteral("json"), CodeToHtmlConverter::CodeJSON},
        {QStringLiteral("make"), CodeToHtmlConverter::CodeMake},
        {QLatin1String("nix"), CodeToHtmlConverter::CodeNix},
        {QStringLiteral("php"), CodeToHtmlConverter::CodePHP},
        {QStringLiteral("py"), CodeToHtmlConverter::CodePython},
        {QStringLiteral("python"), CodeToHtmlConverter::CodePython},
        {QStringLiteral("qml"), CodeToHtmlConverter::CodeQML},
        {QStringLiteral("rust"), CodeToHtmlConverter::CodeRust},
        {QStringLiteral("sh"), CodeToHtmlConverter::CodeBash},
        {QStringLiteral("sql"), CodeToHtmlConverter::CodeSQL},
        {QStringLiteral("ts"), CodeToHtmlConverter::CodeTypeScript},
        {QStringLiteral("typescript"), CodeToHtmlConverter::CodeTypeScript},
        {QStringLiteral("v"), CodeToHtmlConverter::CodeV},
        {QStringLiteral("vex"), CodeToHtmlConverter::CodeVex},
        {QStringLiteral("xml"), CodeToHtmlConverter::CodeXML},
        {QStringLiteral("yml"), CodeToHtmlConverter::CodeYAML},
        {QStringLiteral("yaml"), CodeToHtmlConverter::CodeYAML},
        {QStringLiteral("forth"), CodeToHtmlConverter::CodeForth},
        {QStringLiteral("systemverilog"), CodeToHtmlConverter::CodeSystemVerilog},
        {QStringLiteral("gdscript"), CodeToHtmlConverter::CodeGDScript}};
}

QString CodeToHtmlConverter::process(const QString &input) const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    qDebug() << "Going to highlight input:" << StringView(input).mid(0, 12)
             << ", with lang:" << _currentLang;
    return process(StringView(input));
#else
    qDebug() << "Going to highlight input:" << StringView(&input).mid(0, 12)
             << ", with lang:" << _currentLang;
    return process(StringView(&input));
#endif
}

QString CodeToHtmlConverter::process(StringView input) const {
    if (input.isEmpty()) {
        return QLatin1String("");
    }

    LangData keywords;
    LangData others;
    LangData types;
    LangData builtin;
    LangData literals;

    QChar comment;
    QString output = QLatin1String("");

    switch (_currentLang) {
        case CodeCpp:
            loadCppData(types, keywords, builtin, literals, others);
            break;
        case CodeJs:
            loadJSData(types, keywords, builtin, literals, others);
            break;
        case CodeC:
            loadCppData(types, keywords, builtin, literals, others);
            break;
        case CodeBash:
            loadShellData(types, keywords, builtin, literals, others);
            comment = QLatin1Char('#');
            break;
        case CodePHP:
            loadPHPData(types, keywords, builtin, literals, others);
            break;
        case CodeQML:
            loadQMLData(types, keywords, builtin, literals, others);
            break;
        case CodePython:
            loadPythonData(types, keywords, builtin, literals, others);
            comment = QLatin1Char('#');
            break;
        case CodeRust:
            loadRustData(types, keywords, builtin, literals, others);
            break;
        case CodeJava:
            loadJavaData(types, keywords, builtin, literals, others);
            break;
        case CodeCSharp:
            loadCSharpData(types, keywords, builtin, literals, others);
            break;
        case CodeGo:
            loadGoData(types, keywords, builtin, literals, others);
            break;
        case CodeV:
            loadVData(types, keywords, builtin, literals, others);
            break;
        case CodeVex:
            loadVEXData(types, keywords, builtin, literals, others);
            break;
        case CodeSQL:
            loadSQLData(types, keywords, builtin, literals, others);
            break;
        case CodeJSON:
            loadJSONData(types, keywords, builtin, literals, others);
            break;
        case CodeXML:
            return xmlHighlighter(input);
        case CodeCSS:
            loadCSSData(types, keywords, builtin, literals, others);
            return cssHighlighter(input, types, keywords);
        case CodeTypeScript:
            loadTypescriptData(types, keywords, builtin, literals, others);
            break;
        case CodeYAML:
            loadYAMLData(types, keywords, builtin, literals, others);
            return ymlHighlighter(input);
        case CodeINI:
            return iniHighlighter(input);
        case CodeMake:
            loadMakeData(types, keywords, builtin, literals, others);
            comment = QLatin1Char('#');
            break;
        case CodeNix:
            loadNixData(types, keywords, builtin, literals, others);
            comment = QLatin1Char('#');
            break;
        case CodeCMake:
            loadCMakeData(types, keywords, builtin, literals, others);
            comment = QLatin1Char('#');
            break;
        case CodeForth:
            loadForthData(types, keywords, builtin, literals, others);
            comment = QLatin1Char('\\');
            break;
        case CodeSystemVerilog:
            loadSystemVerilogData(types, keywords, builtin, literals, others);
            break;
        case CodeGDScript:
            loadGDScriptData(types, keywords, builtin, literals, others);
            comment = QLatin1Char('#');
            break;
        default:
            output += escapeString(input);
            return output;
    }

    const auto textLen = input.length();
    // reserve extra space
    output.reserve(input.size() + 100);

    for (int i = 0; i < textLen; ++i) {
        if (input.at(i) == QLatin1Char(' ')) {
            output += (QLatin1Char(' '));
        } else if (input.at(i) == QLatin1Char('\'') || input.at(i) == QLatin1Char('"')) {
            i = highlightStringLiterals(input, input.at(i), output, i);
        } else if (input.at(i).isDigit()) {
            i = highlightNumericLit(input, output, i);
        } else if (comment.isNull() && input.at(i) == QLatin1Char('/')) {
            if (input.at(i + 1) == QLatin1Char('/')) i = highlightComment(input, output, i);
            // Multiline comment i.e /* */
            else if (input.at(i + 1) == QLatin1Char('*'))
                i = highlightComment(input, output, i, false);
            else
                output += escape(input.at(i));
        } else if (input.at(i) == comment) {
            i = highlightComment(input, output, i);
        } else if (input.at(i) == QLatin1Char('<') || input.at(i) == QLatin1Char('>') ||
                   input.at(i) == QLatin1Char('&')) {
            output += escape(input.at(i));
        } else if (input.at(i).isLetter()) {
            int pos = i;
            i = highlightWord(i, types, input, output, Format::Type);
            if (i < textLen && !input.at(i).isLetter()) {
                output += escape(input.at(i));
                continue;
            }

            i = highlightWord(i, keywords, input, output, Format::Keyword);
            if (i < textLen && !input.at(i).isLetter()) {
                output += escape(input.at(i));
                continue;
            }

            i = highlightWord(i, literals, input, output, Format::Literal);
            if (i < textLen && !input.at(i).isLetter()) {
                output += escape(input.at(i));
                continue;
            }

            i = highlightWord(i, builtin, input, output, Format::Builtin);
            if (i < textLen && !input.at(i).isLetter()) {
                output += escape(input.at(i));
                continue;
            }

            i = highlightWord(i, others, input, output, Format::Other);
            if (i < textLen && !input.at(i).isLetter()) {
                output += escape(input.at(i));
                continue;
            }

            if (pos == i) {
                int cnt = i;
                while (cnt < textLen) {
                    if (!input.at(cnt).isLetter()) break;
                    output += input.at(cnt);
                    ++cnt;
                }
                i = cnt;
                i--;
            }
        } else {
            output += escape(input.at(i));
        }
    }
    // release extra memory
    output.squeeze();
    return output;
}

int CodeToHtmlConverter::highlightNumericLit(StringView input, QString &output, int i) const {
    bool isPreAllowed = false;
    if (i == 0)
        isPreAllowed = true;
    else {
        // these values are allowed before a number
        switch (input.at(i - 1).toLatin1()) {
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
                if (_currentLang == CodeCSS) {
                    isPreAllowed = true;
                }
                break;
        }
    }

    if (!isPreAllowed) {
        output += escape(input.at(i));
        return i;
    }

    int start = i;

    if ((i + 1) >= input.length()) {
        output += setFormat(input.mid(i, 1), Format::Literal);
        return ++i;
    }

    ++i;

    // hex numbers highlighting (only if there's a preceding zero)
    if (input.at(i) == QLatin1Char('x') && input.at(i - 1) == QLatin1Char('0')) ++i;

    // unroll till we find a non number
    while (i < input.length()) {
        if (!input.at(i).isNumber() && input.at(i) != QLatin1Char('.')) break;
        ++i;
    }

    // decrement to go back to the last number in sequence
    i--;

    bool isPostAllowed = false;
    if (i + 1 == input.length())
        isPostAllowed = true;
    else {
        // these values are allowed after a number
        switch (input.at(i + 1).toLatin1()) {
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
                if (_currentLang == CodeCSS && input.at(i + 2) == QLatin1Char('x'))
                    if ((i + 3 < input.length()) && (input.at(i + 3) == QLatin1Char(';') ||
                                                     input.at(i + 3) == QLatin1Char('\n')))
                        isPostAllowed = true;
                break;
            case 'e':
                if (_currentLang == CodeCSS && input.at(i + 2) == QLatin1Char('m'))
                    if ((i + 3 < input.length()) && (input.at(i + 3) == QLatin1Char(';') ||
                                                     input.at(i + 3) == QLatin1Char('\n')))
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
        const int end = ++i;
        output += setFormat(input.mid(start, end - start), Format::Literal);
        return --i;
    }
    // this is not a number, add this to the output but no formatting
    // no escaping necessary as we know these are only numbers
    else {
        // inc to capture full number
        ++i;
        output += input.mid(start, i - start);
        // decrement, so that the current index can be used in the main loop.
        // otherwise it will be skipped. We can add it to output right here
        // but in some cases like px or em after a number 100px, p will be added
        // to output but the main loop will not be able to get px highlighted
        // properly
        return --i;
    }
}

int CodeToHtmlConverter::highlightStringLiterals(StringView input, QChar strType, QString &output,
                                                 int i) {
    int start = i;
    ++i;

    while (i < input.length()) {
        // look for string end
        // make sure it's not an escape seq
        if (input.at(i) == strType && input.at(i - 1) != '\\') {
            ++i;
            break;
        }
        // look for escape sequence
        if (input.at(i) == '\\' && (i + 1) < input.length()) {
            int len = 0;
            switch (input.at(i + 1).toLatin1()) {
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
                    // 2 because we have to highlight \ as well as the following
                    // char
                    len = 2;
                    break;
                // octal esc sequence \123
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7': {
                    if (i + 4 <= input.length()) {
                        const bool isCurrentOctal = (isOctal(input.at(i + 2).toLatin1()) &&
                                                     isOctal(input.at(i + 3).toLatin1()))
                                                        ? true
                                                        : false;
                        if (!isCurrentOctal) break;
                        len = isCurrentOctal ? 4 : 0;
                    }
                    break;
                }
                // hex numbers \xFA
                case 'x': {
                    if (i + 3 <= input.length()) {
                        const bool isCurrentHex =
                            (isHex(input.at(i + 2).toLatin1()) && isHex(input.at(i + 3).toLatin1()))
                                ? true
                                : false;
                        if (!isCurrentHex) break;
                        len = isCurrentHex ? 4 : 0;
                    }
                    break;
                }
                // TODO: implement unicode code point escaping
                default:
                    break;
            }

            // if len is zero, that means this wasn't an esc seq
            // increment i so that we skip this backslash
            if (len == 0) {
                ++i;
                continue;
            }

            // first highlight everything till this point as string
            output += setFormat(input.mid(start, i - start), Format::String);
            // then highlight the escape sequence
            output += setFormat(input.mid(i, len), Format::Literal);
            i += len;
            // set start to current pos
            start = i;
            continue;
        }
        ++i;
    }

    if (!input.isEmpty() && i >= input.length() &&
        input.at(input.length() - 1) == QLatin1Char('\n')) {
        // move one position backwards, we want to insert </span> so
        // that it's like </span>\n and not \n</span> i.e., new line
        // should be after
        i--;
    }
    output += setFormat(input.mid(start, i - start), Format::String);

    // decrement i, this ensures that
    // - if current pos wasn't a string we are back to the string start char
    // - if it was a string, we will be at string end char thus not skipping the next character in
    // the next loop iteration
    --i;

    return i;
}

int CodeToHtmlConverter::highlightComment(StringView input, QString &output, int i,
                                          bool isSingleLine) {
    int endPos = -1;
    if (isSingleLine) {
        endPos = input.indexOf(QLatin1Char('\n'), i);
    } else {
        endPos = input.indexOf(QLatin1String("*/"), i);
        if (endPos == -1) {
            // look for the last endline
            endPos = input.lastIndexOf(QLatin1Char('\n'));
        } else {
            endPos += 2;
        }
    }

    auto commentEndPos = endPos - i;
    if (commentEndPos >= 0) {
        output += setFormat(input.mid(i, commentEndPos), Format::Comment);
    } else {
        // Emergency escape, if someone enters a misplaced comment marker
        // directly before the closing mark of the code block
        // See: https://github.com/pbek/QOwnNotes/issues/2959
        return i;
    };

    i = endPos;
    // escape the endline
    if (endPos < input.length()) output += escape(input.at(endPos));

    return i;
}

int CodeToHtmlConverter::highlightWord(int i, const LangData &data, StringView input,
                                       QString &output, CodeToHtmlConverter::Format f) {
    if (data.isEmpty()) return i;
    // check if we are at the beginning OR if this is the start of a word
    // AND the current char is present in the data structure
    if (i == 0 || (!input.at(i - 1).isLetterOrNumber() && input.at(i - 1) != QLatin1Char('_'))) {
        const auto wordList = data.values(input.at(i).toLatin1());
        for (const auto &word : wordList) {
            if (word == input.mid(i, word.size())) {
                // check if we are at the end of text OR if we have a complete
                // word
                if (i + word.size() == input.length() ||
                    !input.at(i + word.size()).isLetterOrNumber()) {
                    output += setFormat(input.mid(i, word.size()), f);
                    i += word.size();
                }
            }
        }
    }
    return i;
}

QString CodeToHtmlConverter::xmlHighlighter(StringView input) const {
    if (input.isEmpty()) return QLatin1String("");
    const auto textLen = input.length();
    QString output = QLatin1String("");

    for (int i = 0; i < textLen; ++i) {
        if (input.at(i) == QLatin1Char('<') && input.at(i + 1) != QLatin1Char('!')) {
            const int found = input.indexOf(QLatin1Char('>'), i);
            if (found > 0) {
                output += escape(input.at(i));
                ++i;
                if (input.at(i) == QLatin1Char('/')) {
                    output += escape(input.at(i));
                    ++i;
                }

                StringView tag = input.mid(i, found - i);

                static const QRegularExpression re(R"(([a-zA-Z0-9]+(\s*=\s*"[^"]*")?))");
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
                // Qt 6.5+: use globalMatchView with StringView
                QRegularExpressionMatchIterator matchIt = re.globalMatchView(tag);
#else
                // Qt 5.x and Qt 6.0-6.4: use globalMatch with QString
                QRegularExpressionMatchIterator matchIt = re.globalMatch(TO_QSTRING(tag));
#endif

                while (matchIt.hasNext()) {
                    QRegularExpressionMatch match = matchIt.next();
                    QString captured = match.captured(0);

                    if (!captured.contains(QLatin1Char('='))) {
                        output += setFormat(captured, Format::Builtin);
                    } else {
                        int eqPos = captured.indexOf(QLatin1Char('='));
                        output += setFormat(captured.left(eqPos), Format::Builtin) + "=" +
                                  setFormat(captured.mid(eqPos + 1, captured.length() - eqPos - 1),
                                            Format::String);
                    }

                    if (matchIt.hasNext()) {
                        output += " ";
                    }
                }

                output += (tag.endsWith(QLatin1Char('/')) ? " />" : ">");

                i = found;
            }
        }

        else if (input.at(i) == QLatin1Char('\"') || input.at(i) == QLatin1Char('\'')) {
            // find the next end of string
            int next = input.indexOf(input.at(i), i + 1);
            bool isEndline = false;
            // if not found
            if (next == -1) {
                // search for endline
                next = input.indexOf(QLatin1Char('\n'), i);
                isEndline = true;
            }
            // extract it
            // next + 1 because we have to include the ' or "
            StringView str = input.mid(i, (next + 1) - i);
            output += setFormat(str, Format::String);
            if (isEndline) output += QLatin1Char('\n');
            i = next;
        } else {
            output += escape(input.at(i));
        }
    }
    return output;
}

/**
 * @brief CSS highlighter
 * @return
 */
QString CodeToHtmlConverter::cssHighlighter(StringView input, const LangData &types,
                                            const LangData &keywords) const {
    if (input.isEmpty()) return QLatin1String("");
    const auto textLen = input.length();
    QString output = QLatin1String("");
    // reserve extra space
    output.reserve(textLen + 100);

    for (int i = 0; i < textLen; ++i) {
        if (input.at(i) == QLatin1Char(' ')) {
            output += (QLatin1Char(' '));
        } else if (input.at(i) == QLatin1Char('.') || input.at(i) == QLatin1Char('#')) {
            if (input.at(i + 1).isSpace() || input.at(i + 1).isNumber()) {
                output += input.at(i);
                continue;
            }
            int bracketOpen = input.indexOf(QLatin1Char('{'), i);
            if (bracketOpen < 0) {
                bracketOpen = input.indexOf(QLatin1Char('\n'), i);
            }
            output += setFormat(input.mid(i, bracketOpen - i), Format::Keyword);
            i = bracketOpen;
            output += input.at(i);
        } else if (input.at(i) == QLatin1Char('\'') || input.at(i) == QLatin1Char('"')) {
            i = highlightStringLiterals(input, input.at(i), output, i);
            // escape text at i, otherwise it gets skipped and won't be present
            // in output
            if (i < textLen) output += escape(input.at(i));
        } else if (input.at(i).isDigit()) {
            i = highlightNumericLit(input, output, i);
        } else if (input.at(i) == QLatin1Char('/')) {
            if (input.at(i + 1) == QLatin1Char('/'))
                i = highlightComment(input, output, i);
            else if (input.at(i + 1) == QLatin1Char('*'))
                i = highlightComment(input, output, i, false);
            else
                output += escape(input.at(i));
        } else if (input.at(i) == QLatin1Char('<') || input.at(i) == QLatin1Char('>') ||
                   input.at(i) == QLatin1Char('&')) {
            output += escape(input.at(i));
        } else if (input.at(i).isLetter()) {
            int pos = i;
            i = highlightWord(i, types, input, output, Format::Type);
            if (i < textLen && !input.at(i).isLetter()) {
                output += escape(input.at(i));
                continue;
            }

            i = highlightWord(i, keywords, input, output, Format::Keyword);
            if (i < textLen && !input.at(i).isLetter()) {
                output += escape(input.at(i));
                continue;
            }

            if (pos == i) {
                int cnt = i;
                while (cnt < textLen) {
                    if (!input.at(cnt).isLetter()) break;
                    output += input.at(cnt);
                    ++cnt;
                }
                i = cnt;
                if (i < textLen) output += escape(input.at(i));
            }
        } else {
            output += input.at(i);
        }
    }
    // release extra memory
    output.squeeze();
    return output;
}

/**
 * @brief YAML highlighter
 * @return
 */
QString CodeToHtmlConverter::ymlHighlighter(StringView input) const {
    if (input.isEmpty()) return QLatin1String("");
    const auto textLen = input.length();

    QString output = QLatin1String("");
    output.reserve(textLen + 100);

    for (int i = 0; i < textLen; ++i) {
        // we found a string literal
        if (input.at(i) == QLatin1Char('"') || input.at(i) == QLatin1Char('\'')) {
            i = highlightStringLiterals(input, input.at(i), output, i);
        } else if (input.at(i) == QLatin1Char('#')) {
            i = highlightComment(input, output, i);
        } else {
            const int colon = input.indexOf(QLatin1Char(':'), i);
            if (colon > 0) {
                // move i to the beginning of the word
                int cursor = i;
                while (cursor < textLen && !input.at(cursor).isLetter()) ++cursor;
                // add this to output
                output += escapeString(input.mid(i, cursor - i));
                i = cursor;

                output += setFormat(input.mid(i, colon - i), Format::Keyword);
                i = colon;
                const int endLine = input.indexOf(QLatin1Char('\n'), i);
                if (endLine > 0) {
                    StringView line = input.mid(i, endLine - i);
                    if (line.contains(QLatin1Char('#'))) {
                        int hashPos = input.indexOf(QLatin1Char('#'), i);
                        // first add everything till the # into output
                        output += input.mid(i, hashPos - i);
                        // advance i
                        i = hashPos;
                        // then add the comment
                        output += setFormat(input.mid(i, endLine - i), Format::Comment);
                        i = endLine;
                    }

                    if (i != endLine) output += escapeString(input.mid(i, endLine - i));
                    i = endLine;
                }
            }
            output += escape(input.at(i));
        }
    }

    output.squeeze();
    return output;
}

QString CodeToHtmlConverter::iniHighlighter(StringView input) const {
    if (input.isEmpty()) return QLatin1String("");
    const auto textLen = input.length();
    QString output;
    output.reserve(input.size() + 100);

    for (int i = 0; i < textLen; ++i) {
        // start of a [section]
        if (input.at(i) == QLatin1Char('[')) {
            int sectionEnd = input.indexOf(QLatin1Char(']'), i);
            if (sectionEnd == -1)
                sectionEnd = input.indexOf(QLatin1Char('\n'), i);
            else
                ++sectionEnd;
            output += setFormat(input.mid(i, sectionEnd - i), Format::Type);
            i = sectionEnd;
            output += escape(input.at(i));
            if (i >= textLen) break;
        }

        // comment ';'
        else if (input.at(i) == QLatin1Char(';')) {
            int end = input.indexOf(QLatin1Char('\n'), i);
            output += setFormat(input.mid(i, end - i), Format::Comment);
            i = end - 1;
            if (i >= textLen) break;
        }

        // key-val
        else if (i == 0 || (input.at(i) == QLatin1Char('\n') && i + 1 < textLen &&
                            input.at(i + 1).isLetter())) {
            int equalsPos = input.indexOf(QLatin1Char('='), i);
            if (equalsPos == -1) equalsPos = input.indexOf(QLatin1Char('\n'), i);
            output += setFormat(input.mid(i, equalsPos - i), Format::Keyword);
            i = equalsPos;
            output += escape(input.at(i));
            if (i >= textLen) break;
        } else {
            output += escape(input.at(i));
        }
    }
    output.squeeze();
    return output;
}

QString CodeToHtmlConverter::escape(QChar c) {
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
        case '~':
            return QStringLiteral("&#126;");
        case '`':
            return QStringLiteral("&#96;");
        case '(':
            return QStringLiteral("&#40;");
        case ')':
            return QStringLiteral("&#41;");
    }
    return c;
}

QString CodeToHtmlConverter::escapeString(StringView s) {
    QString ret = QLatin1String("");
    ret.reserve(s.length());
    for (int i = 0; i < s.length(); ++i) {
        ret += escape(s.at(i));
    }
    return ret;
}

QString CodeToHtmlConverter::setFormat(StringView str, CodeToHtmlConverter::Format format) {
    switch (format) {
        case Type:
            return QStringLiteral("<span class=\"code-type\">") % escapeString(str) %
                   QStringLiteral("</span>");
        case Keyword:
            return QStringLiteral("<span class=\"code-keyword\">") % escapeString(str) %
                   QStringLiteral("</span>");
        case Literal:
            return QStringLiteral("<span class=\"code-literal\">") % escapeString(str) %
                   QStringLiteral("</span>");
        case String:
            return QStringLiteral("<span class=\"code-string\">") % escapeString(str) %
                   QStringLiteral("</span>");
        case Builtin:
            return QStringLiteral("<span class=\"code-builtin\">") % escapeString(str) %
                   QStringLiteral("</span>");
        case Other:
            return QStringLiteral("<span class=\"code-other\">") % escapeString(str) %
                   QStringLiteral("</span>");
        case Comment:
            return QStringLiteral("<span class=\"code-comment\">") % escapeString(str) %
                   QStringLiteral("</span>");
    }

    return str.toString();
}

QString CodeToHtmlConverter::setFormat(const QString &str, CodeToHtmlConverter::Format format) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return setFormat(StringView(str), format);
#else
    return setFormat(StringView(&str), format);
#endif
}
