#include "markdownformatting.h"

#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QStringList>
#include <QTextDocumentFragment>

void Utils::MarkdownFormatting::applyFormatter(QPlainTextEdit *editor, const QString &formatter,
                                               bool multilineInlineHighlighting) {
    if (editor->isReadOnly() || formatter.isEmpty()) {
        return;
    }

    QTextCursor cursor = editor->textCursor();
    QString selectedText = cursor.selectedText();
    const int start = cursor.selectionStart();
    const int end = cursor.selectionEnd();
    const int length = formatter.size();

    // Only inspect surrounding delimiters when they are inside the document.
    if (start >= length && end + length < editor->document()->characterCount()) {
        QTextCursor surrounding = cursor;
        surrounding.setPosition(start - length);
        surrounding.setPosition(end + length, QTextCursor::KeepAnchor);
        const QString text = surrounding.selectedText();
        if (text.startsWith(formatter) && text.endsWith(formatter)) {
            surrounding.beginEditBlock();
            surrounding.insertText(selectedText);
            surrounding.setPosition(start - length);
            surrounding.setPosition(start - length + selectedText.size(), QTextCursor::KeepAnchor);
            surrounding.endEditBlock();
            editor->setTextCursor(surrounding);
            return;
        }
    }

    const bool endsWithLineBreak = selectedText.endsWith(QChar::ParagraphSeparator);
    if (endsWithLineBreak) {
        selectedText.chop(1);
    }

    if (selectedText.isEmpty()) {
        cursor.insertText(formatter.repeated(2));
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, length);
        editor->setTextCursor(cursor);
        return;
    }

    const auto options = multilineInlineHighlighting
                             ? QRegularExpression::DotMatchesEverythingOption
                             : QRegularExpression::NoPatternOption;
    const auto match =
        QRegularExpression(QStringLiteral(R"(^(\s*)(.+?)(\s*)$)"), options).match(selectedText);
    if (!match.hasMatch()) {
        return;
    }

    QString formatted =
        match.captured(1) + formatter + match.captured(2) + formatter + match.captured(3);
    if (endsWithLineBreak) {
        formatted += QChar::ParagraphSeparator;
    }
    cursor.beginEditBlock();
    cursor.insertText(formatted);
    cursor.setPosition(start + match.captured(1).size() + length);
    cursor.setPosition(cursor.position() + match.captured(2).size(), QTextCursor::KeepAnchor);
    cursor.endEditBlock();
    editor->setTextCursor(cursor);
}

QTextCursor Utils::MarkdownFormatting::fullLineSelectionCursor(const QPlainTextEdit *editor) {
    QTextCursor cursor = editor->textCursor();
    if (!cursor.hasSelection()) {
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        return cursor;
    }

    const int start = cursor.selectionStart();
    const int end = cursor.selectionEnd();
    QTextCursor lineCursor(editor->document());
    lineCursor.setPosition(start);
    lineCursor.movePosition(QTextCursor::StartOfBlock);
    QTextCursor endCursor(editor->document());
    endCursor.setPosition(end);
    // A selection ending at the next line's start does not include that line.
    if (end > start && endCursor.atBlockStart()) {
        endCursor.movePosition(QTextCursor::PreviousCharacter);
    }
    endCursor.movePosition(QTextCursor::EndOfBlock);
    lineCursor.setPosition(endCursor.position(), QTextCursor::KeepAnchor);
    return lineCursor;
}

