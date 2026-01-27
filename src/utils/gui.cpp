/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "gui.h"

#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <libraries/qmarkdowntextedit/markdownhighlighter.h>

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QStyleFactory>
#include <QTextBlock>
#include <QTextCursor>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "helpers/nomenuiconstyle.h"
#include "services/settingsservice.h"

#define ORDER_ASCENDING 0     // Qt::AscendingOrder // = 0
#define ORDER_DESCENDING 1    // Qt::DescendingOrder // = 1

Qt::SortOrder Utils::Gui::toQtOrder(int order) {
    return order == ORDER_ASCENDING ? Qt::AscendingOrder : Qt::DescendingOrder;
}

/**
 * Checks if there is at least one child that is visible
 */
bool Utils::Gui::isOneTreeWidgetItemChildVisible(QTreeWidgetItem *item) {
    for (int i = 0; i < item->childCount(); i++) {
        QTreeWidgetItem *child = item->child(i);
        if (!child->isHidden() || isOneTreeWidgetItemChildVisible(child)) {
            return true;
        }
    }

    return false;
}

/**
 * Searches for text in items of a tree widget
 */
void Utils::Gui::searchForTextInTreeWidget(QTreeWidget *treeWidget, const QString &text,
                                           TreeWidgetSearchFlags searchFlags) {
    QStringList searchList;

    if (searchFlags & TreeWidgetSearchFlag::EveryWordSearch) {
        static const QRegularExpression re(QStringLiteral("\\s+"));
        searchList = text.split(re);
    } else {
        searchList << text;
    }

    // get all items
    QList<QTreeWidgetItem *> allItems =
        treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (text.size() >= 1) {
        int searchColumnCount =
            searchFlags & TreeWidgetSearchFlag::AllColumnsSearch ? treeWidget->columnCount() : 1;

        // hide all not found items
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            bool show = false;

            // look in all columns that we want to search
            for (int index = 0; index < searchColumnCount; index++) {
                bool loopShow = true;

                foreach (QString searchText, searchList) {
                    // search for text in the columns
                    bool loopShow2 = item->text(index).contains(searchText, Qt::CaseInsensitive);

                    // also show the item if the text was found in the tooltip
                    if (searchFlags & TreeWidgetSearchFlag::TooltipSearch) {
                        loopShow2 |= item->toolTip(index).contains(searchText, Qt::CaseInsensitive);
                    }

                    loopShow &= loopShow2;
                }

                show |= loopShow;
            }

            // also show the item if an integer id is greater than 0
            if (searchFlags & TreeWidgetSearchFlag::IntCheck) {
                int id = item->data(0, Qt::UserRole).toInt();
                show |= id <= 0;
            }

            item->setHidden(!show);
        }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            if (isOneTreeWidgetItemChildVisible(item)) {
                item->setHidden(false);
                item->setExpanded(true);
            }
        }
    } else {
        // show all items otherwise
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            item->setHidden(false);
        }
    }
}

/**
 * Searches for text in items of a list widget
 */
void Utils::Gui::searchForTextInListWidget(QListWidget *listWidget, const QString &text,
                                           bool searchAddProps) {
    QList<QListWidgetItem *> allItems =
        listWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (text.size() >= 1) {
        // hide all not found items
        Q_FOREACH (QListWidgetItem *item, allItems) {
            bool show = item->text().contains(text, Qt::CaseInsensitive);

            if (searchAddProps) {
                show |= item->toolTip().contains(text, Qt::CaseInsensitive) ||
                        item->whatsThis().contains(text, Qt::CaseInsensitive);
            }

            item->setHidden(!show);
        }
    } else {
        // show all items otherwise
        Q_FOREACH (QListWidgetItem *item, allItems) {
            item->setHidden(false);
        }
    }
}

/**
 * Checks if a variant exists as user data in a tree widget
 */
bool Utils::Gui::userDataInTreeWidgetExists(QTreeWidget *treeWidget, const QVariant &userData,
                                            int column) {
    return getTreeWidgetItemWithUserData(treeWidget, userData, column) != nullptr;
}

/**
 * Returns the tree widget item of a tree widget with that has a certain user
 * data
 *
 * @param treeWidget
 * @param userData
 * @param column
 * @return
 */
QTreeWidgetItem *Utils::Gui::getTreeWidgetItemWithUserData(QTreeWidget *treeWidget,
                                                           const QVariant &userData, int column) {
    // get all items
    QList<QTreeWidgetItem *> allItems =
        treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    Q_FOREACH (QTreeWidgetItem *item, allItems) {
        if (userData == item->data(column, Qt::UserRole)) {
            return item;
        }
    }

    return nullptr;
}

QListWidgetItem *Utils::Gui::getListWidgetItemWithUserData(QListWidget *listWidget,
                                                           const QVariant &userData) {
    // get all items
    QList<QListWidgetItem *> allItems = listWidget->findItems("", Qt::MatchContains);

    Q_FOREACH (QListWidgetItem *item, allItems) {
        if (userData == item->data(Qt::UserRole)) {
            return item;
        }
    }

    return nullptr;
}

/**
 * Resets the bold state of all tree widget items of a tree widget
 *
 * @param treeWidget
 * @param column
 */
void Utils::Gui::resetBoldStateOfAllTreeWidgetItems(QTreeWidget *treeWidget, int column) {
    // get all items
    QList<QTreeWidgetItem *> allItems =
        treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    Q_FOREACH (QTreeWidgetItem *item, allItems) {
        auto font = item->font(column);
        if (font.bold()) {
            font.setBold(false);
            item->setFont(column, font);
        }
    }
}

/**
 * Shows an information message box with a checkbox to override the message box
 * in the future
 *
 * @param parent
 * @param title
 * @param text
 * @param identifier
 * @param buttons
 * @param defaultButton
 * @return
 */
QMessageBox::StandardButton Utils::Gui::information(QWidget *parent, const QString &title,
                                                    const QString &text, const QString &identifier,
                                                    QMessageBox::StandardButtons buttons,
                                                    QMessageBox::StandardButton defaultButton) {
    return showMessageBox(parent, QMessageBox::Icon::Information, title, text, identifier, buttons,
                          defaultButton);
}

/**
 * Shows a question message box with a checkbox to override the message box in
 * the future
 *
 * @param parent
 * @param title
 * @param text
 * @param identifier
 * @param buttons
 * @param defaultButton
 * @param skipOverrideButtons
 * @return
 */
