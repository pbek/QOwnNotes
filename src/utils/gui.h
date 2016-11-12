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

#pragma once

#include <QTreeWidgetItem>

/*  Gui functions that can be useful */

namespace Utils {
    namespace Gui {
        enum TreeWidgetSearchFlags {
            None = 0,

            // also show the item if an integer id is greater than 0
            IntCheck = 1,

            // also show the item if the text was found in the tooltip
            TooltipSearch =2,

            // search in all columns
            AllColumnsSearch =4
        };

        bool isOneTreeWidgetItemChildVisible(QTreeWidgetItem *item);

        void searchForTextInTreeWidget(
                QTreeWidget *treeWidget, QString text,
                TreeWidgetSearchFlags searchFlags =
                TreeWidgetSearchFlags::None);
    }
}
