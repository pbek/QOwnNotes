/*
 * Copyright (c) 2014-2021 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "navigationwidget.h"

#include <libraries/qmarkdowntextedit/markdownhighlighter.h>

#include <QDebug>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocument>
#include <QTreeWidgetItem>
#include <QtConcurrent/QtConcurrent>

NavigationWidget::NavigationWidget(QWidget *parent)
    : QTreeWidget(parent), _document(nullptr) {
    QObject::connect(this, &NavigationWidget::currentItemChanged, this,
                     &NavigationWidget::onCurrentItemChanged);

    _parseFutureWatcher = new QFutureWatcher<QVector<Node>>(this);
    connect(_parseFutureWatcher, &QFutureWatcher<QVector<Node>>::finished, this,
            &NavigationWidget::onParseCompleted);
}

NavigationWidget::~NavigationWidget() {
    this->_parseFutureWatcher->waitForFinished();
}

/**
 * Sets a document to parse
 */
void NavigationWidget::setDocument(const QTextDocument *document) {
    _document = document;
}

/**
 * Emits the positionClicked signal to jump to the clicked navigation item's
 * position
 */
void NavigationWidget::onCurrentItemChanged(QTreeWidgetItem *current,
                                            QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    emit positionClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Parses a text document and builds the navigation tree for it
 */
void NavigationWidget::parse(const QTextDocument *document) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    setDocument(document);

    const QFuture<QVector<Node>> future =
        QtConcurrent::run(&NavigationWidget::parseDocument, document);
    this->_parseFutureWatcher->setFuture(future);
}

QVector<Node> NavigationWidget::parseDocument(
    const QTextDocument *const document) {
    QVector<Node> nodes;
    for (int i = 0; i < document->blockCount(); ++i) {
        const QTextBlock &block = document->findBlockByNumber(i);
        const int elementType = block.userState();

        // ignore all non headline types
        if ((elementType < MarkdownHighlighter::H1) ||
            (elementType > MarkdownHighlighter::H6)) {
            continue;
        }
        QString text =
            block.text().remove(QRegularExpression(QStringLiteral("^#+\\s+")));

        if (text.isEmpty()) {
            continue;
        }

        nodes.append({std::move(text), block.position(), elementType});
    }
    return nodes;
}

void NavigationWidget::onParseCompleted() {
    QVector<Node> nodes = this->_parseFutureWatcher->result();
    if (_navigationTreeNodes == nodes) return;
    _navigationTreeNodes = std::move(nodes);

    clear();
    _lastHeadingItemList.clear();

    for (const auto &node : _navigationTreeNodes) {
        const int elementType = node.elementType;
        const int pos = node.pos;

        auto *item = new QTreeWidgetItem();
        item->setText(0, node.text);
        item->setData(0, Qt::UserRole, pos);
        item->setToolTip(
            0,
            tr("headline %1").arg(elementType - MarkdownHighlighter::H1 + 1));

        // attempt to find a suitable parent item for the element type
        QTreeWidgetItem *lastHigherItem = findSuitableParentItem(elementType);

        if (lastHigherItem == nullptr) {
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
QTreeWidgetItem *NavigationWidget::findSuitableParentItem(
    int elementType) const {
    --elementType;
    auto lastHigherItem = _lastHeadingItemList.value(elementType);

    return (lastHigherItem == nullptr && elementType > MarkdownHighlighter::H1)
               ? findSuitableParentItem(elementType)
               : lastHigherItem;
}