QMessageBox::StandardButton Utils::Gui::question(QWidget *parent, const QString &title,
                                                 const QString &text, const QString &identifier,
                                                 QMessageBox::StandardButtons buttons,
                                                 QMessageBox::StandardButton defaultButton,
                                                 QMessageBox::StandardButtons skipOverrideButtons) {
    return showMessageBox(parent, QMessageBox::Icon::Question, title, text, identifier, buttons,
                          defaultButton, skipOverrideButtons);
}

/**
 * Shows a question message box with a checkbox to override the message box in
 * the future (but no skip override buttons can be defined)
 *
 * @param parent
 * @param title
 * @param text
 * @param identifier
 * @param buttons
 * @param defaultButton
 * @return
 */
QMessageBox::StandardButton Utils::Gui::questionNoSkipOverride(
    QWidget *parent, const QString &title, const QString &text, const QString &identifier,
    QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    return showMessageBox(parent, QMessageBox::Icon::Question, title, text, identifier, buttons,
                          defaultButton, QMessageBox::NoButton);
}

/**
 * Shows a warning message box with a checkbox to override the message box in
 * the future
 *
 * @param parent
 * @param title
 * @param text
 * @param identifier
 * @param buttons
 * @param defaultButton
 * @return
 */
QMessageBox::StandardButton Utils::Gui::warning(QWidget *parent, const QString &title,
                                                const QString &text, const QString &identifier,
                                                QMessageBox::StandardButtons buttons,
                                                QMessageBox::StandardButton defaultButton) {
    return showMessageBox(parent, QMessageBox::Icon::Warning, title, text, identifier, buttons,
                          defaultButton);
}

/**
 * Shows a message box with a checkbox to override the message box in the future
 *
 * @param parent
 * @param icon
 * @param title
 * @param text
 * @param identifier
 * @param buttons
 * @param defaultButton
 * @return
 */
QMessageBox::StandardButton Utils::Gui::showMessageBox(
    QWidget *parent, QMessageBox::Icon icon, const QString &title, const QString &text,
    const QString &identifier, QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton, QMessageBox::StandardButtons skipOverrideButtons) {
    SettingsService settings;
    const QString settingsKey = "MessageBoxOverride/" + identifier;
    auto overrideButton = static_cast<QMessageBox::StandardButton>(
        settings.value(settingsKey, QMessageBox::NoButton).toInt());

    // check if we want to override the message box
    if ((overrideButton != QMessageBox::NoButton) &&
        !skipOverrideButtons.testFlag(overrideButton)) {
        return overrideButton;
    }

    QMessageBox msgBox(icon, title, text, QMessageBox::NoButton, parent);
    auto *buttonBox = msgBox.findChild<QDialogButtonBox *>();
    Q_ASSERT(buttonBox != nullptr);
    auto *checkBox =
        new QCheckBox(icon == QMessageBox::Icon::Question ? QObject::tr("Don't ask again!")
                                                          : QObject::tr("Don't show again!"),
                      parent);

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
        auto sb = static_cast<uint>(buttons & mask);
        mask <<= 1;
        if (!sb) continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);

        // choose the first accept role as the default
        if (msgBox.defaultButton()) continue;
        if ((defaultButton == QMessageBox::NoButton &&
             buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) ||
            (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }

    // set the checkbox in the end, so it doesn't get the focus on the dialog
    // this would lead to accidentally checking the checkbox
    msgBox.setCheckBox(checkBox);

    if (msgBox.exec() == -1) return QMessageBox::Cancel;

    auto result = msgBox.standardButton(msgBox.clickedButton());

    // store the override for the message box
    if (checkBox->isChecked() && !skipOverrideButtons.testFlag(result)) {
        settings.setValue(settingsKey, result);
    }

    return result;
}

/**
 * Checks if a QMessageBox is already present on the screen
 */
bool Utils::Gui::isMessageBoxPresent() {
    QWidgetList topWidgets = QApplication::topLevelWidgets();
    foreach (QWidget *w, topWidgets) {
        if (auto *mb = dynamic_cast<QMessageBox *>(w)) {
            Q_UNUSED(mb)
            return true;
        }
    }

    return false;
}

/**
 * Opens a font dialog to select a font
 *
 * @param ok
 * @param initial
 * @param parent
 * @param title
 * @param options
 * @return
 */
QFont Utils::Gui::fontDialogGetFont(bool *ok, const QFont &initial, QWidget *parent,
                                    const QString &title, QFontDialog::FontDialogOptions options) {
#ifdef Q_OS_MAC
    // there was a bug in Qt 5.11.2 with the native dialog
    // see: https://github.com/pbek/QOwnNotes/issues/1033
    options |= QFontDialog::DontUseNativeDialog;
#endif

    return QFontDialog::getFont(ok, initial, parent, title, options);
}

/**
 * Copies the text from a copy block around initialBlock to the clipboard
 *
 * @param initialBlock
 */
