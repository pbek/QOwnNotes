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

class QTextDocument;
class QTreeWidgetItem;

struct Node {
    QString text;
    int pos;
    int elementType;

    bool operator==(const Node &node) const {
        return text == node.text && pos == node.pos && elementType == node.elementType;
    }
};

class NavigationWidget : public QTreeWidget {
    Q_OBJECT

   public:
    explicit NavigationWidget(QWidget *parent = 0);

    void parse(const QTextDocument *document, int textCursorPosition);
    static QVector<Node> parseDocument(const QTextDocument *const document);

    void selectItemForCursorPosition(int position);

   private slots:
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onItemClicked(QTreeWidgetItem *current, int column);

   private:
    void buildNavTree(const QVector<Node> &nodes);
    void doParse();

   signals:
    void positionClicked(int position);

   private:
    const QTextDocument *_doc;
    QHash<int, QTreeWidgetItem *> _lastHeadingItemList;
    QVector<Node> _navigationTreeNodes;
    int _cursorPosition;

    QTreeWidgetItem *findSuitableParentItem(int elementType) const;
    int findItemIndexForCursorPosition(int position) const;
    static QString stripMarkdown(const QString &input);
};
