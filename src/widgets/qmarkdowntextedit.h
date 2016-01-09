#ifndef QMARKDOWNTEXTEDIT_H
#define QMARKDOWNTEXTEDIT_H

#include <QTextEdit>
#include <QEvent>
#include <markdown-highlight/highlighter.h>

class QMarkdownTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    QMarkdownTextEdit(QWidget *parent = 0);
    HGMarkdownHighlighter *highlighter();
    void setIgnoredClickUrlSchemata(QStringList ignoredUrlSchemata);
    static void openUrl(QUrl url);

protected:
    HGMarkdownHighlighter *_highlighter;
    QStringList _ignoredClickUrlSchemata;
    bool eventFilter(QObject *obj, QEvent *event);
    bool increaseSelectedTextIndention(bool reverse);
    void openLinkAtCursorPosition();

signals:
    void urlClicked(QUrl url);
};

#endif // QMARKDOWNTEXTEDIT_H
