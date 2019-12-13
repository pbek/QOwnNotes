#pragma once

#include <QDialog>
#include <QXmlQuery>
#include <entities/note.h>
#include <QtWidgets/QTreeWidgetItem>
#include "masterdialog.h"

namespace Ui {
class EvernoteImportDialog;
}

class EvernoteImportDialog : public MasterDialog
{
    Q_OBJECT

    struct MediaFileData {
        QString data;
        QString suffix;
        QString fileName;
    };

public:
    explicit EvernoteImportDialog(QWidget *parent = 0);
    ~EvernoteImportDialog();

    int getImportCount();

private slots:
    void on_fileButton_clicked();

    void on_importButton_clicked();

private:
    Ui::EvernoteImportDialog *ui;
    int _importCount;

    void importNotes(const QString& data);

    int countNotes(const QString& data);

    void initNoteCount(const QString &data);

    QString importImages(const Note &note, QString content, QXmlQuery query);

    QString getMarkdownForMediaFileData(Note note, const MediaFileData &mediaFileData);

    QString getMarkdownForAttachmentFileData(Note note, const MediaFileData &mediaFileData);

    void tagNote(QXmlQuery &query, Note &note);

    QString importAttachments(const Note &note, QString content, QXmlQuery query);

    QTreeWidgetItem *addMetaDataTreeWidgetItem(
            const QString& name,
            const QString& attributeName = QString(),
            QTreeWidgetItem *parentItem = nullptr);

    void setupMetaDataTreeWidgetItems();

    bool isMetaDataChecked();

    QString generateMetaDataMarkdown(QXmlQuery query);

    void storeMetaDataTreeWidgetItemsCheckedState();
};
