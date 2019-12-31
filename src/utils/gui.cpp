/*
 * Copyright (c) 2014-2020 Patrizio Bekerle -- <patrizio@bekerle.com>
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
#include <QDebug>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QPushButton>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QTextBlock>
#include <QTextCursor>
#include <QClipboard>
#include <libraries/qmarkdowntextedit/markdownhighlighter.h>


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
void Utils::Gui::searchForTextInTreeWidget(QTreeWidget *treeWidget,
                                           const QString& text,
                                           TreeWidgetSearchFlags searchFlags) {
    QStringList searchList;

    if (searchFlags & TreeWidgetSearchFlag::EveryWordSearch) {
        searchList = text.split(QRegularExpression(QStringLiteral("\\s+")));
    } else {
        searchList << text;
    }

    // get all items
    QList<QTreeWidgetItem*> allItems = treeWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (text.count() >= 1) {
        int searchColumnCount = searchFlags &
                TreeWidgetSearchFlag::AllColumnsSearch ?
                                treeWidget->columnCount() : 1;

        // hide all not found items
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                bool show = false;

                // look in all columns that we want to search
                for (int index = 0; index < searchColumnCount; index++) {
                    bool loopShow = true;

                    foreach(QString searchText, searchList) {
                            // search for text in the columns
                            bool loopShow2 = item->text(index).contains(
                                    searchText, Qt::CaseInsensitive);

                            // also show the item if the text was found in the tooltip
                            if (searchFlags &
                                TreeWidgetSearchFlag::TooltipSearch) {
                                loopShow2 |= item->toolTip(index).contains(
                                        searchText, Qt::CaseInsensitive);
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
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                if (isOneTreeWidgetItemChildVisible(item)) {
                    item->setHidden(false);
                    item->setExpanded(true);
                }
            }
    } else {
        // show all items otherwise
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                item->setHidden(false);
            }
    }
}

/**
 * Searches for text in items of a list widget
 */
void Utils::Gui::searchForTextInListWidget(QListWidget *listWidget,
                                           const QString& text) {
    QList<QListWidgetItem*> allItems = listWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (text.count() >= 1) {
        QList<QListWidgetItem*> items = listWidget->
                findItems(text, Qt::MatchContains | Qt::MatchRecursive);

        // hide all not found items
        Q_FOREACH(QListWidgetItem *item, allItems) {
            item->setHidden(!items.contains(item));
        }
    } else {
        // show all items otherwise
        Q_FOREACH(QListWidgetItem *item, allItems) {
            item->setHidden(false);
        }
    }
}

/**
 * Checks if a variant exists as user data in a tree widget
 */
bool Utils::Gui::userDataInTreeWidgetExists(QTreeWidget *treeWidget,
                                            const QVariant& userData, int column) {
    return getTreeWidgetItemWithUserData(treeWidget, userData, column) != Q_NULLPTR;
}

/**
 * Returns the tree widget item of a tree widget with that has a certain user data
 *
 * @param treeWidget
 * @param userData
 * @param column
 * @return
 */
QTreeWidgetItem *Utils::Gui::getTreeWidgetItemWithUserData(
        QTreeWidget *treeWidget, const QVariant &userData, int column) {
    // get all items
    QList<QTreeWidgetItem*> allItems = treeWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    Q_FOREACH(QTreeWidgetItem *item, allItems) {
            if (userData == item->data(column, Qt::UserRole)) {
                return item;
            }
        }

    return Q_NULLPTR;
}

QListWidgetItem *Utils::Gui::getListWidgetItemWithUserData(
        QListWidget *listWidget, const QVariant &userData) {
    // get all items
    QList<QListWidgetItem*> allItems = listWidget->
            findItems("", Qt::MatchContains);

    Q_FOREACH(QListWidgetItem *item, allItems) {
            if (userData == item->data(Qt::UserRole)) {
                return item;
            }
        }

    return Q_NULLPTR;
}

/**
 * Resets the bold state of all tree widget items of a tree widget
 *
 * @param treeWidget
 * @param column
 */
