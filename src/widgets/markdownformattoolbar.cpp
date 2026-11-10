#include "markdownformattoolbar.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QEvent>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QStyle>
#include <QTextDocumentFragment>
#include <QToolButton>
#include <QUrl>

#include "services/settingsservice.h"
#include "utils/listutils.h"
#include "utils/markdownformatting.h"

namespace {
const QString kActionPrefix = QStringLiteral("markdownFormat_");

QIcon themeIcon(const QString &name) {
    return QIcon::fromTheme(
        name, QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/%1.svg").arg(name)));
}

bool looksLikeUrl(const QString &text) {
    static const QRegularExpression urlRegex(
        QStringLiteral(R"(^(https?|ftp|file|mailto|nextcloud):\S+$|^www\.\S+\.\S+$)"),
        QRegularExpression::CaseInsensitiveOption);
    return urlRegex.match(text.trimmed()).hasMatch();
}
}    // namespace

MarkdownFormatToolbar::MarkdownFormatToolbar(QPlainTextEdit *editor, QWidget *parent)
    : QToolBar(tr("Markdown formatting"), parent), _editor(editor) {
    setObjectName(QStringLiteral("markdownFormatToolbar"));
    setMovable(false);
    setFloatable(false);
    setFocusPolicy(Qt::NoFocus);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setContentsMargins(0, 0, 0, 0);
    const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this);
    setIconSize(QSize(iconSize, iconSize));

    const auto inlineFormat = [this](const QString &delimiter) {
        Utils::MarkdownFormatting::applyFormatter(
            _editor, delimiter,
            SettingsService()
                .value(QStringLiteral("Editor/multilineInlineHighlighting"), true)
                .toBool());
    };

    addFormattingAction(QStringLiteral("bold"), tr("Bold"), tr("Format the selected text as bold"),
                        QStringLiteral("format-text-bold"), QKeySequence::Bold,
                        [inlineFormat]() { inlineFormat(QStringLiteral("**")); });
    addFormattingAction(QStringLiteral("italic"), tr("Italic"),
                        tr("Format the selected text as italic"),
                        QStringLiteral("format-text-italic"), QKeySequence::Italic,
                        [inlineFormat]() { inlineFormat(QStringLiteral("*")); });
    addFormattingAction(QStringLiteral("strikeOut"), tr("Strike out"),
                        tr("Strike out the selected text"),
                        QStringLiteral("format-text-strikethrough"), QKeySequence(),
                        [inlineFormat]() { inlineFormat(QStringLiteral("~~")); });
    addSeparator();
    addFormattingAction(QStringLiteral("inlineCode"), tr("Inline code"),
                        tr("Format the selected text as inline code"),
                        QStringLiteral("code-context"), QKeySequence(),
                        [inlineFormat]() { inlineFormat(QStringLiteral("`")); });
    addFormattingAction(QStringLiteral("codeBlock"), tr("Code block"),
                        tr("Insert a code block or put the selected lines into one"),
                        QStringLiteral("code-block"), QKeySequence(),
                        [this]() { Utils::MarkdownFormatting::insertCodeBlock(_editor, true); });
    addFormattingAction(QStringLiteral("blockQuote"), tr("Quote"),
                        tr("Insert a block quote or quote the selected lines"),
                        QStringLiteral("format-text-blockquote"), QKeySequence(),
                        [this]() { Utils::MarkdownFormatting::toggleBlockQuote(_editor); });
    addSeparator();
    addFormattingAction(QStringLiteral("unorderedList"), tr("Bullet list"),
                        tr("Insert a bullet list or turn the selected lines into one"),
                        QStringLiteral("format-list-unordered"), QKeySequence(), [this]() {
                            formatList(Utils::ListUtils::createUnorderedList, QStringLiteral("- "));
                        });
    addFormattingAction(QStringLiteral("orderedList"), tr("Numbered list"),
                        tr("Insert a numbered list or turn the selected lines into one"),
                        QStringLiteral("format-list-ordered"), QKeySequence(), [this]() {
                            formatList(Utils::ListUtils::createOrderedList, QStringLiteral("1. "));
                        });
    addFormattingAction(
        QStringLiteral("checkboxList"), tr("Checklist"),
        tr("Insert a checklist or turn the selected lines into one"),
        QStringLiteral("format-list-checkbox"), QKeySequence(),
        [this]() { formatList(Utils::ListUtils::createCheckboxList, QStringLiteral("- [ ] ")); });
    addSeparator();
    addFormattingAction(QStringLiteral("link"), tr("Link"),
                        tr("Insert a link or link the selected text"),
                        QStringLiteral("text-frame-link"), QKeySequence(QStringLiteral("Ctrl+L")),
                        [this]() { insertLink(); });

