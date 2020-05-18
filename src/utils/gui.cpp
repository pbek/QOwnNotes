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

#include <libraries/qmarkdowntextedit/markdownhighlighter.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QDebug>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QTextBlock>
#include <QTextCursor>

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
                                           const QString &text,
                                           TreeWidgetSearchFlags searchFlags) {
    QStringList searchList;

    if (searchFlags & TreeWidgetSearchFlag::EveryWordSearch) {
        searchList = text.split(QRegularExpression(QStringLiteral("\\s+")));
    } else {
        searchList << text;
    }

    // get all items
    QList<QTreeWidgetItem *> allItems =
        treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (text.count() >= 1) {
        int searchColumnCount =
            searchFlags & TreeWidgetSearchFlag::AllColumnsSearch
                ? treeWidget->columnCount()
                : 1;

        // hide all not found items
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            bool show = false;

            // look in all columns that we want to search
            for (int index = 0; index < searchColumnCount; index++) {
                bool loopShow = true;

                foreach (QString searchText, searchList) {
                    // search for text in the columns
                    bool loopShow2 = item->text(index).contains(
                        searchText, Qt::CaseInsensitive);

                    // also show the item if the text was found in the tooltip
                    if (searchFlags & TreeWidgetSearchFlag::TooltipSearch) {
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
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            if (isOneTreeWidgetItemChildVisible(item)) {
                item->setHidden(false);
                item->setExpanded(true);
            }
        }
    } else {
        // show all items otherwise
        Q_FOREACH (QTreeWidgetItem *item, allItems) { item->setHidden(false); }
    }
}

/**
 * Searches for text in items of a list widget
 */
void Utils::Gui::searchForTextInListWidget(QListWidget *listWidget,
                                           const QString &text,
                                           bool searchAddProps) {
    QList<QListWidgetItem *> allItems =
        listWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (text.count() >= 1) {
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
        Q_FOREACH (QListWidgetItem *item, allItems) { item->setHidden(false); }
    }
}

/**
 * Checks if a variant exists as user data in a tree widget
 */
bool Utils::Gui::userDataInTreeWidgetExists(QTreeWidget *treeWidget,
                                            const QVariant &userData,
                                            int column) {
    return getTreeWidgetItemWithUserData(treeWidget, userData, column) !=
           Q_NULLPTR;
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
QTreeWidgetItem *Utils::Gui::getTreeWidgetItemWithUserData(
    QTreeWidget *treeWidget, const QVariant &userData, int column) {
    // get all items
    QList<QTreeWidgetItem *> allItems =
        treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    Q_FOREACH (QTreeWidgetItem *item, allItems) {
        if (userData == item->data(column, Qt::UserRole)) {
            return item;
        }
    }

    return Q_NULLPTR;
}

QListWidgetItem *Utils::Gui::getListWidgetItemWithUserData(
    QListWidget *listWidget, const QVariant &userData) {
    // get all items
    QList<QListWidgetItem *> allItems =
        listWidget->findItems("", Qt::MatchContains);

    Q_FOREACH (QListWidgetItem *item, allItems) {
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
void Utils::Gui::resetBoldStateOfAllTreeWidgetItems(QTreeWidget *treeWidget,
                                                    int column) {
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
QMessageBox::StandardButton Utils::Gui::information(
    QWidget *parent, const QString &title, const QString &text,
    const QString &identifier, QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton) {
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
 * @param skipOverrideButtons
 * @return
 */
QMessageBox::StandardButton Utils::Gui::question(
    QWidget *parent, const QString &title, const QString &text,
    const QString &identifier, QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton,
    QMessageBox::StandardButtons skipOverrideButtons) {
    return showMessageBox(parent, QMessageBox::Icon::Question, title, text,
                          identifier, buttons, defaultButton,
                          skipOverrideButtons);
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
    QWidget *parent, const QString &title, const QString &text,
    const QString &identifier, QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton) {
    return showMessageBox(parent, QMessageBox::Icon::Question, title, text,
                          identifier, buttons, defaultButton,
                          QMessageBox::NoButton);
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
    const QString &identifier, QMessageBox::StandardButtons buttons,
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
QMessageBox::StandardButton Utils::Gui::showMessageBox(
    QWidget *parent, QMessageBox::Icon icon, const QString &title,
    const QString &text, const QString &identifier,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton,
    QMessageBox::StandardButtons skipOverrideButtons) {
    QSettings settings;
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
    auto *checkBox = new QCheckBox(icon == QMessageBox::Icon::Question
                                       ? QObject::tr("Don't ask again!")
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
            (defaultButton != QMessageBox::NoButton &&
             sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }

    // set the checkbox in the end so it doesn't get the focus on the dialog.
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

        if (block.userState() !=
                MarkdownHighlighter::HighlighterState::CodeBlock &&
            block.userState() !=
                MarkdownHighlighter::HighlighterState::CodeBlockComment &&
            block.userState() !=
                MarkdownHighlighter::HighlighterState::CodeBlockTilde &&
            block.userState() !=
                MarkdownHighlighter::HighlighterState::CodeBlockTildeComment &&
            block.userState() <
                MarkdownHighlighter::HighlighterState::CodeCpp) {
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

            if (block.userState() !=
                    MarkdownHighlighter::HighlighterState::CodeBlock &&
                block.userState() !=
                    MarkdownHighlighter::HighlighterState::CodeBlockComment &&
                block.userState() !=
                    MarkdownHighlighter::HighlighterState::CodeBlockEnd &&
                block.userState() !=
                    MarkdownHighlighter::HighlighterState::CodeBlockTilde &&
                block.userState() != MarkdownHighlighter::HighlighterState::
                                         CodeBlockTildeComment &&
                block.userState() !=
                    MarkdownHighlighter::HighlighterState::CodeBlockTildeEnd &&
                block.userState() <
                    MarkdownHighlighter::HighlighterState::CodeCpp) {
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
 * Automatically formats a markdown table in a text edit
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

    // return if text doesn't seem to be part of a table
    if (!block.text().startsWith(QLatin1String("|"))) {
        return false;
    }

    QString tableText = block.text();
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

        QString prevBlockText = block.text();
        if (!prevBlockText.startsWith(QLatin1String("|"))) {
            break;
        }

        const QStringList &stringList =
            prevBlockText.split(QStringLiteral("|"));
        tableTextList.prepend(stringList);
        startPosition = block.position();
        maxColumns = std::max(maxColumns, stringList.count());
    }

    // check the next blocks
    block = initialBlock;
    while (true) {
        block = block.next();

        if (!block.isValid()) {
            break;
        }

        QString nextBlockText = block.text();
        if (!nextBlockText.startsWith(QLatin1String("|"))) {
            break;
        }

        const QStringList &stringList =
            nextBlockText.split(QStringLiteral("|"));
        tableTextList.append(stringList);
        endPosition = block.position() + nextBlockText.count();
        maxColumns = std::max(maxColumns, stringList.count());
    }

    QRegularExpression headlineSeparatorRegExp(QStringLiteral(R"(^-+$)"));
    QString justifiedText;

    // justify text in tableTextList
    // we can skip the first column in the list since it is bound to have no
    // text
    const int lineCount = tableTextList.count();
    for (int col = 1; col < maxColumns; col++) {
        // find the maximum text length
        int maxTextLength = 0;
        for (int line = 0; line < lineCount; line++) {
            const QStringList &lineTextList = tableTextList.at(line);

            if (col >= lineTextList.count()) {
                break;
            }

            const QString &text = lineTextList.at(col).trimmed();
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
            if (headlineSeparatorRegExp.match(text.trimmed()).hasMatch()) {
                // justify the headline separator text
                justifiedText = QStringLiteral(" ") +
                                QStringLiteral("-").repeated(maxTextLength) +
                                QStringLiteral(" ");
            } else {
                // justify the text
                justifiedText = QStringLiteral(" ") +
                                text.trimmed().leftJustified(maxTextLength) +
                                QStringLiteral(" ");
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
    QSettings settings;
    bool overrideInterfaceFontSize =
        settings.value(QStringLiteral("overrideInterfaceFontSize"), false)
            .toBool();

    // remove old style
    QString stylesheet = qApp->styleSheet().remove(QRegularExpression(
        QRegularExpression::escape(INTERFACE_OVERRIDE_STYLESHEET_PRE_STRING) +
        QStringLiteral(".*") +
        QRegularExpression::escape(INTERFACE_OVERRIDE_STYLESHEET_POST_STRING)));

    if (overrideInterfaceFontSize) {
        int interfaceFontSize =
            fontSize != -1
                ? fontSize
                : settings.value(QStringLiteral("interfaceFontSize"), 11)
                      .toInt();

        stylesheet += QStringLiteral("\n") +
                      QString(INTERFACE_OVERRIDE_STYLESHEET_PRE_STRING) +
                      QStringLiteral("QWidget {font-size: ") +
                      QString::number(interfaceFontSize) +
                      QStringLiteral("px;}") +
                      QString(INTERFACE_OVERRIDE_STYLESHEET_POST_STRING);
    }

    qApp->setStyleSheet(stylesheet);
}

/**
 * Sets the current index of a combo box by user data
 */
void Utils::Gui::setComboBoxIndexByUserData(QComboBox *comboBox,
                                            const QVariant &userData) {
    const int index = comboBox->findData(userData);
    comboBox->setCurrentIndex(index);
}

int Utils::Gui::getTabWidgetIndexByProperty(QTabWidget *tabWidget,
                                            const QString &propertyName,
                                            const QVariant &propertyValue) {
    for (int i = 0; i < tabWidget->count(); i++) {
        const auto value = tabWidget->widget(i)->property(
                          propertyName.toLocal8Bit());
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

Note Utils::Gui::getTabWidgetNote(QTabWidget *tabWidget, int index,
                                  bool fetchByName) {
    if (fetchByName) {
        QWidget *widget = tabWidget->widget(index);

        if (widget == nullptr) {
            return Note();
        }

        const QString &noteName = widget->property("note-name").toString();
        const QString &noteSubFolderPathData = widget->property(
                             "note-subfolder-path-data").toString();
        return Note::fetchByName(noteName, noteSubFolderPathData);
    } else {
        const int noteId = getTabWidgetNoteId(tabWidget, index);
        return Note::fetch(noteId);
    }
}

void Utils::Gui::storeNoteTabs(QTabWidget *tabWidget) {
    // check if we want to store note tabs
    const QSettings settings;
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
    noteFolder.setSettingsValue(QStringLiteral("NoteTabNameList"),
                                noteNameList);
    noteFolder.setSettingsValue(QStringLiteral("NoteTabSubFolderPathDataList"),
                                noteSubFolderPathDataList);
    noteFolder.setSettingsValue(QStringLiteral("NoteTabStickinessList"),
                                noteStickinessList);
}

void Utils::Gui::restoreNoteTabs(QTabWidget *tabWidget, QVBoxLayout *layout) {
    const QSignalBlocker blocker(tabWidget);
    Q_UNUSED(blocker)

    // remove all but the first tab
    while (tabWidget->count() > 1) {
        tabWidget->removeTab(1);
    }

    const QSettings settings;

    // check if we want to restore note tabs
    if (settings.value(QStringLiteral("restoreNoteTabs"), true).toBool()) {
        NoteFolder noteFolder = NoteFolder::currentNoteFolder();
        const QStringList noteNameList = noteFolder.settingsValue(
            QStringLiteral("NoteTabNameList")).toStringList();
        const QStringList noteSubFolderPathDataList = noteFolder.settingsValue(
            QStringLiteral("NoteTabSubFolderPathDataList")).toStringList();
        const QStringList noteStickinessList = noteFolder.settingsValue(
            QStringLiteral("NoteTabStickinessList")).toStringList();
        const int noteNameListCount = noteNameList.count();

        // only restore if there was more than one tab and
        // NoteTabSubFolderPathDataList has enough entries
        if (noteNameListCount > 1 &&
            noteSubFolderPathDataList.count() >= noteNameListCount) {
            for (int i = 0; i < noteNameListCount; i++) {
                const QString &noteName = noteNameList.at(i);
                const QString &noteSubFolderPathData =
                    noteSubFolderPathDataList.at(i);
                const bool isSticky = noteStickinessList.contains(
                    QString::number(i));
                const Note note = Note::fetchByName(noteName,
                                                    noteSubFolderPathData);

                // skip if note was not found any more
                if (!note.isFetched()) {
                    continue;
                }

                // create a new tab if there are too few tabs
                if ((tabWidget->count() - 1) < i) {
                    auto *widgetPage = new QWidget();
                    tabWidget->addTab(widgetPage, QStringLiteral(""));
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

void Utils::Gui::updateTabWidgetTabData(QTabWidget *tabWidget, int index,
                                       const Note &note) {
    QWidget *widget = tabWidget->widget(index);

    if (widget == nullptr) {
        return;
    }

    widget->setProperty("note-id", note.getId());
    widget->setProperty("note-name", note.getName());
    widget->setProperty("note-subfolder-path-data",
                        note.getNoteSubFolder().pathData());

    QString text = note.getName();
    const bool isSticky = isTabWidgetTabSticky(tabWidget, index);

    if (isSticky) {
        // https://unicode-table.com/en/search/?q=flag
        text.prepend(QStringLiteral("\u2690 "));
    }

    tabWidget->setTabText(index, text);
    tabWidget->setTabToolTip(index, isSticky ?
            QObject::tr("Double-click to unstick note from tab") :
            QObject::tr("Double-click to stick note to tab"));
}

void Utils::Gui::setTabWidgetTabSticky(QTabWidget *tabWidget, int index,
                                       bool sticky) {
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
