#pragma once

#include <QToolBar>
#include <functional>

class QPlainTextEdit;
class QMenu;

class MarkdownFormatToolbar : public QToolBar {
    Q_OBJECT

   public:
    explicit MarkdownFormatToolbar(QPlainTextEdit *editor, QWidget *parent = nullptr);
    static QList<QAction *> formattingActions(const QPlainTextEdit *editor);
    static void addFormattingMenu(QMenu *menu, const QPlainTextEdit *editor);

   protected:
    bool eventFilter(QObject *object, QEvent *event) override;

   private:
    QAction *addFormattingAction(const QString &name, const QString &text, const QString &statusTip,
                                 const QString &icon, const QKeySequence &shortcut,
                                 const std::function<void()> &callback);
    void updateActionState();
    void formatList(const std::function<QString(const QString &)> &formatter,
                    const QString &emptyLinePrefix);
    void insertLink();
    QPlainTextEdit *_editor;
};