void Utils::Gui::copyCodeBlockText(const QTextBlock &initialBlock) {
    QTextBlock block = initialBlock;
    QString codeBlockText = block.text();
    QStringList codeBlockTextList;
    codeBlockTextList << codeBlockText;

    // check the previous blocks
    while (true) {
        block = block.previous();

        if (!block.isValid()) {
            break;
        }

        if (block.userState() != MarkdownHighlighter::HighlighterState::CodeBlock &&
            block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockComment &&
            block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockTilde &&
            block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockTildeComment &&
            block.userState() < MarkdownHighlighter::HighlighterState::CodeCpp) {
            break;
        }
        codeBlockTextList.prepend(block.text());
    }

    // check the next blocks if we are not at the CodeBlockEnd
    if (!MarkdownHighlighter::isCodeBlockEnd(initialBlock.userState())) {
        block = initialBlock;

        while (true) {
            block = block.next();

            if (!block.isValid()) {
                break;
            }

            if (block.userState() != MarkdownHighlighter::HighlighterState::CodeBlock &&
                block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockComment &&
                block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockEnd &&
                block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockTilde &&
                block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockTildeComment &&
                block.userState() != MarkdownHighlighter::HighlighterState::CodeBlockTildeEnd &&
                block.userState() < MarkdownHighlighter::HighlighterState::CodeCpp) {
                break;
            }

            codeBlockTextList.append(block.text());
        }
    }

    if (!codeBlockTextList.isEmpty()) {
        if (codeBlockTextList.first().startsWith(QLatin1String("```")) ||
            codeBlockTextList.first().startsWith(QLatin1String("~~~")))
            codeBlockTextList.removeFirst();
        if (!codeBlockTextList.isEmpty() &&
            (codeBlockTextList.last().startsWith(QLatin1String("```")) ||
             codeBlockTextList.last().startsWith(QLatin1String("~~~"))))
            codeBlockTextList.removeLast();
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(codeBlockTextList.join(QStringLiteral("\n")));
}

/**
 * Attempts to toggle a checkbox at the cursor position
 *
 * @param textEdit
 */
bool Utils::Gui::toggleCheckBoxAtCursor(QPlainTextEdit *textEdit) {
    auto cursor = textEdit->textCursor();
    const int pos = cursor.position();

    // select the full range of "- [ ]" text in front and after cursor
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 5);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 10);

    bool result = false;
    auto text = cursor.selectedText();
    static const auto reUnchecked = QRegularExpression(R"(([-\+\*]) \[ \])");
    static const auto reChecked = QRegularExpression(R"(([-\+\*]) \[x\])");
    static const auto reNumberUnchecked = QRegularExpression(R"(([\d+]\.) \[ \])");
    static const auto reNumberChecked = QRegularExpression(R"(([\d+]\.) \[x\])");

    // try to toggle the checkbox
    if (reUnchecked.match(text).hasMatch()) {
        text.replace(reUnchecked, QStringLiteral("\\1 [x]"));
        result = true;
    } else if (reChecked.match(text).hasMatch()) {
        text.replace(reChecked, QStringLiteral("\\1 [ ]"));
        result = true;
    } else if (reNumberUnchecked.match(text).hasMatch()) {
        text.replace(reNumberUnchecked, QStringLiteral("\\1 [x]"));
        result = true;
    } else if (reNumberChecked.match(text).hasMatch()) {
        text.replace(reNumberChecked, QStringLiteral("\\1 [ ]"));
        result = true;
    }

    // insert the new checkbox text if it was toggled
    if (result) {
        cursor.insertText(text);
        cursor.setPosition(pos);
        textEdit->setTextCursor(cursor);
    }

    return result;
}

/**
 * Automatically formats a Markdown table in a text edit
 *
 * @param textEdit
 */
bool Utils::Gui::autoFormatTableAtCursor(QPlainTextEdit *textEdit) {
    QTextCursor initialCursor = textEdit->textCursor();
    QTextCursor cursor = initialCursor;
    cursor.movePosition(QTextCursor::EndOfBlock);
    int endPosition = cursor.position();
    QTextBlock initialBlock = cursor.block();
    QTextBlock block = initialBlock;
    // We are trimming to support leading spaces, because md4c seems to support up to 3 of it
    // See https://github.com/pbek/QOwnNotes/issues/3137
    const QString &tableText = block.text().trimmed();

    // return if text doesn't seem to be part of a table
    if (!tableText.startsWith(QLatin1String("|"))) {
        return false;
    }

    QList<QStringList> tableTextList;
    tableTextList << tableText.split(QStringLiteral("|"));
    int startPosition = block.position();
    int maxColumns = 0;
    bool tableWasModified = false;

    // check the previous blocks
    while (true) {
        block = block.previous();

        if (!block.isValid()) {
            break;
        }

        QString prevBlockText = block.text().trimmed();
        if (!prevBlockText.startsWith(QLatin1String("|"))) {
            break;
        }

        const QStringList &stringList = prevBlockText.split(QStringLiteral("|"));
        tableTextList.prepend(stringList);
        startPosition = block.position();
        maxColumns = std::max(maxColumns, (int)stringList.count());
    }

    // check the next blocks
    block = initialBlock;
    while (true) {
        block = block.next();

        if (!block.isValid()) {
            break;
        }

        QString nextBlockText = block.text().trimmed();
        if (!nextBlockText.startsWith(QLatin1String("|"))) {
            break;
        }

        const QStringList &stringList = nextBlockText.split(QStringLiteral("|"));
        tableTextList.append(stringList);
        endPosition = block.position() + nextBlockText.size();
        maxColumns = std::max(maxColumns, (int)stringList.count());
    }

    static const QRegularExpression headlineSeparatorRegExp(QStringLiteral(R"(^(:)?-+(:)?$)"));
    QString justifiedText;

    const int lineCount = tableTextList.size();
    if (lineCount < 2) {
        return false;
    }

    QVector<int> colLength;
    colLength << 1;

    // for every column, store maximum column length
    for (int col = 1; col < maxColumns; col++) {
        int maxTextLength = 0;
        for (int line = 0; line < lineCount; line++) {
            const QStringList &lineTextList = tableTextList.at(line);

            if (col >= lineTextList.count()) {
                break;
            }

            const QString &text = lineTextList.at(col).trimmed();

            // don't count the headline separator line, so it can shrink
            // down to 3 characters if needed
            if (line == 1 && headlineSeparatorRegExp.match(text).hasMatch()) {
                continue;
            }

            maxTextLength = std::max((int)text.size(), maxTextLength);
        }

        // a minimum of 3 headline separator characters are needed for
        // valid Markdown tables
        maxTextLength = std::max(3, maxTextLength);

        colLength << maxTextLength;
    }

    QVector<Qt::AlignmentFlag> colAlignment;
    colAlignment << Qt::AlignLeft;

    const QStringList &headerStringList = tableTextList.at(1);

    // for every column, store column alignment
    for (int col = 1; col < maxColumns - 1; col++) {
        const QString &text = headerStringList.at(col);
        QString trimmedText = text.trimmed();

        if (!headlineSeparatorRegExp.match(trimmedText).hasMatch()) {
            return false;
        }

        const bool startsWithCol = trimmedText.startsWith(QStringLiteral(":"));
        const bool endsWithCol = trimmedText.endsWith(QStringLiteral(":"));

        if (startsWithCol && endsWithCol) {
            colAlignment << Qt::AlignCenter;
        } else if (endsWithCol) {
            colAlignment << Qt::AlignRight;
        } else if (startsWithCol) {
            colAlignment << Qt::AlignLeft;
        } else {
            // implicit left alignment
            colAlignment << static_cast<Qt::AlignmentFlag>(0);
        }
    }

    // justify text in tableTextList
    // we can skip the first column in the list since it is bound to have no
    // text
    for (int col = 1; col < maxColumns; col++) {
        const Qt::AlignmentFlag alignment =
            colAlignment.value(col, static_cast<Qt::AlignmentFlag>(0));
        const int maxTextLength = colLength.at(col);

        // do the text justification
        for (int line = 0; line < lineCount; line++) {
            QStringList lineTextList = tableTextList.at(line);

            const int lineTextListCount = lineTextList.count();

            if (col >= lineTextListCount) {
                break;
            }

            const QString &text = lineTextList.at(col);

            // stop if the text in the last column is empty
            if (col == (lineTextListCount - 1) && text.isEmpty()) {
                break;
            }

            QString trimmedText = text.trimmed();

            if (line == 1) {
                // heading
                QString prefix(" ");
                QString suffix(" ");

                if (alignment == Qt::AlignCenter) {
                    prefix = " :";
                    suffix = ": ";
                } else if (alignment == Qt::AlignLeft) {
                    prefix = " :";
                } else if (alignment == Qt::AlignRight) {
                    suffix = ": ";
                }

                const int numSpecialChars(prefix.size() + suffix.size() - 2);
                justifiedText =
                    prefix + QStringLiteral("-").repeated(maxTextLength - numSpecialChars) + suffix;
            } else {
                if (alignment == Qt::AlignCenter) {
                    const int leftFillSize = (maxTextLength - trimmedText.size()) / 2;
                    const int rightFillSize = (maxTextLength - trimmedText.size()) - leftFillSize;
                    justifiedText = QString().fill(' ', leftFillSize) + trimmedText +
                                    QString().fill(' ', rightFillSize);
                } else if (alignment == Qt::AlignRight) {
                    justifiedText = trimmedText.rightJustified(maxTextLength);
                } else {
                    justifiedText = trimmedText.leftJustified(maxTextLength);
                }

                // add padding
                justifiedText = QStringLiteral(" ") + justifiedText + QStringLiteral(" ");
            }

            // update the tableTextList
            if (text != justifiedText) {
                lineTextList.replace(col, justifiedText);
                tableTextList.replace(line, lineTextList);
                tableWasModified = true;
            }
        }
    }

    // return if table doesn't need to be modified
    if (!tableWasModified) {
        return false;
    }

    // generate the new table text
    QString newTableText;
    for (int line = 0; line < lineCount; line++) {
        newTableText += tableTextList.at(line).join(QStringLiteral("|"));

        if (line < (lineCount - 1)) {
            newTableText += QStringLiteral("\n");
        }
    }

    // select whole table to overwrite text
    cursor.setPosition(startPosition);
    cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
    textEdit->setTextCursor(cursor);

    // overwrite the table
    cursor.insertText(newTableText);

    return true;
}

