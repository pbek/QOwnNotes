#ifndef QOWNNOTESMARKDOWNTEXTEDIT_H
#define QOWNNOTESMARKDOWNTEXTEDIT_H

#include <QObject>
#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"

class QOwnNotesMarkdownTextEdit : public QMarkdownTextEdit
{
    Q_OBJECT

public:
    QOwnNotesMarkdownTextEdit(QWidget *parent = 0);
    void openUrl(QUrl url);
};

#endif // QOWNNOTESMARKDOWNTEXTEDIT_H
