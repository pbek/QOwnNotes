#pragma once

#include <QTextCursor>

class QPlainTextEdit;

namespace Utils {
namespace MarkdownFormatting {
void applyFormatter(QPlainTextEdit *editor, const QString &formatter,
                    bool multilineInlineHighlighting = true);
QTextCursor fullLineSelectionCursor(const QPlainTextEdit *editor);
void insertCodeBlock(QPlainTextEdit *editor, bool fenced = false);
void insertLink(QPlainTextEdit *editor, const QString &url, const QString &label);
void toggleBlockQuote(QPlainTextEdit *editor);
}    // namespace MarkdownFormatting
}    // namespace Utils