/**
 * Updates the interface font size from the interface font size override
 * settings
 */
void Utils::Gui::updateInterfaceFontSize(int fontSize) {
    SettingsService settings;
    bool overrideInterfaceFontSize =
        settings.value(QStringLiteral("overrideInterfaceFontSize"), false).toBool();

    // remove old style
    static const QRegularExpression re(
        QRegularExpression::escape(INTERFACE_OVERRIDE_STYLESHEET_PRE_STRING) +
        QStringLiteral(".*") +
        QRegularExpression::escape(INTERFACE_OVERRIDE_STYLESHEET_POST_STRING));
    QString stylesheet = qApp->styleSheet().remove(re);

    if (overrideInterfaceFontSize) {
        int interfaceFontSize =
            fontSize != -1 ? fontSize
                           : settings.value(QStringLiteral("interfaceFontSize"), 11).toInt();

        stylesheet += QStringLiteral("\n") + QString(INTERFACE_OVERRIDE_STYLESHEET_PRE_STRING) +
                      QStringLiteral("QWidget {font-size: ") + QString::number(interfaceFontSize) +
                      QStringLiteral("px;}") + QString(INTERFACE_OVERRIDE_STYLESHEET_POST_STRING);
    }

    qApp->setStyleSheet(stylesheet);
}

/**
 * Sets the current index of a combo box by user data
 */
void Utils::Gui::setComboBoxIndexByUserData(QComboBox *comboBox, const QVariant &userData) {
    const int index = comboBox->findData(userData);
    comboBox->setCurrentIndex(index);
}

int Utils::Gui::getTabWidgetIndexByProperty(QTabWidget *tabWidget, const QString &propertyName,
                                            const QVariant &propertyValue) {
    for (int i = 0; i < tabWidget->count(); i++) {
        const auto value = tabWidget->widget(i)->property(propertyName.toLocal8Bit());
        if (value == propertyValue) {
            return i;
        }
    }

    return -1;
}

int Utils::Gui::getTabWidgetNoteId(QTabWidget *tabWidget, int index) {
    QWidget *widget = tabWidget->widget(index);

    if (widget == nullptr) {
        return 0;
    }

    return widget->property("note-id").toInt();
}

Note Utils::Gui::getTabWidgetNote(QTabWidget *tabWidget, int index, bool fetchByName) {
    if (fetchByName) {
        QWidget *widget = tabWidget->widget(index);

        if (widget == nullptr) {
            return Note();
        }

        const QString &noteName = widget->property("note-name").toString();
        const QString &noteSubFolderPathData =
            widget->property("note-subfolder-path-data").toString();
        return Note::fetchByName(noteName, noteSubFolderPathData);
    } else {
        const int noteId = getTabWidgetNoteId(tabWidget, index);
        return Note::fetch(noteId);
    }
}

void Utils::Gui::storeNoteTabs(QTabWidget *tabWidget) {
    // check if we want to store note tabs
    const SettingsService settings;
    if (!settings.value(QStringLiteral("restoreNoteTabs"), true).toBool()) {
        return;
    }

    QStringList noteNameList;
    QStringList noteSubFolderPathDataList;
    QStringList noteStickinessList;

    for (int i = 0; i < tabWidget->count(); i++) {
        const Note note = getTabWidgetNote(tabWidget, i);

        if (!note.isFetched()) {
            continue;
        }

        noteNameList << note.getName();
        noteSubFolderPathDataList << note.getNoteSubFolder().pathData();

        if (isTabWidgetTabSticky(tabWidget, i)) {
            noteStickinessList << QString::number(i);
        }
    }

    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    noteFolder.setSettingsValue(QStringLiteral("NoteTabNameList"), noteNameList);
    noteFolder.setSettingsValue(QStringLiteral("NoteTabSubFolderPathDataList"),
                                noteSubFolderPathDataList);
    noteFolder.setSettingsValue(QStringLiteral("NoteTabStickinessList"), noteStickinessList);
}

