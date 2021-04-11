#pragma once

#include "masterdialog.h"

namespace Ui {
class EvernoteImportDialog;
}

class Note;
class QXmlQuery;
class QTreeWidgetItem;

class EvernoteImportDialog : public MasterDialog {
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

    void setupMetaDataTreeWidgetItems();

    void storeMetaDataTreeWidgetItemsCheckedState();

    void initNoteCount(const QString &data);

    QString getMarkdownForMediaFileData(Note note,
                                        const MediaFileData &mediaFileData);

    QString getMarkdownForAttachmentFileData(
        Note note, const MediaFileData &mediaFileData);

    QTreeWidgetItem *addMetaDataTreeWidgetItem(
        const QString &name, const QString &attributeName = QString(),
        QTreeWidgetItem *parentItem = nullptr);

    bool isMetaDataChecked();


    /** These require xml patterns **/
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    void importNotes(const QString &data);

    int countNotes(const QString &data);

    QString importImages(const Note &note, QString content, QXmlQuery query);

    void tagNote(QXmlQuery &query, Note &note);

    QString importAttachments(const Note &note, QString content,
                              QXmlQuery query);

    QString generateMetaDataMarkdown(QXmlQuery query);
#endif
};
