#ifndef QOWNNOTESMARKDOWNTEXTEDIT_H
#define QOWNNOTESMARKDOWNTEXTEDIT_H

#include <QObject>
#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"
#include "mainwindow.h"

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

    enum FontModificationMode {
        Increase = 1,
        Decrease,
        Reset
    };
    Q_ENUMS(FontModificationMode)

    explicit QOwnNotesMarkdownTextEdit(QWidget *parent = 0);

    void setStyles();
    void openUrl(QString urlString);
//    void setViewportMargins(int left, int top, int right, int bottom);
    void setPaperMargins(int width = -1);
    void setMainWindow(MainWindow *mainWindow);
    int modifyFontSize(FontModificationMode mode);
    void updateSettings();

protected:
    void insertFromMimeData(const QMimeData * source);
    void resizeEvent(QResizeEvent* event);

private slots:
    void highlightCurrentLine();

private:
    MainWindow *mainWindow;

    void setFormatStyle(MarkdownHighlighter::HighlighterState index);

Q_SIGNALS:
    void resize(QResizeEvent* event);
};

#endif // QOWNNOTESMARKDOWNTEXTEDIT_H