    // Keep the help button right-aligned, apart from the formatting actions.
    auto *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer->setFocusPolicy(Qt::NoFocus);
    addWidget(spacer);
    auto *helpAction = addAction(themeIcon(QStringLiteral("help-about")), tr("Markdown help"));
    helpAction->setObjectName(QStringLiteral("markdownFormatHelp"));
    helpAction->setToolTip(tr("Open the Markdown cheat sheet"));
    connect(helpAction, &QAction::triggered, this, []() {
        QDesktopServices::openUrl(
            QUrl(QStringLiteral("https://www.qownnotes.org/getting-started/markdown.html")));
    });
    if (auto *button = qobject_cast<QToolButton *>(widgetForAction(helpAction))) {
        button->setFocusPolicy(Qt::NoFocus);
    }

    _editor->installEventFilter(this);
    updateActionState();
}

QAction *MarkdownFormatToolbar::addFormattingAction(const QString &name, const QString &text,
                                                    const QString &statusTip, const QString &icon,
                                                    const QKeySequence &shortcut,
                                                    const std::function<void()> &callback) {
    auto *action = new QAction(themeIcon(icon), text, this);
    action->setObjectName(kActionPrefix + name);
    action->setShortcut(shortcut);
    action->setShortcutContext(Qt::WidgetShortcut);
    action->setStatusTip(statusTip);
    action->setToolTip(shortcut.isEmpty()
                           ? text
                           : QStringLiteral("%1 <span style=\"color: palette(mid);\">%2</span>")
                                 .arg(text.toHtmlEscaped(),
                                      shortcut.toString(QKeySequence::NativeText).toHtmlEscaped()));
    // Register shortcuts on the editor only, not on the dialog or main window.
    _editor->addAction(action);
    addAction(action);
    if (auto *button = qobject_cast<QToolButton *>(widgetForAction(action))) {
        button->setFocusPolicy(Qt::NoFocus);
    }
    connect(action, &QAction::triggered, this, [this, callback]() {
        if (!_editor->isReadOnly() && _editor->isEnabled()) {
            callback();
            _editor->setFocus();
        }
    });
    return action;
}

QList<QAction *> MarkdownFormatToolbar::formattingActions(const QPlainTextEdit *editor) {
    QList<QAction *> result;
    for (auto *action : editor->actions()) {
        if (action->objectName().startsWith(kActionPrefix)) {
            result.append(action);
        }
    }
    return result;
}

void MarkdownFormatToolbar::addFormattingMenu(QMenu *menu, const QPlainTextEdit *editor) {
    menu->addSeparator();
    auto *formatMenu = menu->addMenu(themeIcon(QStringLiteral("format-text-bold")), tr("Format"));
    formatMenu->setObjectName(QStringLiteral("markdownFormatMenu"));
    QString previousGroup;
    for (auto *action : formattingActions(editor)) {
        // Mirror the toolbar groups in the context menu.
        const QString name = action->objectName().mid(kActionPrefix.size());
        const QString group =
            (name == QLatin1String("bold") || name == QLatin1String("italic") ||
             name == QLatin1String("strikeOut"))
                ? QStringLiteral("text")
            : (name == QLatin1String("inlineCode") || name == QLatin1String("codeBlock") ||
               name == QLatin1String("blockQuote"))
                ? QStringLiteral("block")
            : name.endsWith(QLatin1String("List")) ? QStringLiteral("list")
                                                   : QStringLiteral("link");
        if (!previousGroup.isEmpty() && group != previousGroup) {
            formatMenu->addSeparator();
        }
        previousGroup = group;
        formatMenu->addAction(action);
    }
    formatMenu->setEnabled(editor->isEnabled() && !editor->isReadOnly());
}

