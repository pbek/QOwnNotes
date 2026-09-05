#pragma once

#include <entities/notesubfolder.h>

#include "masterdialog.h"

namespace Ui {
class JoplinImportDialog;
}

class Note;
class QTreeWidgetItem;
class QFile;

class JoplinImportDialog : public MasterDialog {
    Q_OBJECT

    struct MediaFileData {
        QString data;
        QString suffix;
        QString fileName;
    };

   public:
    explicit JoplinImportDialog(QWidget* parent = 0);
    ~JoplinImportDialog() override;

    int getImportCount() const;
   private slots:
    void on_directoryButton_clicked();

    void on_importButton_clicked();

   private:
    Ui::JoplinImportDialog* ui;
    int _importCount;
    QString _dirPath;
    QHash<QString, QString> _noteData;
    QHash<QString, QString> _folderData;
    QHash<QString, QString> _tagData;
    QHash<QString, QStringList> _tagAssignmentData;
    QHash<QString, QString> _imageData;
    QHash<QString, QString> _attachmentData;
    QHash<QString, NoteSubFolder> _importedFolders;
    // Resource id -> attachments-folder filename already written for it this
    // import run, so a resource referenced more than once (e.g. once as an
    // inline attachment link and again elsewhere in the same or a later
    // note) reuses the existing copy instead of writing another
    // byte-identical "<id>-1.ext" duplicate.
    QHash<QString, QString> _importedAttachmentFileNames;

    bool importNote(const QString& id, const QString& text, const QString& dirPath);
    static void applyJoplinTimestamps(const QString& text, Note& note);
    void tagNote(const QString& id, const Note& note);
    void handleImages(Note& note, const QString& dirPath);
    void handleAttachments(Note& note, const QString& dirPath);
    bool importFolders();
    NoteSubFolder importFolder(const QString& id, const QString& text);
    void importImage(Note& note, const QString& dirPath, QString& noteText, const QString& imageTag,
                     const QString& imageId, const QString& imageName = "");
    static QFile* findResourceFile(const QString& dirPath, const QString& id,
                                   const QString& metaData);
};
