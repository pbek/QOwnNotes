/*
 * Copyright (c) 2014-2017 Patrizio Bekerle -- http://www.bekerle.com
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
                                           QString text,
                                           TreeWidgetSearchFlags searchFlags) {
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
                    // search for text in the columns
                    show |= item->text(index).contains(
                            text, Qt::CaseInsensitive);

                    // also show the item if the text was found in the tooltip
                    if (searchFlags & TreeWidgetSearchFlag::TooltipSearch) {
                        show |= item->toolTip(index).contains(
                                text, Qt::CaseInsensitive);
                    }
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
