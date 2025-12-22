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

#pragma once

#include <entities/note.h>

#include <QFontDialog>
#include <QMessageBox>
#include <QtWidgets/QMenu>

class QTreeWidgetItem;
class QTreeWidget;
class QListWidget;
class QListWidgetItem;
class QTextBlock;
class QTabWidget;
class QComboBox;
class QPlainTextEdit;
class QVBoxLayout;
class QDockWidget;
class Tag;

/*  Gui functions that can be useful */

#define INTERFACE_OVERRIDE_STYLESHEET_PRE_STRING "/* BEGIN INTERFACE OVERRIDE STYLESHEET */"
#define INTERFACE_OVERRIDE_STYLESHEET_POST_STRING "/* END INTERFACE OVERRIDE STYLESHEET */"

namespace Utils {
namespace Gui {
enum TreeWidgetSearchFlag {
    None = 0x0000,

    // also show the item if an integer id is greater than 0
    IntCheck = 0x0001,

    // also show the item if the text was found in the tooltip
    TooltipSearch = 0x0002,

    // search in all columns
    AllColumnsSearch = 0x0004,

    // search for every word in the search text
    EveryWordSearch = 0x0008
};

Q_DECLARE_FLAGS(TreeWidgetSearchFlags, TreeWidgetSearchFlag)

Qt::SortOrder toQtOrder(int order);

bool isOneTreeWidgetItemChildVisible(QTreeWidgetItem *item);

void searchForTextInTreeWidget(QTreeWidget *treeWidget, const QString &text,
                               TreeWidgetSearchFlags searchFlags = None);
void searchForTextInListWidget(QListWidget *listWidget, const QString &text,
                               bool searchAddProps = false);

QMessageBox::StandardButton showMessageBox(
    QWidget *parent, QMessageBox::Icon icon, const QString &title, const QString &text,
    const QString &identifier, QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton,
    QMessageBox::StandardButtons skipOverrideButtons = QMessageBox::NoButton);

QMessageBox::StandardButton information(
    QWidget *parent, const QString &title, const QString &text,
    const QString &identifier = QStringLiteral("default"),
    QMessageBox::StandardButtons buttons = QMessageBox::Ok,
    QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

QMessageBox::StandardButton question(
    QWidget *parent, const QString &title, const QString &text,
    const QString &identifier = QStringLiteral("default"),
    QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes |
                                                                        QMessageBox::No),
    QMessageBox::StandardButton defaultButton = QMessageBox::NoButton,
    QMessageBox::StandardButtons skipOverrideButtons =
        QMessageBox::StandardButtons(QMessageBox::No));

QMessageBox::StandardButton questionNoSkipOverride(
    QWidget *parent, const QString &title, const QString &text,
    const QString &identifier = QStringLiteral("default"),
    QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes |
                                                                        QMessageBox::No),
    QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton warning(
    QWidget *parent, const QString &title, const QString &text,
    const QString &identifier = QStringLiteral("default"),
    QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Ok),
    QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

bool userDataInTreeWidgetExists(QTreeWidget *treeWidget, const QVariant &userData, int column = 0);

QTreeWidgetItem *getTreeWidgetItemWithUserData(QTreeWidget *treeWidget, const QVariant &userData,
                                               int column = 0);

QListWidgetItem *getListWidgetItemWithUserData(QListWidget *listWidget, const QVariant &userData);

void resetBoldStateOfAllTreeWidgetItems(QTreeWidget *treeWidget, int column = 0);

bool isMessageBoxPresent();

QFont fontDialogGetFont(bool *ok, const QFont &initial, QWidget *parent = nullptr,
                        const QString &title = QString(),
                        QFontDialog::FontDialogOptions options = QFontDialog::FontDialogOptions());

void copyCodeBlockText(const QTextBlock &initialBlock);

bool toggleCheckBoxAtCursor(QPlainTextEdit *textEdit);
bool autoFormatTableAtCursor(QPlainTextEdit *textEdit);

void updateInterfaceFontSize(int fontSize = -1);

void setComboBoxIndexByUserData(QComboBox *comboBox, const QVariant &userData);

int getTabWidgetIndexByProperty(QTabWidget *tabWidget, const QString &propertyName,
                                const QVariant &propertyValue);
int getTabWidgetNoteId(QTabWidget *tabWidget, int index);
Note getTabWidgetNote(QTabWidget *tabWidget, int index, bool fetchByName = false);
void storeNoteTabs(QTabWidget *tabWidget);
void restoreNoteTabs(QTabWidget *tabWidget, QVBoxLayout *layout);
void setTabWidgetTabSticky(QTabWidget *tabWidget, int index, bool sticky);
bool isTabWidgetTabSticky(QTabWidget *tabWidget, int index);
void updateTabWidgetTabData(QTabWidget *tabWidget, int index, const Note &note);
void reloadNoteTabs(QTabWidget *tabWidget);
void setTreeWidgetItemToolTipForNote(QTreeWidgetItem *item, const Note &note,
                                     QDateTime *overrideFileLastModified = nullptr);
bool doWindowsDarkModeCheck();
bool doLinuxDarkModeCheck();

QIcon folderIcon();
QIcon noteIcon();
QIcon favoriteNoteIcon();
QIcon tagIcon();

/**
 * Reads the color from a tag and sets the background color of a tree widget
 * item
 *
 * @param item
 * @param tag
 */
void handleTreeWidgetItemTagColor(QTreeWidgetItem *item, const Tag &tag);
void handleTreeWidgetItemTagColor(QTreeWidgetItem *item, int tag);
bool enableDockWidgetQuestion(QDockWidget *dockWidget);
void fixDarkModeIcons(QWidget *widget);
QAction *findActionByData(QMenu *menu, const QVariant &data);
void applyInterfaceStyle(QString interfaceStyle = QString());
}    // namespace Gui
}    // namespace Utils
