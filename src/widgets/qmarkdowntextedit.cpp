#include "qmarkdowntextedit.h"

QMarkdownTextEdit::QMarkdownTextEdit(QWidget *parent)
 : QTextEdit(parent)
{
    // setup the markdown highlighting
    highlighter = new HGMarkdownHighlighter( document(), 1000 );
}
