/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#pragma once

#include <QFutureWatcher>
#include <QTreeWidget>

#include "entities/note.h"

class QTreeWidgetItem;

class BacklinkWidget : public QTreeWidget {
    Q_OBJECT

   public:
    explicit BacklinkWidget(QWidget *parent = 0);

    void findBacklinks(Note note);

   private slots:
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onItemClicked(QTreeWidgetItem *current, int column);

   signals:
    void noteClicked(int position, const QString &markdown);
};