void Utils::Gui::restoreNoteTabs(QTabWidget *tabWidget, QVBoxLayout *layout) {
    const QSignalBlocker blocker(tabWidget);
    Q_UNUSED(blocker)

    // remove all but the first tab
    while (tabWidget->count() > 1) {
        tabWidget->removeTab(1);
    }

    const SettingsService settings;

    // check if we want to restore note tabs
    if (settings.value(QStringLiteral("restoreNoteTabs"), true).toBool()) {
        NoteFolder noteFolder = NoteFolder::currentNoteFolder();
        const QStringList noteNameList =
            noteFolder.settingsValue(QStringLiteral("NoteTabNameList")).toStringList();
        const QStringList noteSubFolderPathDataList =
            noteFolder.settingsValue(QStringLiteral("NoteTabSubFolderPathDataList")).toStringList();
        const QStringList noteStickinessList =
            noteFolder.settingsValue(QStringLiteral("NoteTabStickinessList")).toStringList();
        const int noteNameListCount = noteNameList.count();

        // only restore if there was more than one tab and
        // NoteTabSubFolderPathDataList has enough entries
        if (noteNameListCount > 1 && noteSubFolderPathDataList.count() >= noteNameListCount) {
            for (int i = 0; i < noteNameListCount; i++) {
                const QString &noteName = noteNameList.at(i);
                const QString &noteSubFolderPathData = noteSubFolderPathDataList.at(i);
                const bool isSticky = noteStickinessList.contains(QString::number(i));
                const Note note = Note::fetchByName(noteName, noteSubFolderPathData);

                // skip if note was not found anymore
                if (!note.isFetched()) {
                    continue;
                }

                // create a new tab if there are too few tabs
                if ((tabWidget->count() - 1) < i) {
                    auto *widgetPage = new QWidget();
                    tabWidget->addTab(widgetPage, QLatin1String(""));
                }

                // set the current tab index and the note data
                tabWidget->setCurrentIndex(i);
                updateTabWidgetTabData(tabWidget, i, note);
                setTabWidgetTabSticky(tabWidget, i, isSticky);
            }
        }
    }

    // make sure a layout is set in the end
    tabWidget->currentWidget()->setLayout(layout);
}

void Utils::Gui::reloadNoteTabs(QTabWidget *tabWidget) {
    //    const QSignalBlocker blocker(tabWidget);
    //    Q_UNUSED(blocker)
    //    return;

    for (int i = 0; i < tabWidget->count(); i++) {
        const Note note = getTabWidgetNote(tabWidget, i, true);

        if (!note.isFetched()) {
            continue;
        }

        updateTabWidgetTabData(tabWidget, i, note);
    }
}

void Utils::Gui::updateTabWidgetTabData(QTabWidget *tabWidget, int index, const Note &note) {
    QWidget *widget = tabWidget->widget(index);

    if (widget == nullptr) {
        return;
    }

    widget->setProperty("note-id", note.getId());
    widget->setProperty("note-name", note.getName());
    widget->setProperty("note-subfolder-path-data", note.getNoteSubFolder().pathData());

    QString text = note.getName();
    const bool isSticky = isTabWidgetTabSticky(tabWidget, index);

    if (isSticky) {
        // https://unicode-table.com/en/search/?q=flag
        text.prepend(QStringLiteral("\u2690 "));
    }

    // you need to use "&&" to show a "&" in the tab text
    // see https://github.com/pbek/QOwnNotes/issues/2135
    tabWidget->setTabText(index, text.replace("&", "&&"));

    tabWidget->setTabToolTip(index, isSticky ? QObject::tr("Double-click to unstick note from tab")
                                             : QObject::tr("Double-click to stick note to tab"));
}

void Utils::Gui::setTabWidgetTabSticky(QTabWidget *tabWidget, int index, bool sticky) {
    QWidget *widget = tabWidget->widget(index);

    if (widget == nullptr) {
        return;
    }

    widget->setProperty("sticky", sticky);
    Note note = getTabWidgetNote(tabWidget, index);
    updateTabWidgetTabData(tabWidget, index, note);
}

bool Utils::Gui::isTabWidgetTabSticky(QTabWidget *tabWidget, int index) {
    QWidget *widget = tabWidget->widget(index);

    if (widget == nullptr) {
        return false;
    }

    return widget->property("sticky").toBool();
}

/**
 * Sets the tree widget tooltip for a note
 */
void Utils::Gui::setTreeWidgetItemToolTipForNote(QTreeWidgetItem *item, const Note &note,
                                                 QDateTime *overrideFileLastModified) {
    if (item == nullptr) {
        return;
    }

    QDateTime modified = note.getFileLastModified();
    QDateTime *fileLastModified =
        (overrideFileLastModified != nullptr) ? overrideFileLastModified : &modified;

    QString toolTipText =
        QObject::tr("<strong>%1</strong><br />last modified: %2<br />file size: %3")
            .arg(note.getFileName(), fileLastModified->toString(),
                 Utils::Misc::toHumanReadableByteSize(note.getFileSize()));

    NoteSubFolder noteSubFolder = note.getNoteSubFolder();
    if (noteSubFolder.isFetched()) {
        toolTipText += QObject::tr("<br />path: %1").arg(noteSubFolder.relativePath());
    }

#ifdef QT_DEBUG
    toolTipText += QStringLiteral("<br />id: %1").arg(note.getId());
#endif

    item->setToolTip(0, toolTipText);

    // TODO: handle item widget too
}

/**
 * Checks if Windows is in dark or light mode and if we want to switch to those modes too.
 * This only works under Windows 10 (or newer).
 */
bool Utils::Gui::doWindowsDarkModeCheck() {
    QSettings settings(
        R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
        QSettings::NativeFormat);

    // If setting wasn't found we are not on Windows 10 (or newer)
    if (!settings.contains("AppsUseLightTheme")) {
        return false;
    }

    bool windowsDarkMode = settings.value("AppsUseLightTheme") == 0;
    bool appDarkMode = SettingsService().value("darkMode").toBool();

    // Check for Windows dark mode and application default mode
    if (windowsDarkMode && !appDarkMode) {
        if (Utils::Gui::questionNoSkipOverride(
                nullptr, QObject::tr("Dark mode detected"),
                QObject::tr("Your Windows system seems to be in dark mode. "
                            "Do you also want to turn on dark mode in QOwnNotes?"),
                QStringLiteral("windows-dark-mode")) == QMessageBox::Yes) {
            Utils::Misc::switchToDarkMode();

            return true;
        }
    }

    // Check for Windows light mode and application dark mode
    if (!windowsDarkMode && appDarkMode) {
        if (Utils::Gui::questionNoSkipOverride(
                nullptr, QObject::tr("Light mode detected"),
                QObject::tr("Your Windows system seems to be in light mode. "
                            "Do you also want to turn off dark mode in QOwnNotes?"),
                QStringLiteral("windows-light-mode")) == QMessageBox::Yes) {
            Utils::Misc::switchToLightMode();

            return true;
        }
    }

    return false;
}

