/*
 * Copyright (c) 2014-2018 Patrizio Bekerle -- http://www.bekerle.com
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

#include <QTextBlock>
#include <QDebug>
#include <libraries/qmarkdowntextedit/markdownhighlighter.h>
#include "navigationwidget.h"


NavigationWidget::NavigationWidget(QWidget *parent)
    : QTreeWidget(parent) {

    QObject::connect(
            this,
            SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this,
            SLOT(onCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
}

/**
 * Sets a document to parse
 */
void NavigationWidget::setDocument(QTextDocument *document) {
    _document = document;
}

/**
 * Emits the positionClicked signal to jump to the clicked navigation item's
 * position
 */
void NavigationWidget::onCurrentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    if (current == NULL) {
        return;
    }

    emit positionClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Parses a text document and builds the navigation tree for it
 */
void NavigationWidget::parse(QTextDocument *document) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker);

    setDocument(document);
    clear();
    _lastHeadingItemList.clear();

    for (int i = 0; i < document->blockCount(); i++) {
        QTextBlock block = document->findBlockByNumber(i);
        int elementType = block.userState();
        QString text = block.text();

        // check for unrecognized headlines, like `# Header [link](http://url)`
//        if (text.startsWith("#") && elementType != -1) {
//            QRegularExpressionMatch match =
//                    QRegularExpression("^(#+)").match(text);
//
//            if (match.hasMatch()) {
//                // override the element type
//                elementType = MarkdownHighlighter::H1 +
//                        match.captured(1).count() - 1;
//            }
//        }

        // ignore all non headline types
        if ((elementType < MarkdownHighlighter::H1) ||
                (elementType > MarkdownHighlighter::H6)) {
            continue;
        }

        text.remove(QRegularExpression("^#+"))
                .remove(QRegularExpression("#+$"))
                .remove(QRegularExpression("^\\s+"))
                .remove(QRegularExpression("^=+$"))
                .remove(QRegularExpression("^-+$"));

        if (text.isEmpty()) {
            continue;
        }

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, text);
        item->setData(0, Qt::UserRole, block.position());
        item->setToolTip(0, tr("headline %1").arg(
                elementType - MarkdownHighlighter::H1 + 1));

        // attempt to find a suitable parent item for the element type
        QTreeWidgetItem *lastHigherItem = findSuitableParentItem(elementType);

        if (lastHigherItem == NULL) {
            // if there wasn't a last higher level item then add the current
            // item to the top level
            addTopLevelItem(item);
        } else {
            // if there was a last higher level item then add the current
            // item as child of that item
            lastHigherItem->addChild(item);
        }

        _lastHeadingItemList[elementType] = item;
    }

    expandAll();
}

/**
 * Attempts to find a suitable parent item for the element type
 */
QTreeWidgetItem * NavigationWidget::findSuitableParentItem(int elementType) {
    elementType--;
    QTreeWidgetItem *lastHigherItem = _lastHeadingItemList[elementType];

    return ((lastHigherItem == NULL) &&
            (elementType > MarkdownHighlighter::H1)) ?
           findSuitableParentItem(elementType) : lastHigherItem;
}
