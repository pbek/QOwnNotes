#include <helpers/fakevimproxy.h>
#include <libraries/fakevim/fakevim/fakevimactions.h>
#include <libraries/fakevim/fakevim/fakevimhandler.h>
#include <mainwindow.h>

#include <QDir>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTextStream>
#include <QtGui/QTextBlock>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStatusBar>

FakeVimProxy::FakeVimProxy(QWidget *widget, QObject *parent) : QObject(parent), m_widget(widget) {
    using namespace FakeVim::Internal;

    auto *handler = static_cast<FakeVim::Internal::FakeVimHandler *>(parent);

    handler->installEventFilter();
    handler->setupWidget();

    const auto homeDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    const QFile vimRcQONFile(QDir(homeDir).filePath(".vimrc.qownnotes"));

    // Try to load .vimrc.qownnotes or .vimrc files
    if (vimRcQONFile.exists()) {
        handler->handleCommand(QStringLiteral("source ") + vimRcQONFile.fileName());
    } else {
        const QFile vimRcFile(QDir(homeDir).filePath(".vimrc"));
        if (vimRcFile.exists()) {
            handler->handleCommand(QStringLiteral("source ") + vimRcFile.fileName());
        }
    }

    QSettings settings;
    bool setExpandTab = !settings.value(QStringLiteral("Editor/useTabIndent")).toBool();
    fakeVimSettings()->item("et")->setValue(setExpandTab);
    fakeVimSettings()->item("ts")->setValue(Utils::Misc::indentSize());
    fakeVimSettings()->item("sw")->setValue(Utils::Misc::indentSize());

    {
        auto h = [this](const QString &contents, int cursorPos, int anchorPos, int msgLvl) {
            changeStatusMessage(contents, cursorPos, anchorPos, msgLvl);
        };
        handler->commandBufferChanged.connect(h);
    }

    {
        auto h = [this](const QString &msg) { changeExtraInformation(msg); };
        handler->extraInformationChanged.connect(h);
    }

    {
        auto h = [this](const QString &msg) { changeStatusData(msg); };
        handler->statusDataChanged.connect(h);
    }

    {
        auto h = [this](const QString &msg) { highlightMatches(msg); };
        handler->highlightMatches.connect(h);
    }

    {
        auto h = [this](bool *handled, const ExCommand &cmd) { handleExCommand(handled, cmd); };
        handler->handleExCommandRequested.connect(h);
    }

    {
        auto h = [this](const QTextCursor &cursor) { requestSetBlockSelection(cursor); };
        handler->requestSetBlockSelection.connect(h);
    }

    {
        auto h = [this]() { requestDisableBlockSelection(); };
        handler->requestDisableBlockSelection.connect(h);
    }

    {
        auto h = [this](bool *on) { requestHasBlockSelection(on); };
        handler->requestHasBlockSelection.connect(h);
    }

    {
        auto h = [this](int beginLine, int endLine, QChar typedChar) {
            indentRegion(beginLine, endLine, typedChar);
        };
        handler->indentRegion.connect(h);
    }

    {
        auto h = [this](bool *result, QChar c) { checkForElectricCharacter(result, c); };
        handler->checkForElectricCharacter.connect(h);
    }

    // regular signal
    connect(this, &FakeVimProxy::handleInput, handler,
            [handler](const QString &text) { handler->handleInput(text); });
}

void FakeVimProxy::changeStatusData(const QString &info) {
    m_statusData = info;
    updateStatusBar();
}

void FakeVimProxy::changeExtraInformation(const QString &info) {
    QMessageBox::information(m_widget, tr("Information"), info);
}

