/*
 * Copyright (C) 2016 Patrizio Bekerle -- http://www.bekerle.com
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
        // search for items
        QList<QTreeWidgetItem*> foundItems = treeWidget->
                findItems(text, Qt::MatchContains | Qt::MatchRecursive);

        // hide all not found items
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                bool show = foundItems.contains(item);

                // also show the item if the text was found in the tooltip
                if (searchFlags & TreeWidgetSearchFlags::TooltipSearch) {
                    show |= item->toolTip(0).contains(text,
                                                      Qt::CaseInsensitive);
                }

                // also show the item if an integer id is greater than 0
                if (searchFlags & TreeWidgetSearchFlags::IntCheck) {
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