void Utils::MarkdownFormatting::insertCodeBlock(QPlainTextEdit *editor, bool fenced) {
    if (editor->isReadOnly()) {
        return;
    }
    QTextCursor cursor = editor->textCursor();
    QString selectedText = cursor.selection().toPlainText();
    cursor.beginEditBlock();
    if (fenced) {
        QTextCursor startCursor = cursor;
        startCursor.setPosition(cursor.selectionStart());
        QTextCursor endCursor = cursor;
        endCursor.setPosition(cursor.selectionEnd());
        const QString prefix = startCursor.atBlockStart() ? QString() : QStringLiteral("\n");
        const QString suffix = endCursor.atBlockEnd() ? QString() : QStringLiteral("\n");
        const int contentStart = cursor.selectionStart() + prefix.size() + 4;
        if (!selectedText.endsWith(QLatin1Char('\n'))) {
            selectedText += QLatin1Char('\n');
        }
        cursor.insertText(prefix + QStringLiteral("```\n") + selectedText + QStringLiteral("```") +
                          suffix);
        cursor.setPosition(contentStart);
        cursor.endEditBlock();
        editor->setTextCursor(cursor);
        return;
    }
    if (selectedText.isEmpty()) {
        if (cursor.atBlockStart() && cursor.atBlockEnd()) {
            cursor.insertText(QStringLiteral("```\n\n```"));
            cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
        } else {
            cursor.insertText(QStringLiteral("``"));
        }
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
    } else {
        const bool multiline = selectedText.contains(QLatin1Char('\n'));
        if (multiline) {
            const QString endNewline =
                selectedText.endsWith(QLatin1Char('\n')) ? QString() : QStringLiteral("\n");
            selectedText =
                QStringLiteral("``\n") + selectedText + endNewline + QStringLiteral("``");
        }
        cursor.insertText(QStringLiteral("`") + selectedText + QStringLiteral("`"));
        if (multiline) {
            cursor.insertText(QStringLiteral("\n"));
        }
    }
    cursor.endEditBlock();
    editor->setTextCursor(cursor);
}

void Utils::MarkdownFormatting::insertLink(QPlainTextEdit *editor, const QString &url,
                                           const QString &label) {
    if (editor->isReadOnly() || url.trimmed().isEmpty()) {
        return;
    }
    QTextCursor cursor = editor->textCursor();
    const int start = cursor.selectionStart();
    // Escape syntax characters so labels and URLs cannot break the Markdown link.
    QString escapedLabel = label;
    escapedLabel.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    escapedLabel.replace(QStringLiteral("["), QStringLiteral("\\["));
    escapedLabel.replace(QStringLiteral("]"), QStringLiteral("\\]"));
    QString escapedUrl = url.trimmed();
    escapedUrl.replace(QStringLiteral(" "), QStringLiteral("%20"));
    escapedUrl.replace(QStringLiteral("("), QStringLiteral("%28"));
    escapedUrl.replace(QStringLiteral(")"), QStringLiteral("%29"));
    cursor.beginEditBlock();
    cursor.insertText(QStringLiteral("[%1](%2)").arg(escapedLabel, escapedUrl));
    cursor.setPosition(start + 1);
    cursor.setPosition(start + 1 + escapedLabel.size(), QTextCursor::KeepAnchor);
    cursor.endEditBlock();
    editor->setTextCursor(cursor);
}

void Utils::MarkdownFormatting::toggleBlockQuote(QPlainTextEdit *editor) {
    if (editor->isReadOnly()) {
        return;
    }
    QTextCursor cursor = fullLineSelectionCursor(editor);
    const int start = cursor.selectionStart();
    QString selectedText = cursor.selectedText();
    selectedText.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
    QStringList lines = selectedText.split(QLatin1Char('\n'));

    // Remove quoting only if every non-empty line is already quoted
    bool hasContent = false;
    bool allQuoted = true;
    const QStringList &constLines = lines;
    for (const QString &line : constLines) {
        if (!line.trimmed().isEmpty()) {
            hasContent = true;
            allQuoted = allQuoted && line.startsWith(QLatin1Char('>'));
        }
    }
    allQuoted = allQuoted && hasContent;

    for (QString &line : lines) {
        if (allQuoted) {
            if (line.startsWith(QLatin1Char('>'))) {
                line.remove(0, line.startsWith(QLatin1String("> ")) ? 2 : 1);
            }
        } else if (!hasContent || !line.trimmed().isEmpty()) {
            line.prepend(QStringLiteral("> "));
        }
    }

    const QString text = lines.join(QLatin1Char('\n'));
    cursor.beginEditBlock();
    cursor.insertText(text);
    if (hasContent) {
        cursor.setPosition(start);
        cursor.setPosition(start + text.size(), QTextCursor::KeepAnchor);
    }
    cursor.endEditBlock();
    editor->setTextCursor(cursor);
}
