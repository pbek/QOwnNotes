#ifndef QMARKDOWNTEXTEDIT_H
#define QMARKDOWNTEXTEDIT_H

#include <QWidget>
#include <QTextEdit>
#include <QEvent>
#include <markdown-highlight/highlighter.h>

class QMarkdownTextEdit : public QTextEdit
{
//    Q_OBJECT

public:
    QMarkdownTextEdit(QWidget *parent = 0);
    void anchorClicked(const QUrl &url);

protected:
    HGMarkdownHighlighter *highlighter;
    bool eventFilter(QObject *obj, QEvent *event);
    bool increaseSelectedTextIndention(bool reverse);
    void openLinkAtCursorPosition();
};

#endif // QMARKDOWNTEXTEDIT_H
