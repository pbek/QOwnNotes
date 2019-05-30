/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
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

#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QFontDialog>
#include <QPlainTextEdit>
#include <QTextBlock>

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

        bool isOneTreeWidgetItemChildVisible(QTreeWidgetItem *item);

        void searchForTextInTreeWidget(
                QTreeWidget *treeWidget, const QString& text,
                TreeWidgetSearchFlags searchFlags = None);

        QMessageBox::StandardButton showMessageBox(
                QWidget *parent, QMessageBox::Icon icon,
                const QString &title, const QString &text,
                const QString &identifier,
                QMessageBox::StandardButtons buttons,
                QMessageBox::StandardButton defaultButton);

        QMessageBox::StandardButton information(
                QWidget *parent, const QString &title, const QString &text,
                const QString &identifier = "default",
                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

        QMessageBox::StandardButton question(
                QWidget *parent, const QString &title, const QString &text,
                const QString &identifier = "default",
                QMessageBox::StandardButtons buttons =
                QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

        QMessageBox::StandardButton warning(
                QWidget *parent, const QString &title, const QString &text,
                const QString &identifier = "default",
                QMessageBox::StandardButtons buttons =
                QMessageBox::StandardButtons(QMessageBox::Ok),
                QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

        bool userDataInTreeWidgetExists(QTreeWidget *treeWidget,
                                        const QVariant& userData,
                                        int column = 0);

        QTreeWidgetItem *getTreeWidgetItemWithUserData(
                QTreeWidget *treeWidget, const QVariant &userData,
                int column = 0);

        void resetBoldStateOfAllTreeWidgetItems(
                QTreeWidget *treeWidget, int column = 0);

        bool isMessageBoxPresent();

        QFont fontDialogGetFont(
                bool *ok, const QFont &initial, QWidget *parent = nullptr,
                const QString &title = QString(),
                QFontDialog::FontDialogOptions options =
                        QFontDialog::FontDialogOptions());

        void copyCodeBlockText(const QTextBlock& initialBlock);

        bool autoFormatTableAtCursor(QPlainTextEdit *textEdit);

        void updateInterfaceFontSize(int fontSize = -1);
    }
}
