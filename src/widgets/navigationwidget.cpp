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

#include "navigationwidget.h"

#include <libraries/qmarkdowntextedit/markdownhighlighter.h>

#include <QDebug>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocument>
#include <QTreeWidgetItem>

NavigationWidget::NavigationWidget(QWidget *parent) : QTreeWidget(parent) {
    // we want to handle currentItemChanged because it also works with the keyboard
    QObject::connect(this, &NavigationWidget::currentItemChanged, this,
                     &NavigationWidget::onCurrentItemChanged);
    // we want to handle itemClicked because it allows to click on an item a 2nd time
    QObject::connect(this, &NavigationWidget::itemClicked, this, &NavigationWidget::onItemClicked);
}

/**
 * Emits the positionClicked signal to jump to the changed navigation item's
 * position
 */
void NavigationWidget::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    emit positionClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Emits the positionClicked signal to jump to the clicked navigation item's
 * position
 */
void NavigationWidget::onItemClicked(QTreeWidgetItem *current, int column) {
    Q_UNUSED(column)

    if (current == nullptr) {
        return;
    }

    emit positionClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Parses a text document and builds the navigation tree for it
 */
void NavigationWidget::parse(const QTextDocument *document, int textCursorPosition) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    _doc = document;
    _cursorPosition = textCursorPosition;
    doParse();
}

void NavigationWidget::doParse() {
    const auto nodes = parseDocument(_doc);
    buildNavTree(nodes);
}

QVector<Node> NavigationWidget::parseDocument(const QTextDocument *const document) {
    QVector<Node> nodes;
    for (int i = 0; i < document->blockCount(); ++i) {
        const QTextBlock block = document->findBlockByNumber(i);
        if (!block.isValid()) {
            continue;
        }
        const int elementType = block.userState();

        // ignore all non headline types
        if ((elementType < MarkdownHighlighter::H1) || (elementType > MarkdownHighlighter::H6)) {
            continue;
        }
        static const QRegularExpression re(QStringLiteral("^#+\\s+"));
        // Trim the heading text, in case there are trailing carriage return characters leaking in
        // Windows
        QString text = block.text().remove(re).trimmed();

        if (text.isEmpty()) {
            continue;
        }

        nodes.append({std::move(text), block.position(), elementType});
    }
    return nodes;
}

void NavigationWidget::selectItemForCursorPosition(int position) {
    int itemIndex = findItemIndexForCursorPosition(position);

    QTreeWidgetItem *itemToSelect{nullptr};
    if (itemIndex >= 0) {
        QTreeWidgetItemIterator it(this);
        it += itemIndex;
        itemToSelect = *it;
    }

    QSignalBlocker b(this);
    setCurrentItem(itemToSelect);
}

int NavigationWidget::findItemIndexForCursorPosition(int position) const {
    auto fwdIt =
        std::lower_bound(_navigationTreeNodes.begin(), _navigationTreeNodes.end(), position,
                         [](const Node &node, int position) { return node.pos <= position; });

    return fwdIt - std::begin(_navigationTreeNodes) - 1;
}

QString NavigationWidget::stripMarkdown(const QString &input) {
    // Regular expressions for different Markdown syntax patterns
    static const QRegularExpression boldRegex(R"(\*{2}([^*]+)\*{2})");      // **bold**
    static const QRegularExpression italicRegex(R"(\*{1}([^*]+)\*{1})");    // *italic*
    static const QRegularExpression strikethroughRegex(
        R"(\~{2}([^~]+)\~{2})");                                               // ~~strikethrough~~
    static const QRegularExpression linkRegex(R"(\[([^]]+)\]\(([^)]+)\))");    // [link](url)
    static const QRegularExpression angleBracketLinkRegex(R"(<([^>]+)>)");     // <http://link>
    static const QRegularExpression codeRegex(R"(`([^`]+)`+)");                // `code`

    // Replace each Markdown pattern with an empty string
    QString strippedText = input;
    // boldRegex goes first, because italicRegex needs to match the remaining "*bold*" pattern
    strippedText.replace(boldRegex, "\\1");
    strippedText.replace(italicRegex, "\\1");
    strippedText.replace(strikethroughRegex, "\\1");
    strippedText.replace(linkRegex, "\\1");
    strippedText.replace(angleBracketLinkRegex, "\\1");
    strippedText.replace(codeRegex, "\\1");

    return strippedText;
}

void NavigationWidget::buildNavTree(const QVector<Node> &nodes) {
    if (_navigationTreeNodes == nodes) return;

    _navigationTreeNodes = std::move(nodes);

    clear();
    _lastHeadingItemList.clear();

    for (const auto &node : _navigationTreeNodes) {
        const int elementType = node.elementType;
        const int pos = node.pos;

        auto *item = new QTreeWidgetItem();

        // Strip out Markdown syntax from the headline text
        item->setText(0, stripMarkdown(node.text));
        item->setData(0, Qt::UserRole, pos);
        item->setToolTip(0, tr("headline %1").arg(elementType - MarkdownHighlighter::H1 + 1));

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
    selectItemForCursorPosition(_cursorPosition);
}

/**
 * Attempts to find a suitable parent item for the element type
 */
QTreeWidgetItem *NavigationWidget::findSuitableParentItem(int elementType) const {
    --elementType;
    auto lastHigherItem = _lastHeadingItemList.value(elementType);

    return (lastHigherItem == nullptr && elementType > MarkdownHighlighter::H1)
               ? findSuitableParentItem(elementType)
               : lastHigherItem;
}
