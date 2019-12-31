/*
 * Copyright (c) 2014-2020 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextDocument>

class NavigationWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit NavigationWidget(QWidget *parent = 0);
    void parse(QTextDocument *document);
    void setDocument(QTextDocument *document);

private slots:
    void onCurrentItemChanged(
            QTreeWidgetItem *current, QTreeWidgetItem *previous);
signals:
    void positionClicked(int position);

private:
    QTextDocument *_document;
    QMap<int, QTreeWidgetItem *> _lastHeadingItemList;

    QTreeWidgetItem *findSuitableParentItem(int elementType);
};