/**
 * Checks if Linux is in dark or light mode and if we want to switch to those modes too.
 * This only works with dbus.
 */
bool Utils::Gui::doLinuxDarkModeCheck() {
    //    auto result = Utils::Misc::startSynchronousProcess("/usr/bin/dbus-send", QStringList() <<
    //                    QStringLiteral("--session") << QStringLiteral("--print-reply=literal") <<
    //                    QStringLiteral("--reply-timeout=1000") <<
    //                    QStringLiteral("--dest=org.freedesktop.portal.Desktop") <<
    //                    QStringLiteral("/org/freedesktop/portal/desktop") <<
    //                    QStringLiteral("org.freedesktop.portal.Settings.Read") <<
    //                    QStringLiteral("string:'org.freedesktop.appearance'") <<
    //                    QStringLiteral("string:'color-scheme'"));

    //    auto parameters = QStringList() <<
    //                    QStringLiteral("--session") << QStringLiteral("--print-reply=literal") <<
    //                    QStringLiteral("--reply-timeout=1000") <<
    //                    QStringLiteral("--dest=org.freedesktop.portal.Desktop") <<
    //                    QStringLiteral("/org/freedesktop/portal/desktop") <<
    //                    QStringLiteral("org.freedesktop.portal.Settings.Read") <<
    //                    QStringLiteral("string:'org.freedesktop.appearance'") <<
    //                    QStringLiteral("string:'color-scheme'");

    //    auto parameters = QStringList() <<
    //                    QStringLiteral("--session") << QStringLiteral("--print-reply=literal") <<
    //                    QStringLiteral("--reply-timeout=1000") <<
    //                    QStringLiteral("--dest=org.freedesktop.portal.Desktop") <<
    //                    QStringLiteral("/org/freedesktop/portal/desktop") <<
    //                    QStringLiteral("org.freedesktop.portal.Settings.Read") <<
    //                    QStringLiteral("string:'org.freedesktop.appearance'
    //                    string:'color-scheme'");

    auto parameters = QStringList()
                      << "-c"
                      << "dbus-send --session --print-reply=literal --reply-timeout=1000 "
                         "--dest=org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop "
                         "org.freedesktop.portal.Settings.Read string:'org.freedesktop.appearance' "
                         "string:'color-scheme'";

    QProcess process;
    //    process.start(QStringLiteral("dbus-send"), parameters);
    process.start(QStringLiteral("/bin/sh"), parameters);

    if (!process.waitForStarted()) {
        qWarning() << __func__ << " - 'doLinuxDarkModeCheck' returned false";
        return false;
    }

    if (!process.waitForFinished()) {
        qWarning() << __func__ << " - 'doLinuxDarkModeCheck' returned false";
        return false;
    }

    //    const auto result = QString(process.readAllStandardError());
    const int systemColorSchema = QString(process.readAll()).trimmed().right(1).toInt();
    const bool appDarkMode = SettingsService().value("darkMode").toBool();

    // Check for Linux dark mode and application default mode
    if (systemColorSchema == 1 && !appDarkMode) {
        if (Utils::Gui::questionNoSkipOverride(
                nullptr, QObject::tr("Dark mode detected"),
                QObject::tr("Your Linux system seems to use the dark mode. "
                            "Do you also want to turn on dark mode in QOwnNotes?"),
                QStringLiteral("linux-dark-mode")) == QMessageBox::Yes) {
            Utils::Misc::switchToDarkMode();

            return true;
        }
    }

    // Check for Linux light mode and application dark mode
    if (systemColorSchema == 2 && appDarkMode) {
        if (Utils::Gui::questionNoSkipOverride(
                nullptr, QObject::tr("Light mode detected"),
                QObject::tr("Your Linux system seems to use the light mode. "
                            "Do you also want to turn off dark mode in QOwnNotes?"),
                QStringLiteral("linux-light-mode")) == QMessageBox::Yes) {
            Utils::Misc::switchToLightMode();

            return true;
        }
    }

    return false;
}

