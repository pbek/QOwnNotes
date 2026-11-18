#pragma once

#include <QPixmap>
#include <QPointer>
#include <QTreeWidget>
#include <QVector>

class QDrag;

// Shared drag support for the note list and the note subfolder panel.
class NoteDragTreeWidget : public QTreeWidget {
    Q_OBJECT

   public:
    explicit NoteDragTreeWidget(QWidget *parent = nullptr);

   signals:
    void saveRequested();
    void moveRequested(const QVector<int> &noteIds, const QVector<int> &folderIds,
                       int destinationFolderId);
    void copyRequested(const QVector<int> &noteIds, const QVector<int> &folderIds,
                       int destinationFolderId);

   protected:
    void setInternalDragCursor(QDrag *drag, Qt::KeyboardModifiers modifiers);
    bool canDropItems(const QMimeData *data, int destinationFolderId) const;
    Qt::DropAction chooseDropAction(Qt::KeyboardModifiers modifiers, const QPoint &globalPosition);
    QStringList mimeTypes() const override;
    Qt::DropActions supportedDropActions() const override;
    bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data,
                      Qt::DropAction action) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMimeData *mimeData(const QList<QTreeWidgetItem *> &items) const override;
#else
    QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const override;
#endif
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

   private:
    QPointer<QDrag> _activeDrag;
    QPixmap _moveCursor;
    void updateDragCursor(const QDropEvent *event);
    int destinationFolderId(const QPoint &position) const;
    bool acceptsDrag(const QDropEvent *event) const;
};