void FakeVimProxy::highlightMatches(const QString &pattern) {
    QPlainTextEdit *ed = qobject_cast<QPlainTextEdit *>(m_widget);
    if (!ed) return;

    QTextCursor cur = ed->textCursor();

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(Qt::yellow);
    selection.format.setForeground(Qt::black);

    // Highlight matches.
    QTextDocument *doc = ed->document();
    QRegularExpression re(pattern);
    cur = doc->find(re);

    m_searchSelection.clear();

    int a = cur.position();
    while (!cur.isNull()) {
        if (cur.hasSelection()) {
            selection.cursor = cur;
            m_searchSelection.append(selection);
        } else {
            cur.movePosition(QTextCursor::NextCharacter);
        }
        cur = doc->find(re, cur);
        int b = cur.position();
        if (a == b) {
            cur.movePosition(QTextCursor::NextCharacter);
            cur = doc->find(re, cur);
            b = cur.position();
            if (a == b) break;
        }
        a = b;
    }

    updateExtraSelections();
}

void FakeVimProxy::changeStatusMessage(const QString &contents, int cursorPos, int /*anchorPos*/,
                                       int /*messageLevel*/) {
    m_statusMessage = cursorPos == -1
                          ? contents
                          : contents.left(cursorPos) + QChar(10073) + contents.mid(cursorPos);
    updateStatusBar();
}

void FakeVimProxy::updateStatusBar() {
    int slack = 80 - m_statusMessage.size() - m_statusData.size();
    QString msg = m_statusMessage + QString(slack, QLatin1Char(' ')) + m_statusData;
    MainWindow::instance()->statusBar()->showMessage(msg);
}

void FakeVimProxy::handleExCommand(bool *handled, const FakeVim::Internal::ExCommand &cmd) {
    if (wantSaveAndQuit(cmd)) {
        // :wq
        if (save()) cancel();
    } else if (wantSave(cmd)) {
        save();    // :w
    } else if (wantQuit(cmd)) {
        if (cmd.hasBang)
            invalidate();    // :q!
        else
            cancel();    // :q
    } else {
        *handled = false;
        return;
    }

    *handled = true;
}

void FakeVimProxy::requestSetBlockSelection(const QTextCursor &tc) {
    auto *ed = qobject_cast<QPlainTextEdit *>(m_widget);
    if (!ed) return;

    QPalette pal =
        ed->parentWidget() != nullptr ? ed->parentWidget()->palette() : QApplication::palette();

    m_blockSelection.clear();
    m_clearSelection.clear();

    QTextCursor cur = tc;

    QTextEdit::ExtraSelection selection = QTextEdit::ExtraSelection();
    selection.format.setBackground(pal.color(QPalette::Base));
    selection.format.setForeground(pal.color(QPalette::Text));
    selection.cursor = cur;
    m_clearSelection.append(selection);

    selection.format.setBackground(pal.color(QPalette::Highlight));
    selection.format.setForeground(pal.color(QPalette::HighlightedText));

    int from = cur.positionInBlock();
    int to = cur.anchor() - cur.document()->findBlock(cur.anchor()).position();
    const int min = qMin<int>(cur.position(), cur.anchor());
    const int max = qMax<int>(cur.position(), cur.anchor());
    for (QTextBlock block = cur.document()->findBlock(min);
         block.isValid() && block.position() < max; block = block.next()) {
        cur.setPosition(block.position() + qMin<int>(from, block.length()));
        cur.setPosition(block.position() + qMin<int>(to, block.length()), QTextCursor::KeepAnchor);
        selection.cursor = cur;
        m_blockSelection.append(selection);
    }

    disconnect(ed, &QPlainTextEdit::selectionChanged, this, &FakeVimProxy::updateBlockSelection);
    ed->setTextCursor(tc);
    connect(ed, &QPlainTextEdit::selectionChanged, this, &FakeVimProxy::updateBlockSelection);

    QPalette pal2 = ed->palette();
    pal2.setColor(QPalette::Highlight, Qt::transparent);
    pal2.setColor(QPalette::HighlightedText, Qt::transparent);
    ed->setPalette(pal2);

    updateExtraSelections();
}

void FakeVimProxy::requestDisableBlockSelection() {
    auto *ed = qobject_cast<QPlainTextEdit *>(m_widget);
    if (!ed) return;

    QPalette pal =
        ed->parentWidget() != nullptr ? ed->parentWidget()->palette() : QApplication::palette();

    m_blockSelection.clear();
    m_clearSelection.clear();

    ed->setPalette(pal);

    disconnect(ed, &QPlainTextEdit::selectionChanged, this, &FakeVimProxy::updateBlockSelection);

    updateExtraSelections();
}