void Utils::Gui::resetBoldStateOfAllTreeWidgetItems(
        QTreeWidget *treeWidget, int column) {
    // get all items
    QList<QTreeWidgetItem*> allItems = treeWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    Q_FOREACH(QTreeWidgetItem *item, allItems) {
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
QMessageBox::StandardButton Utils::Gui::information(
        QWidget *parent, const QString &title, const QString& text,
        const QString &identifier,
        QMessageBox::StandardButtons buttons,
        QMessageBox::StandardButton defaultButton)
{
    return showMessageBox(parent, QMessageBox::Icon::Information, title, text,
                          identifier, buttons, defaultButton);
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
 * @return
 */
QMessageBox::StandardButton Utils::Gui::question(
        QWidget *parent, const QString &title, const QString &text,
        const QString &identifier,
        QMessageBox::StandardButtons buttons,
        QMessageBox::StandardButton defaultButton) {
    return showMessageBox(parent, QMessageBox::Icon::Question, title, text,
                          identifier, buttons, defaultButton);
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
QMessageBox::StandardButton Utils::Gui::warning(
        QWidget *parent, const QString &title, const QString &text,
        const QString &identifier,
        QMessageBox::StandardButtons buttons,
        QMessageBox::StandardButton defaultButton) {
    return showMessageBox(parent, QMessageBox::Icon::Warning, title, text,
                          identifier, buttons, defaultButton);
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
QMessageBox::StandardButton
Utils::Gui::showMessageBox(QWidget *parent, QMessageBox::Icon icon,
                           const QString &title, const QString &text,
                           const QString &identifier,
                           QMessageBox::StandardButtons buttons,
                           QMessageBox::StandardButton defaultButton) {
    QSettings settings;
    const QString settingsKey = "MessageBoxOverride/" + identifier;
    auto overrideButton = static_cast<QMessageBox::StandardButton>(
            settings.value(settingsKey, QMessageBox::NoButton).toInt());

    // check if we want to override the message box
    if (overrideButton != QMessageBox::NoButton) {
        return overrideButton;
    }

    QMessageBox msgBox(icon, title, text, QMessageBox::NoButton, parent);
    auto *buttonBox = msgBox.findChild<QDialogButtonBox*>();
    Q_ASSERT(buttonBox != nullptr);
    auto *checkBox = new QCheckBox(
            icon == QMessageBox::Icon::Question ?
            QObject::tr("Don't ask again!") : QObject::tr("Don't show again!"),
            parent);

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
        auto sb = static_cast<uint>(buttons & mask);
        mask <<= 1;
        if (!sb)
            continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);

        // choose the first accept role as the default
        if (msgBox.defaultButton())
            continue;
        if ((defaultButton == QMessageBox::NoButton &&
                buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
            || (defaultButton != QMessageBox::NoButton &&
                sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }

    // set the checkbox in the end so it doesn't get the focus on the dialog.
    // this would lead to accidentally checking the checkbox
    msgBox.setCheckBox(checkBox);

    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;

    auto result = msgBox.standardButton(msgBox.clickedButton());

    // store the override for the message box
    if (checkBox->isChecked()) {
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
QFont Utils::Gui::fontDialogGetFont(bool *ok, const QFont &initial,
        QWidget *parent, const QString &title,
        QFontDialog::FontDialogOptions options) {

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
void Utils::Gui::copyCodeBlockText(const QTextBlock& initialBlock) {
    QTextBlock block = initialBlock;
    QString codeBlockText = block.text();
    QStringList codeBlockTextList;
    codeBlockTextList << codeBlockText;

    // check the previous blocks
    while(true) {
        block = block.previous();

        if (!block.isValid()) {
            break;
        }

        if (block.userState() != 
            MarkdownHighlighter::HighlighterState::CodeBlock) {
            break;
        }

        codeBlockTextList.prepend(block.text());
    }

    // check the next blocks if we are not at the CodeBlockEnd
    if (initialBlock.userState() !=
        MarkdownHighlighter::HighlighterState::CodeBlockEnd) {
        block = initialBlock;

        while(true) {
            block = block.next();

            if (!block.isValid()) {
                break;
            }

            if (block.userState() !=
                MarkdownHighlighter::HighlighterState::CodeBlock &&
                    block.userState() !=
                    MarkdownHighlighter::HighlighterState::CodeBlockEnd) {
                break;
            }

            codeBlockTextList.append(block.text());
        }
    }

    codeBlockTextList.removeFirst();
    codeBlockTextList.removeLast();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(codeBlockTextList.join(QStringLiteral("\n") + QStringLiteral("\n")));
}

/**
 * Automatically formats a markdown table in a text edit
 *
 * @param textEdit
 */
bool Utils::Gui::autoFormatTableAtCursor(QPlainTextEdit *textEdit) {
    QTextCursor initialCursor = textEdit->textCursor();
    QTextCursor cursor = initialCursor;
    cursor.movePosition(QTextCursor::EndOfLine);
    int endPosition = cursor.position();
    QTextBlock initialBlock = cursor.block();
    QTextBlock block = initialBlock;

    // return if text doesn't seem to be part of a table
    if (!block.text().startsWith(QLatin1String("|"))) {
        return false;
    }

    QString tableText = block.text();
    QList <QStringList> tableTextList;
    tableTextList << tableText.split(QStringLiteral("|"));
    int startPosition = block.position();
    int maxColumns = 0;
    bool tableWasModified = false;

    // check the previous blocks
    while(true) {
        block = block.previous();

        if (!block.isValid()) {
            break;
        }

        QString prevBlockText = block.text();
        if (!prevBlockText.startsWith(QLatin1String("|"))) {
            break;
        }

        const QStringList &stringList = prevBlockText.split(QStringLiteral("|"));
        tableTextList.prepend(stringList);
        startPosition = block.position();
        maxColumns = std::max(maxColumns, stringList.count());
    }

    // check the next blocks
    block = initialBlock;
    while(true) {
        block = block.next();

        if (!block.isValid()) {
            break;
        }

        QString nextBlockText = block.text();
        if (!nextBlockText.startsWith(QLatin1String("|"))) {
            break;
        }

        const QStringList &stringList = nextBlockText.split(QStringLiteral("|"));
        tableTextList.append(stringList);
        endPosition = block.position() + nextBlockText.count();
        maxColumns = std::max(maxColumns, stringList.count());
    }

    QRegularExpression headlineSeparatorRegExp(QStringLiteral(R"(^-+$)"));
    QString justifiedText;

    // justify text in tableTextList
    // we can skip the first column in the list since it is bound to have no text
    const int lineCount = tableTextList.count();
    for (int col = 1; col < maxColumns; col++) {
        // find the maximum text length
        int maxTextLength = 0;
        for (int line = 0; line < lineCount; line++) {
            const QStringList &lineTextList = tableTextList.at(line);

            if (col >= lineTextList.count()) {
                break;
            }

            const QString &text = lineTextList.at(col);
            maxTextLength = std::max(text.count(), maxTextLength);
        }

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

            // check if text is a headline separator
            if (maxTextLength > 2 &&
                headlineSeparatorRegExp.match(text.trimmed()).hasMatch()) {
                // justify the headline separator text
                justifiedText = QStringLiteral(" ") + text.trimmed().leftJustified(
                        maxTextLength - 2, '-') + QStringLiteral(" ");
            } else {
                // justify the text
                justifiedText = text.leftJustified(maxTextLength);

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
 * Updates the interface font size from the interface font size override settings
 */
void Utils::Gui::updateInterfaceFontSize(int fontSize) {
    QSettings settings;
    bool overrideInterfaceFontSize = settings.value(
            QStringLiteral("overrideInterfaceFontSize"), false).toBool();

    // remove old style
    QString stylesheet = qApp->styleSheet().remove(QRegularExpression(
            QRegularExpression::escape(INTERFACE_OVERRIDE_STYLESHEET_PRE_STRING) +
            QStringLiteral(".*") + QRegularExpression::escape(INTERFACE_OVERRIDE_STYLESHEET_POST_STRING)));

    if (overrideInterfaceFontSize) {
        int interfaceFontSize = fontSize != -1 ?
                fontSize : settings.value(QStringLiteral("interfaceFontSize"), 11).toInt();

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
