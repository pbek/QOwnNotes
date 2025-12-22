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
 */

#include "backlinkwidget.h"

#include <QTreeWidgetItem>

#include "entities/note.h"
#include "entities/notefolder.h"
#include "utils/gui.h"

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

    emit noteClicked(current->data(0, Qt::UserRole).toInt(),
                     current->data(0, Qt::UserRole + 1).toString());
}

/**
 * Emits the noteClicked signal to jump to the clicked navigation item's note
 */
void BacklinkWidget::onItemClicked(QTreeWidgetItem *current, int column) {
    Q_UNUSED(column)

    if (current == nullptr) {
        return;
    }

    emit noteClicked(current->data(0, Qt::UserRole).toInt(),
                     current->data(0, Qt::UserRole + 1).toString());
}

/**
 * Find the backlinks of the document
 */
void BacklinkWidget::findBacklinks(Note note) {
    clear();
    auto reverseLinkNotes = note.findReverseLinkNotes();

    // Iterate over reverseLinkNotes
    for (auto it = reverseLinkNotes.begin(); it != reverseLinkNotes.end(); ++it) {
        const Note &backlinkNote = it.key();
        const QSet<LinkHit> &linkTextList = it.value();

        auto *topItem = new QTreeWidgetItem();

        topItem->setText(0, backlinkNote.getName());
        Utils::Gui::setTreeWidgetItemToolTipForNote(topItem, backlinkNote);
        // Disable selection for the top items
        topItem->setFlags(topItem->flags() & ~Qt::ItemIsSelectable);

        addTopLevelItem(topItem);

        for (const LinkHit &linkHit : linkTextList) {
            auto *item = new QTreeWidgetItem();

            item->setText(0, linkHit.text);
            item->setData(0, Qt::UserRole, backlinkNote.getId());
            item->setData(0, Qt::UserRole + 1, linkHit.markdown);
            item->setToolTip(0, tr("Open note and find <code>%1</code>")
                                    .arg(Utils::Misc::htmlspecialchars(linkHit.markdown)));

            topItem->addChild(item);
        }
    }

    expandAll();
}