QIcon Utils::Gui::folderIcon() {
    static const QIcon s_folderIcon =
        QIcon::fromTheme(QStringLiteral("folder"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/folder.svg")));
    return s_folderIcon;
}

QIcon Utils::Gui::noteIcon() {
    static const QIcon s_noteIcon =
        QIcon::fromTheme(QStringLiteral("text-x-generic"),
                         QIcon(":icons/breeze-qownnotes/16x16/text-x-generic.svg"));
    return s_noteIcon;
}

QIcon Utils::Gui::favoriteNoteIcon() {
    static const QIcon s_favoriteNoteIcon = QIcon::fromTheme(
        QStringLiteral("favorite-favorited"),
        QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/favorite-favorited.svg")));
    return s_favoriteNoteIcon;
}

QIcon Utils::Gui::tagIcon() {
    static const QIcon s_tagIcon = QIcon::fromTheme(
        QStringLiteral("tag"), QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/tag.svg")));
    return s_tagIcon;
}

void Utils::Gui::handleTreeWidgetItemTagColor(QTreeWidgetItem *item, const Tag &tag) {
    if (item == nullptr) {
        qWarning() << "Unexpected null item in handleTreeWidgetItemTagColor";
        return;
    }
    const int columnCount = item->columnCount();
    if (columnCount == 0) {
        return;
    }

    // get the color from the tag
    QColor color = tag.getColor();

    // if no color was set reset it by using a transparent white
    if (!color.isValid()) {
        color = Qt::transparent;
    }

    QBrush brush = QBrush(color);

    // the tree widget events have to be blocked because when called in
    // assignColorToTagItem() the 2nd setBackground() crashes the app,
    // because it seems the tag tree will be reloaded
    const QSignalBlocker blocker(item->treeWidget());
    Q_UNUSED(blocker)

    // set the color for all columns
    for (int column = 0; column < columnCount; ++column) {
        item->setBackground(column, brush);
    }
}

void Utils::Gui::handleTreeWidgetItemTagColor(QTreeWidgetItem *item, int tagId) {
    const Tag tag = Tag::fetch(tagId);
    if (!tag.isFetched()) return;
    Utils::Gui::handleTreeWidgetItemTagColor(item, tag);
}

bool Utils::Gui::enableDockWidgetQuestion(QDockWidget *dockWidget) {
    if (dockWidget->isVisible()) {
        return true;
    }
    if (Utils::Gui::question(dockWidget, QObject::tr("Panel disabled"),
                             QObject::tr("Panel <strong>%1</strong> is currently disabled, "
                                         "do you want to turn it on again for this action to work?")
                                 .arg(dockWidget->windowTitle()),
                             QStringLiteral("enable-panel-question-") + dockWidget->objectName()) !=
        QMessageBox::Yes) {
        return false;
    }

    dockWidget->setVisible(true);
    return true;
}

/**
 * Checks if the cursor is currently in a Markdown table
 *
 * @param textEdit
 * @param cursorColumn optional pointer to store the column index (0-based, ignoring leading empty
 * column)
 * @return true if cursor is in a table
 */
bool Utils::Gui::isTableAtCursor(QPlainTextEdit *textEdit, int *cursorColumn) {
    QTextCursor cursor = textEdit->textCursor();
    QTextBlock block = cursor.block();
    const QString &tableText = block.text().trimmed();

    // Check if text is part of a table
    if (!tableText.startsWith(QLatin1String("|"))) {
        return false;
    }

    // If cursorColumn is provided, calculate which column the cursor is in
    if (cursorColumn != nullptr) {
        const QString blockText = block.text();
        const int posInBlock = cursor.positionInBlock();

        // Find the leading whitespace offset
        int leadingSpaces = 0;
        while (leadingSpaces < blockText.length() && blockText[leadingSpaces].isSpace()) {
            leadingSpaces++;
        }

        // Adjust position to account for leading spaces
        const int adjustedPos = posInBlock - leadingSpaces;

        // Count pipes before cursor position
        int col = 0;
        for (int i = 0; i < adjustedPos && i < tableText.length(); i++) {
            if (tableText[i] == '|') {
                col++;
            }
        }

        // Markdown tables have an empty first column (before first pipe)
        // so we subtract 1 to get the actual column index
        *cursorColumn = std::max(0, col - 1);
    }

    return true;
}

/**
 * Inserts a column to the left of the cursor position in a Markdown table
 *
 * @param textEdit
 * @return true if a column was inserted
 */
bool Utils::Gui::insertTableColumnLeft(QPlainTextEdit *textEdit) {
    int cursorColumn = 0;
    if (!isTableAtCursor(textEdit, &cursorColumn)) {
        return false;
    }

    QTextCursor initialCursor = textEdit->textCursor();
    QTextCursor cursor = initialCursor;
    cursor.movePosition(QTextCursor::EndOfBlock);
    QTextBlock initialBlock = cursor.block();
    QTextBlock block = initialBlock;

    // Collect all table rows
    QList<QTextBlock> tableBlocks;
    tableBlocks << block;
    int startPosition = block.position();
    int endPosition = cursor.position();

    // Check previous blocks
    block = initialBlock;
    while (true) {
        block = block.previous();
        if (!block.isValid() || !block.text().trimmed().startsWith(QLatin1String("|"))) {
            break;
        }
        tableBlocks.prepend(block);
        startPosition = block.position();
    }

    // Check next blocks
    block = initialBlock;
    while (true) {
        block = block.next();
        if (!block.isValid() || !block.text().trimmed().startsWith(QLatin1String("|"))) {
            break;
        }
        tableBlocks.append(block);
        QString nextBlockText = block.text().trimmed();
        endPosition = block.position() + block.text().size();
    }

    // Build new table with added column
    static const QRegularExpression headlineSeparatorRegExp(QStringLiteral(R"(^(:)?-+(:)?$)"));
    QString newTableText;

    for (int lineIdx = 0; lineIdx < tableBlocks.size(); lineIdx++) {
        const QString &lineText = tableBlocks.at(lineIdx).text().trimmed();
        QStringList parts = lineText.split(QStringLiteral("|"));

        // Insert empty column at cursorColumn + 1 (accounting for empty first element)
        const int insertIdx = cursorColumn + 1;

        if (lineIdx == 1 && insertIdx < parts.size()) {
            // This is the header separator line
            const QString &nextColText = parts.at(insertIdx).trimmed();
            if (headlineSeparatorRegExp.match(nextColText).hasMatch()) {
                // Insert separator matching the next column's alignment
                parts.insert(insertIdx, QStringLiteral(" --- "));
            } else {
                parts.insert(insertIdx, QStringLiteral("     "));
            }
        } else {
            parts.insert(insertIdx, QStringLiteral("     "));
        }

        newTableText += parts.join(QStringLiteral("|"));
        if (lineIdx < tableBlocks.size() - 1) {
            newTableText += QStringLiteral("\n");
        }
    }

    // Replace the table
    cursor.setPosition(startPosition);
    cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
    cursor.insertText(newTableText);

    // Format the table
    textEdit->setTextCursor(cursor);
    autoFormatTableAtCursor(textEdit);

    return true;
}

/**
 * Inserts a column to the right of the cursor position in a Markdown table
 *
 * @param textEdit
 * @return true if a column was inserted
 */
bool Utils::Gui::insertTableColumnRight(QPlainTextEdit *textEdit) {
    int cursorColumn = 0;
    if (!isTableAtCursor(textEdit, &cursorColumn)) {
        return false;
    }

    QTextCursor initialCursor = textEdit->textCursor();
    QTextCursor cursor = initialCursor;
    cursor.movePosition(QTextCursor::EndOfBlock);
    QTextBlock initialBlock = cursor.block();
    QTextBlock block = initialBlock;

    // Collect all table rows
    QList<QTextBlock> tableBlocks;
    tableBlocks << block;
    int startPosition = block.position();
    int endPosition = cursor.position();

    // Check previous blocks
    block = initialBlock;
    while (true) {
        block = block.previous();
        if (!block.isValid() || !block.text().trimmed().startsWith(QLatin1String("|"))) {
            break;
        }
        tableBlocks.prepend(block);
        startPosition = block.position();
    }

    // Check next blocks
    block = initialBlock;
    while (true) {
        block = block.next();
        if (!block.isValid() || !block.text().trimmed().startsWith(QLatin1String("|"))) {
            break;
        }
        tableBlocks.append(block);
        QString nextBlockText = block.text().trimmed();
        endPosition = block.position() + block.text().size();
    }

    // Build new table with added column
    static const QRegularExpression headlineSeparatorRegExp(QStringLiteral(R"(^(:)?-+(:)?$)"));
    QString newTableText;

    for (int lineIdx = 0; lineIdx < tableBlocks.size(); lineIdx++) {
        const QString &lineText = tableBlocks.at(lineIdx).text().trimmed();
        QStringList parts = lineText.split(QStringLiteral("|"));

        // Insert empty column at cursorColumn + 2 (accounting for empty first element, +1 for
        // right)
        const int insertIdx = cursorColumn + 2;

        if (lineIdx == 1 && insertIdx <= parts.size()) {
            // This is the header separator line
            const QString &prevColText = parts.at(cursorColumn + 1).trimmed();
            if (headlineSeparatorRegExp.match(prevColText).hasMatch()) {
                // Insert separator matching the current column's alignment
                parts.insert(insertIdx, QStringLiteral(" --- "));
            } else {
                parts.insert(insertIdx, QStringLiteral("     "));
            }
        } else {
            parts.insert(insertIdx, QStringLiteral("     "));
        }

        newTableText += parts.join(QStringLiteral("|"));
        if (lineIdx < tableBlocks.size() - 1) {
            newTableText += QStringLiteral("\n");
        }
    }

    // Replace the table
    cursor.setPosition(startPosition);
    cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
    cursor.insertText(newTableText);

    // Format the table
    textEdit->setTextCursor(cursor);
    autoFormatTableAtCursor(textEdit);

    return true;
}

/**
 * Inserts a row above the cursor position in a Markdown table
 *
 * @param textEdit
 * @return true if a row was inserted
 */
bool Utils::Gui::insertTableRowAbove(QPlainTextEdit *textEdit) {
    if (!isTableAtCursor(textEdit)) {
        return false;
    }

    QTextCursor cursor = textEdit->textCursor();
    QTextBlock currentBlock = cursor.block();
    const QString currentLineText = currentBlock.text().trimmed();

    // Count the number of columns in the current row
    QStringList parts = currentLineText.split(QStringLiteral("|"));
    int columnCount = parts.size();

    // Check if we're at the header separator row (line 1, 0-indexed)
    static const QRegularExpression headlineSeparatorRegExp(QStringLiteral(R"(^(:)?-+(:)?$)"));
    bool isAtSeparatorRow = false;

    // Check if this is a separator row
    for (int i = 1; i < parts.size() - 1; i++) {
        if (headlineSeparatorRegExp.match(parts.at(i).trimmed()).hasMatch()) {
            isAtSeparatorRow = true;
            break;
        }
    }

    // Don't allow inserting above the separator row (would break table structure)
    if (isAtSeparatorRow) {
        // Check if previous row exists and is the header
        QTextBlock prevBlock = currentBlock.previous();
        if (prevBlock.isValid() && prevBlock.text().trimmed().startsWith(QLatin1String("|"))) {
            // Insert before the header instead
            cursor.setPosition(prevBlock.position());
            cursor.movePosition(QTextCursor::StartOfBlock);
        } else {
            // Can't insert above separator if there's no header
            return false;
        }
    } else {
        // Position cursor at the start of current line
        cursor.movePosition(QTextCursor::StartOfBlock);
    }

    // Build empty row with same number of columns
    QString newRow;
    for (int i = 0; i < columnCount - 1; i++) {
        newRow += QStringLiteral("|");
        if (i > 0) {
            newRow += QStringLiteral("     ");
        }
    }
    newRow += QStringLiteral("\n");

    // Insert the new row
    cursor.insertText(newRow);

    // Format the table
    textEdit->setTextCursor(cursor);
    autoFormatTableAtCursor(textEdit);

    return true;
}

/**
 * Inserts a row below the cursor position in a Markdown table
 *
 * @param textEdit
 * @return true if a row was inserted
 */
bool Utils::Gui::insertTableRowBelow(QPlainTextEdit *textEdit) {
    if (!isTableAtCursor(textEdit)) {
        return false;
    }

    QTextCursor cursor = textEdit->textCursor();
    QTextBlock currentBlock = cursor.block();
    const QString currentLineText = currentBlock.text().trimmed();

    // Count the number of columns in the current row
    QStringList parts = currentLineText.split(QStringLiteral("|"));
    int columnCount = parts.size();

    // Position cursor at the end of current line
    cursor.movePosition(QTextCursor::EndOfBlock);

    // Build empty row with same number of columns
    QString newRow = QStringLiteral("\n");
    for (int i = 0; i < columnCount - 1; i++) {
        newRow += QStringLiteral("|");
        if (i > 0) {
            newRow += QStringLiteral("     ");
        }
    }

    // Insert the new row
    cursor.insertText(newRow);

    // Format the table
    textEdit->setTextCursor(cursor);
    autoFormatTableAtCursor(textEdit);

    return true;
}

/**
 * Fixes the icons of the dark mode
 * @param widget
 */
void Utils::Gui::fixDarkModeIcons(QWidget *widget) {
    const bool darkMode = SettingsService().value(QStringLiteral("darkMode")).toBool();

    if (!darkMode) {
        return;
    }

    foreach (QLineEdit *lineEdit, widget->findChildren<QLineEdit *>()) {
        auto action = lineEdit->findChild<QAction *>("_q_qlineeditclearaction");
        if (action) {
            action->setIcon(QIcon(QStringLiteral(":/images/cleartext-dark.svg")));
        }
    }
}

QAction *Utils::Gui::findActionByData(QMenu *menu, const QVariant &data) {
    // Iterate through all actions in the menu
    for (QAction *action : menu->actions()) {
        // Check if the action's data matches the desired data
        if (action->data() == data) {
            return action;
        }

        // Check if the action is a submenu
        if (QMenu *subMenu = action->menu()) {
            // Recursively search in the submenu
            QAction *foundAction = findActionByData(subMenu, data);
            if (foundAction) {
                return foundAction;
            }
        }
    }
    // Return nullptr if no matching action is found
    return nullptr;
}

void Utils::Gui::applyInterfaceStyle(QString interfaceStyle) {
    if (interfaceStyle.isEmpty()) {
        interfaceStyle = SettingsService().value(QStringLiteral("interfaceStyle")).toString();
    }

    // Apply custom style to hide menu icons
    if (Utils::Misc::areMenuIconsHidden()) {
        if (!interfaceStyle.isEmpty()) {
            // Apply the selected interface style and the custom style
            QStyle *interfaceStyleClass = QStyleFactory::create(interfaceStyle);
            QApplication::setStyle(new NoMenuIconStyle(interfaceStyleClass));
        } else {
            // Apply the custom style only
            QApplication::setStyle(new NoMenuIconStyle);
        }
    } else if (!interfaceStyle.isEmpty()) {
        // Restore the interface style
        QApplication::setStyle(interfaceStyle);
    }
}
