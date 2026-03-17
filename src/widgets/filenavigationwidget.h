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

#pragma once

#include <QPoint>
#include <QString>
#include <QTreeWidget>
#include <QVector>

class QKeyEvent;
class QTextDocument;
class QTreeWidgetItem;

class FileNavigationWidget : public QTreeWidget {
    Q_OBJECT

   public:
    enum class FileItemType { MediaFile, AttachmentFile };

    struct FileLinkNode {
        QString text;
        int pos;
        FileItemType type;
        QString filePath;

        bool operator==(const FileLinkNode &node) const {
            return text == node.text && pos == node.pos && type == node.type &&
                   filePath == node.filePath;
        }
    };

    explicit FileNavigationWidget(QWidget *parent = 0);

    void parse(const QTextDocument *document, int textCursorPosition);
    void setFileLinkNodes(const QVector<FileLinkNode> &nodes, int textCursorPosition);
    void selectItemForCursorPosition(int position);
    bool hasFileLinks() const;
    static QVector<FileLinkNode> parseText(const QString &text, const QString &mediaPath,
                                           const QString &attachmentsPath);

   private slots:
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onItemClicked(QTreeWidgetItem *current, int column);
    void onItemChanged(QTreeWidgetItem *item, int column);
    void showContextMenu(const QPoint &pos);

   signals:
    void positionClicked(int position);

   private:
    void keyPressEvent(QKeyEvent *event) override;
    void buildTree(const QVector<FileLinkNode> &nodes);
    void emitPositionForItem(const QTreeWidgetItem *item);
    bool deleteSelectedLinkedFiles();
    bool renameLinkedFile(QTreeWidgetItem *item, const QString &oldFileName,
                          const QString &newFileName, FileItemType type);
    void refreshFromCurrentNote();

    QVector<FileLinkNode> _fileLinkNodes;
    int _cursorPosition = 0;
};