void FakeVimProxy::updateBlockSelection() {
    auto *ed = qobject_cast<QPlainTextEdit *>(m_widget);
    if (!ed) return;

    requestSetBlockSelection(ed->textCursor());
}

void FakeVimProxy::requestHasBlockSelection(bool *on) { *on = !m_blockSelection.isEmpty(); }

void FakeVimProxy::indentRegion(int beginBlock, int endBlock, QChar typedChar) {
    auto *ed = qobject_cast<QPlainTextEdit *>(m_widget);
    if (!ed) return;

    const qint64 indentSize = FakeVim::Internal::fakeVimSettings()->shiftWidth.value();

    QTextDocument *doc = ed->document();
    QTextBlock startBlock = doc->findBlockByNumber(beginBlock);

    // Record line lengths for mark adjustments
    QVector<int> lineLengths(endBlock - beginBlock + 1);
    QTextBlock block = startBlock;

    for (int i = beginBlock; i <= endBlock; ++i) {
        const auto line = block.text();
        lineLengths[i - beginBlock] = line.length();
        if (typedChar.unicode() == 0 && line.simplified().isEmpty()) {
            // clear empty lines
            QTextCursor cursor(block);
            while (!cursor.atBlockEnd()) cursor.deleteChar();
        } else {
            const auto previousBlock = block.previous();
            const auto previousLine = previousBlock.isValid() ? previousBlock.text() : QString();

            qint64 indent = firstNonSpace(previousLine);
            if (typedChar == '}')
                indent = std::max<int>(0, int(indent - indentSize));
            else if (previousLine.endsWith(QLatin1String("{")))
                indent += indentSize;
            const auto indentString = QStringLiteral(" ").repeated(indent);

            QTextCursor cursor(block);
            cursor.beginEditBlock();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                                firstNonSpace(line));
            cursor.removeSelectedText();
            cursor.insertText(indentString);
            cursor.endEditBlock();
        }
        block = block.next();
    }
}

void FakeVimProxy::checkForElectricCharacter(bool *result, QChar c) {
    *result = c == '{' || c == '}';
}

int FakeVimProxy::firstNonSpace(const QString &text) {
    int indent = 0;
    while (indent < text.length() && text.at(indent) == ' ') ++indent;
    return indent;
}

void FakeVimProxy::updateExtraSelections() {
    auto *ed = qobject_cast<QPlainTextEdit *>(m_widget);
    if (ed) ed->setExtraSelections(m_clearSelection + m_searchSelection + m_blockSelection);
}

bool FakeVimProxy::wantSaveAndQuit(const FakeVim::Internal::ExCommand &cmd) {
    return cmd.cmd == QLatin1String("wq");
}

bool FakeVimProxy::wantSave(const FakeVim::Internal::ExCommand &cmd) {
    return cmd.matches(QStringLiteral("w"), QStringLiteral("write")) ||
           cmd.matches(QStringLiteral("wa"), QStringLiteral("wall"));
}

bool FakeVimProxy::wantQuit(const FakeVim::Internal::ExCommand &cmd) {
    return cmd.matches(QStringLiteral("q"), QStringLiteral("quit")) ||
           cmd.matches(QStringLiteral("qa"), QStringLiteral("qall"));
}

void FakeVimProxy::cancel() { invalidate(); }

bool FakeVimProxy::save() {
    MainWindow::instance()->storeUpdatedNotesToDisk();
    return true;
}

void FakeVimProxy::invalidate() { QApplication::quit(); }

bool FakeVimProxy::hasChanges() { return false; }

QTextDocument *FakeVimProxy::document() const {
    QTextDocument *doc = nullptr;
    if (auto *ed = qobject_cast<QPlainTextEdit *>(m_widget))
        doc = ed->document();
    else if (auto *ed = qobject_cast<QPlainTextEdit *>(m_widget))
        doc = ed->document();
    return doc;
}

QString FakeVimProxy::content() const { return document()->toPlainText(); }
