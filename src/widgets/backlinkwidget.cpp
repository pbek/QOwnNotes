/*
 * Copyright (c) 2014-2024 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "backlinkwidget.h"
#include "entities/note.h"

#include <QTreeWidgetItem>

BacklinkWidget::BacklinkWidget(QWidget *parent) : QTreeWidget(parent) {
    // we want to handle currentItemChanged because it also works with the keyboard
    QObject::connect(this, &BacklinkWidget::currentItemChanged, this,
                     &BacklinkWidget::onCurrentItemChanged);
    // we want to handle itemClicked because it allows to click on an item a 2nd time
    QObject::connect(this, &BacklinkWidget::itemClicked, this, &BacklinkWidget::onItemClicked);
}

/**
 * Emits the noteClicked signal to jump to the changed navigation item's note
 */
void BacklinkWidget::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    emit noteClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Emits the noteClicked signal to jump to the clicked navigation item's note
 */
void BacklinkWidget::onItemClicked(QTreeWidgetItem *current, int column) {
    Q_UNUSED(column)

    if (current == nullptr) {
        return;
    }

    emit noteClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Find the backlinks of the document
 */
void BacklinkWidget::findBacklinks(Note note) {
    clear();
    auto notes = note.findBacklinks();

    qDebug() << __func__ << " - 'notes count': " << notes.count();

    foreach (const Note &backlinkNote, notes) {
        auto *item = new QTreeWidgetItem();

        item->setText(0, backlinkNote.getName());
        item->setData(0, Qt::UserRole, backlinkNote.getId());
        item->setToolTip(0, tr("Open note"));

        addTopLevelItem(item);
    }

    expandAll();
}
