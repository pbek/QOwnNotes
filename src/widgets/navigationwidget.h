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
        return text == node.text && pos == node.pos &&
               elementType == node.elementType;
    }
};

class NavigationWidget : public QTreeWidget {
    Q_OBJECT

   public:
    explicit NavigationWidget(QWidget *parent = 0);
    ~NavigationWidget();

    void parse(const QTextDocument *document);
    void setDocument(const QTextDocument *document);
    static QVector<Node> parseDocument(const QTextDocument *const document);

   private slots:
    void onCurrentItemChanged(QTreeWidgetItem *current,
                              QTreeWidgetItem *previous);
    void onParseCompleted();

   signals:
    void positionClicked(int position);

   private:
    const QTextDocument *_document;
    QHash<int, QTreeWidgetItem *> _lastHeadingItemList;
    QFutureWatcher<QVector<Node>> *_parseFutureWatcher;
    QVector<Node> _navigationTreeNodes;

    QTreeWidgetItem *findSuitableParentItem(int elementType) const;
};
