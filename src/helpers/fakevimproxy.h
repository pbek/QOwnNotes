
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtGui/QTextBlock>
#include <QtWidgets/QMessageBox>
#include <libraries/fakevim/fakevim/fakevimhandler.h>
#include <mainwindow.h>

class FakeVimProxy : public QObject
{
    Q_OBJECT

public:
    FakeVimProxy(QWidget *widget, MainWindow *mw, QObject *parent = 0);

    signals:
            void handleInput(const QString &keys);

public slots:
    void changeStatusData(const QString &info);

    void highlightMatches(const QString &pattern);

    void changeStatusMessage(const QString &contents, int cursorPos);

    void changeExtraInformation(const QString &info);

    void updateStatusBar();

    void handleExCommand(bool *handled, const FakeVim::Internal::ExCommand &cmd);

    void requestSetBlockSelection(const QTextCursor &tc);

    void requestDisableBlockSelection();

    void updateBlockSelection();

    void requestHasBlockSelection(bool *on);

    void indentRegion(int beginBlock, int endBlock, QChar typedChar);

    void checkForElectricCharacter(bool *result, QChar c);

private:
    static int firstNonSpace(const QString &text);

    void updateExtraSelections();

    bool wantSaveAndQuit(const FakeVim::Internal::ExCommand &cmd);

    bool wantSave(const FakeVim::Internal::ExCommand &cmd);

    bool wantQuit(const FakeVim::Internal::ExCommand &cmd);

    bool save();

    void cancel();

    void invalidate();

    bool hasChanges();

    QTextDocument *document() const;

    QString content() const;

    QWidget *m_widget;
    MainWindow *m_mainWindow;
    QString m_statusMessage;
    QString m_statusData;

    QList<QTextEdit::ExtraSelection> m_searchSelection;
    QList<QTextEdit::ExtraSelection> m_clearSelection;
    QList<QTextEdit::ExtraSelection> m_blockSelection;
    typedef QLatin1String _;
};
