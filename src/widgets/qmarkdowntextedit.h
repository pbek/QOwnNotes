#ifndef QMARKDOWNTEXTEDIT_H
#define QMARKDOWNTEXTEDIT_H

#include <QWidget>
#include <QTextEdit>
#include <markdown-highlight/highlighter.h>

class QMarkdownTextEdit : public QTextEdit
{

public:
    QMarkdownTextEdit(QWidget *parent = 0);

protected:
    HGMarkdownHighlighter *highlighter;
};

#endif // QMARKDOWNTEXTEDIT_H
