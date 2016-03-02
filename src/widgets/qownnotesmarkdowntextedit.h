#ifndef QOWNNOTESMARKDOWNTEXTEDIT_H
#define QOWNNOTESMARKDOWNTEXTEDIT_H

#include <QObject>
#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"

class QOwnNotesMarkdownTextEdit : public QMarkdownTextEdit {
Q_OBJECT

public:
    enum EditorWidthMode {
        Narrow = 1,
        Medium,
        Wide,
        Full
    };
    Q_ENUMS(EditorWidthMode)

    explicit QOwnNotesMarkdownTextEdit(QWidget *parent = 0);

    void setStyles();
    void openUrl(QUrl url);
//    void setViewportMargins(int left, int top, int right, int bottom);
    void setPaperMargins(int width);
};

#endif // QOWNNOTESMARKDOWNTEXTEDIT_H
