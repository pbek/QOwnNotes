#pragma once

#include <QXmlStreamReader>

#include "masterdialog.h"

namespace Ui {
class EvernoteImportDialog;
}

class Note;
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
    QHash<QString, MediaFileData> _mediaFileDataHash;
    QHash<QString, MediaFileData> _attachmentFileDataHash;
    QHash<QString, QString> _metaDataAttributeHash;
    QString _metaDataTableText;

    void setupMetaDataTreeWidgetItems();

    void storeMetaDataTreeWidgetItemsCheckedState();

    void initNoteCount(const QString &data);

    QString getMarkdownForMediaFileData(Note note, const MediaFileData &mediaFileData);

    QString getMarkdownForAttachmentFileData(Note note, const MediaFileData &mediaFileData);

    QTreeWidgetItem *addMetaDataTreeWidgetItem(const QString &name,
                                               const QString &attributeName = QString(),
                                               QTreeWidgetItem *parentItem = nullptr);

    bool isMetaDataChecked();
    void importNotes(QXmlStreamReader &xml);
    Note parseNote(QXmlStreamReader &xml, bool importMetaData = false);
    void parseResource(QXmlStreamReader &xml);
    void importImages(const Note &note, QString &content);
    void importAttachments(const Note &note, QString &content);
    void parseMetaDataItem(QXmlStreamReader &xml, bool isNoteAttribute = false);
    void determineMetaDataAttributes();
    QString compileMetaDataText();
    void parseNoteAttributes(QXmlStreamReader &xml);
    void tagNote(Note &note, QStringList &tagNames);
    void resetNoteCount();
};