bool MarkdownFormatToolbar::eventFilter(QObject *object, QEvent *event) {
    if (object == _editor &&
        (event->type() == QEvent::ShortcutOverride || event->type() == QEvent::KeyPress)) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        const QKeySequence sequence(static_cast<int>(keyEvent->modifiers()) | keyEvent->key());
        for (auto *action : formattingActions(_editor)) {
            if (!action->shortcut().isEmpty() && action->shortcut() == sequence) {
                // Prefer editor formatting over dialog shortcuts, such as Tasks' Ctrl+I.
                event->accept();
                if (event->type() == QEvent::KeyPress && action->isEnabled()) {
                    action->trigger();
                }
                return true;
            }
        }
    }

    if (object == _editor &&
        (event->type() == QEvent::ReadOnlyChange || event->type() == QEvent::EnabledChange)) {
        updateActionState();
    }
    return QToolBar::eventFilter(object, event);
}

void MarkdownFormatToolbar::updateActionState() {
    const bool enabled = _editor->isEnabled() && !_editor->isReadOnly();
    for (auto *action : formattingActions(_editor)) {
        action->setEnabled(enabled);
    }
}

void MarkdownFormatToolbar::formatList(const std::function<QString(const QString &)> &formatter,
                                       const QString &emptyLinePrefix) {
    QTextCursor cursor = Utils::MarkdownFormatting::fullLineSelectionCursor(_editor);
    const int start = cursor.selectionStart();
    const bool emptyLine = cursor.selectedText().trimmed().isEmpty();
    const QString text =
        emptyLine ? cursor.selectedText() + emptyLinePrefix : formatter(cursor.selectedText());
    cursor.beginEditBlock();
    cursor.insertText(text);
    if (!emptyLine) {
        cursor.setPosition(start);
        cursor.setPosition(start + text.size(), QTextCursor::KeepAnchor);
    }
    cursor.endEditBlock();
    _editor->setTextCursor(cursor);
}

void MarkdownFormatToolbar::insertLink() {
    const QString selectedText = _editor->textCursor().selection().toPlainText();
    const bool selectionIsUrl = looksLikeUrl(selectedText);

    // Suggest a URL from the selection or the clipboard to save typing.
    QString suggestedUrl;
    const QString clipboardText = QApplication::clipboard()->text().trimmed();
    if (selectionIsUrl) {
        suggestedUrl = selectedText.trimmed();
    } else if (looksLikeUrl(clipboardText)) {
        suggestedUrl = clipboardText;
    }

    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("Insert link"));
    dialog.setLabelText(selectedText.isEmpty() || selectionIsUrl
                            ? tr("URL:")
                            : tr("URL for \"%1\":").arg(selectedText.left(60)));
    dialog.setTextValue(suggestedUrl);
    dialog.setOkButtonText(tr("Insert"));
    dialog.resize(qMax(dialog.sizeHint().width(), 420), dialog.sizeHint().height());
    if (auto *lineEdit = dialog.findChild<QLineEdit *>()) {
        lineEdit->setPlaceholderText(QStringLiteral("https://"));
        lineEdit->selectAll();
    }
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const QString url = dialog.textValue().trimmed();
    QString label = selectedText;
    if (selectedText.isEmpty() || selectionIsUrl) {
        label = selectionIsUrl ? selectedText.trimmed() : tr("link text");
    }
    Utils::MarkdownFormatting::insertLink(_editor, url, label);
}
