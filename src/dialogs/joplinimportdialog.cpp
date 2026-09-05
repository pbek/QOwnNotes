#include "joplinimportdialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/tag.h>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "filedialog.h"
#include "services/settingsservice.h"
#include "ui_joplinimportdialog.h"

JoplinImportDialog::JoplinImportDialog(QWidget* parent)
    : MasterDialog(parent), ui(new Ui::JoplinImportDialog) {
    ui->setupUi(this);
    afterSetupUI();

    ui->progressBar->hide();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    _importCount = 0;

    SettingsService settings;
    bool showFolders = NoteFolder::isCurrentShowSubfolders();

    if (!showFolders) {
        ui->folderImportCheckBox->setToolTip(
            QObject::tr("Turned on note subfolders for the current note folder to be "
                        "able to import folders."));
    }

    ui->folderImportCheckBox->setEnabled(showFolders);
    ui->folderImportCheckBox->setChecked(
        settings.value(QStringLiteral("JoplinImport/FolderImportCheckBoxChecked"), true).toBool() &&
        showFolders);
    ui->tagImportCheckBox->setChecked(
        settings.value(QStringLiteral("JoplinImport/TagImportCheckBoxChecked"), true).toBool());
    ui->metadataImportCheckBox->setChecked(
        settings.value(QStringLiteral("JoplinImport/MetadataImportCheckBoxChecked"), true)
            .toBool());
    ui->imageImportCheckBox->setChecked(
        settings.value(QStringLiteral("JoplinImport/ImageImportCheckBoxChecked"), true).toBool());
    ui->attachmentImportCheckBox->setChecked(
        settings.value(QStringLiteral("JoplinImport/AttachmentImportCheckBoxChecked"), true)
            .toBool());

    // Load the last selected directory
    QString lastDirectory = settings.value(QStringLiteral("JoplinImport/LastDirectory")).toString();
    if (!lastDirectory.isEmpty()) {
        QDir dir(lastDirectory);
        if (dir.exists()) {
            ui->directoryLineEdit->setText(lastDirectory);
        }
    }
}

JoplinImportDialog::~JoplinImportDialog() {
    SettingsService settings;
    settings.setValue(QStringLiteral("JoplinImport/FolderImportCheckBoxChecked"),
                      ui->folderImportCheckBox->isChecked());
    settings.setValue(QStringLiteral("JoplinImport/TagImportCheckBoxChecked"),
                      ui->tagImportCheckBox->isChecked());
    settings.setValue(QStringLiteral("JoplinImport/MetadataImportCheckBoxChecked"),
                      ui->metadataImportCheckBox->isChecked());
    settings.setValue(QStringLiteral("JoplinImport/ImageImportCheckBoxChecked"),
                      ui->imageImportCheckBox->isChecked());
    settings.setValue(QStringLiteral("JoplinImport/AttachmentImportCheckBoxChecked"),
                      ui->attachmentImportCheckBox->isChecked());

    // Save the last selected directory
    QString currentDirectory = ui->directoryLineEdit->text();
    if (!currentDirectory.isEmpty()) {
        settings.setValue(QStringLiteral("JoplinImport/LastDirectory"), currentDirectory);
    }

    delete ui;
}

/**
 * Imports the notes from a selected file
 */
void JoplinImportDialog::on_directoryButton_clicked() {
    FileDialog dialog(QStringLiteral("Joplin Import"));
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowTitle(tr("Select 'RAW - Joplin Export Directory' to import"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString directoryName = dialog.selectedFile();
        QDir d = QDir(directoryName);

        if (d.exists() && !directoryName.isEmpty()) {
            ui->directoryLineEdit->setText(directoryName);
        }
    }
}

/**
 * Reads the files and imports the notes
 */
void JoplinImportDialog::on_importButton_clicked() {
    _importCount = 0;
    _noteData.clear();
    _tagData.clear();
    _tagAssignmentData.clear();
    _imageData.clear();
    _attachmentData.clear();
    _folderData.clear();
    _importedFolders.clear();
    _importedAttachmentFileNames.clear();

    auto directoryName = ui->directoryLineEdit->text();
    QDir dir(directoryName);

    if (directoryName.isEmpty() || !dir.exists()) {
        return;
    }

    // A Joplin resource referenced by more than one image tag (e.g. the same
    // picture used twice) would otherwise pop up a "use existing file?"
    // dialog once per repeat, or -- unanswered -- fall back to writing a
    // byte-identical "<id>-1.ext" duplicate (see
    // Note::getInsertMediaMarkdown()). Force it to reuse the existing file
    // for the duration of this import, then restore whatever the user had
    // configured.
    SettingsService importSettings;
    const QString reuseImageOverrideKey =
        QStringLiteral("MessageBoxOverride/insert-media-use-existing-image");
    const QVariant previousReuseImageOverride = importSettings.value(reuseImageOverrideKey);
    importSettings.setValue(reuseImageOverrideKey, QMessageBox::Yes);

    QStringList files = dir.entryList(QStringList() << "*.md", QDir::Files);
    _dirPath = dir.path();
    ui->importButton->setEnabled(false);
    ui->progressBar->setMaximum(files.count());
    ui->progressBar->show();

    foreach (QString fileName, files) {
        QCoreApplication::processEvents();
        QString filePath = dir.path() + "/" + fileName;
        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << file.errorString() << " (path: " << directoryName << ")";
            continue;
        }

        const QString text = file.readAll();
        auto textLines = text.split(QRegularExpression(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));
        const auto id = fileName.remove(QRegularExpression("\\.md$"));

        // check if text contains a note
        if (text.contains(QRegularExpression("^type_: 1$", QRegularExpression::MultilineOption))) {
            _noteData[id] = text;

            // don't increase the progress bar yet
            continue;
        }
        // check if text contains a folder
        if (text.contains(QRegularExpression("^type_: 2$", QRegularExpression::MultilineOption))) {
            _folderData[id] = text;
        }
        // check if text contains an image or attachment
        else if (text.contains(
                     QRegularExpression("^type_: 4$", QRegularExpression::MultilineOption))) {
            if (text.contains(
                    QRegularExpression("^mime: image/", QRegularExpression::MultilineOption))) {
                _imageData[id] = text;
            } else {
                _attachmentData[id] = text;
            }
        }
        // check if text contains a tag
        else if (text.contains(
                     QRegularExpression("^type_: 5$", QRegularExpression::MultilineOption))) {
            _tagData[id] = textLines[0];
        }
        // check if text contains a tag assignment
        else if (text.contains(
                     QRegularExpression("^type_: 6$", QRegularExpression::MultilineOption))) {
            auto noteIdMatch =
                QRegularExpression("^note_id: (.+)$", QRegularExpression::MultilineOption)
                    .match(text);
            QString noteId;

            if (noteIdMatch.hasMatch()) {
                noteId = noteIdMatch.captured(1);
            }

            auto tagIdMatch =
                QRegularExpression("^tag_id: (.+)$", QRegularExpression::MultilineOption)
                    .match(text);
            QString tagId;

            if (tagIdMatch.hasMatch()) {
                tagId = tagIdMatch.captured(1);
            }

            _tagAssignmentData[noteId].append(tagId);
        }

        ui->progressBar->setValue(ui->progressBar->value() + 1);
        QCoreApplication::processEvents();
    }

    // import the folders first so we will not get into troubles when importing the notes
    if (ui->folderImportCheckBox->isChecked()) {
        importFolders();
    }

    // now that we have tag-, image- and attachment-data really import the notes
    QHashIterator<QString, QString> noteDataIterator(_noteData);
    while (noteDataIterator.hasNext()) {
        noteDataIterator.next();

        importNote(noteDataIterator.key(), noteDataIterator.value(), dir.path());
        _importCount++;

        ui->progressBar->setValue(ui->progressBar->value() + 1);
        QCoreApplication::processEvents();
    }

    ui->importButton->setEnabled(true);

    if (previousReuseImageOverride.isValid()) {
        importSettings.setValue(reuseImageOverrideKey, previousReuseImageOverride);
    } else {
        importSettings.remove(reuseImageOverrideKey);
    }
}

bool JoplinImportDialog::importFolders() {
    QHashIterator<QString, QString> folderDataIterator(_folderData);
    while (folderDataIterator.hasNext()) {
        folderDataIterator.next();

        importFolder(folderDataIterator.key(), folderDataIterator.value());
    }

    return true;
}

NoteSubFolder JoplinImportDialog::importFolder(const QString& id, const QString& text) {
    // return NoteSubFolder if it was already created
    if (_importedFolders.contains(id)) {
        return _importedFolders[id];
    }

    // if the text is empty, the reference most certainly wasn't found,
    // so we just set an empty note sub folder
    if (text.isEmpty()) {
        _importedFolders[id] = NoteSubFolder();

        return {};
    }

    auto parentSubFolder = NoteSubFolder();
    auto parentIdMatch =
        QRegularExpression("^parent_id: (.+)$", QRegularExpression::MultilineOption).match(text);

    // check if we need to create the parent first
    if (parentIdMatch.hasMatch()) {
        QString parentId = parentIdMatch.captured(1).trimmed();

        if (!parentId.isEmpty()) {
            parentSubFolder = importFolder(parentId, _folderData[parentId]);
        }
    }

    auto textLines = text.split(QRegularExpression(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));

    auto folderName = textLines.at(0).trimmed();

    if (folderName.isEmpty()) {
        return {};
    }

    QDir dir;
    QString path = parentSubFolder.fullPath();
    path += "/" + folderName;

    // create the folder on the disk
    if (!dir.mkpath(path)) {
        return {};
    }

    // try to fetch NoteSubFolder object
    auto noteSubFolder = NoteSubFolder::fetchByNameAndParentId(folderName, parentSubFolder.getId());

    if (noteSubFolder.getId() > 0) {
        return noteSubFolder;
    }

    // if NoteSubFolder didn't exist create it
    noteSubFolder.setName(folderName);
    noteSubFolder.setParentId(parentSubFolder.getId());
    noteSubFolder.store();
    _importedFolders[id] = noteSubFolder;

    return noteSubFolder;
}

/**
 * Handle importing of a note
 *
 * @param id
 * @param text
 * @param dirPath
 * @return
 */
bool JoplinImportDialog::importNote(const QString& id, const QString& text,
                                    const QString& dirPath) {
    auto textLines = text.split(QRegularExpression(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));

    auto title = textLines.at(0);
    title.remove(QRegularExpression("^[\\d+\\.]+"));
    auto noteText = Note::createNoteHeader(title.trimmed());
    noteText += text.trimmed();

    auto parts =
        noteText.split(QRegularExpression("^id: " + id + "$", QRegularExpression::MultilineOption));

    Note note = Note();

    // Import the note to the current note subfolder
    note.setNoteSubFolderId(NoteSubFolder::activeNoteSubFolderId());

    QString newNoteText;

    // add metadata as frontmatter
    if (ui->metadataImportCheckBox->isChecked()) {
        newNoteText = "---\nid: " + id + parts[1] + "\n---\n\n";
    }

    newNoteText += parts[0];
    note.setNoteText(newNoteText);

    if (ui->folderImportCheckBox->isChecked()) {
        auto parentIdMatch =
            QRegularExpression("^parent_id: (.+)$", QRegularExpression::MultilineOption)
                .match(text);

        // check if we need to create note sub-folders
        if (parentIdMatch.hasMatch()) {
            QString parentId = parentIdMatch.captured(1).trimmed();

            if (!parentId.isEmpty()) {
                auto noteSubFolder = importFolder(parentId, _folderData[parentId]);

                if (noteSubFolder.getId() != 0) {
                    note.setNoteSubFolder(noteSubFolder);
                }
            }
        }
    }

    // in case the user enabled that the filename can be different
    // from the note name
    // Attention: may already store the note
    note.handleNoteTextFileName();

    note.store();
    note.storeNoteTextFileToDisk();

    if (ui->tagImportCheckBox->isChecked()) {
        // tag the note if tags are found
        tagNote(id, note);
    }

    if (ui->imageImportCheckBox->isChecked()) {
        handleImages(note, dirPath);
        note.storeNoteTextFileToDisk();
    }

    if (ui->attachmentImportCheckBox->isChecked()) {
        handleAttachments(note, dirPath);
        note.storeNoteTextFileToDisk();
    }

    // Joplin's raw export carries the note's real creation/modification
    // history in its metadata block, but everything above this point wrote
    // the note file "now", so file_last_modified would otherwise reflect
    // the moment the import ran instead of the note's actual history
    // (breaking Note -> Sort by -> By date). Restore the real timestamps as
    // the last step, after every storeNoteTextFileToDisk() call above.
    applyJoplinTimestamps(text, note);

    return true;
}

/**
 * Applies a Joplin note's original updated_time metadata to the file
 * already written to disk for that note, so the app's file_last_modified
 * sorting reflects the note's real history instead of the moment the
 * import ran.
 *
 * @param text the raw Joplin note text, still containing its metadata block
 * @param note the already-stored, already-written-to-disk imported note
 */
void JoplinImportDialog::applyJoplinTimestamps(const QString& text, Note& note) {
// QFile::setFileTime() was added in Qt 5.10; this project's floor is Qt 5.5,
// so skip the correction gracefully on older Qt5 instead of failing to build.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    auto match =
        QRegularExpression("^updated_time: (.+)$", QRegularExpression::MultilineOption).match(text);

    if (!match.hasMatch()) {
        return;
    }

    // Joplin writes timestamps like "2019-11-15T16:16:34.302Z"
    const QDateTime updatedTime =
        QDateTime::fromString(match.captured(1).trimmed(), Qt::ISODateWithMs);

    if (!updatedTime.isValid()) {
        return;
    }

    // QFile::setFileTime silently fails unless the file is open
    QFile noteFile(note.fullNoteFilePath());
    if (noteFile.open(QIODevice::ReadWrite)) {
        noteFile.setFileTime(updatedTime, QFileDevice::FileModificationTime);
        noteFile.close();
    }

    // re-read the corrected mtime from disk into the note and persist it to
    // the DB's file_last_modified column, which is what sort-by-date reads
    note.updateNoteTextFromDisk();
    note.store();
#else
    Q_UNUSED(text)
    Q_UNUSED(note)
#endif
}

int JoplinImportDialog::getImportCount() const { return _importCount; }

/**
 * Handle note tagging
 * @param id
 * @param note
 */
void JoplinImportDialog::tagNote(const QString& id, const Note& note) {
    QStringList tagAssignmentData = _tagAssignmentData[id];

    foreach (QString tagId, tagAssignmentData) {
        QString tagName = _tagData[tagId];

        // create a new tag if it doesn't exist
        Tag tag = Tag::fetchByName(tagName);
        if (!tag.isFetched()) {
            tag.setName(tagName);
            tag.store();
        }

        // link the note to the tag
        if (tag.isFetched()) {
            tag.linkToNote(note);
        }
    }
}

/**
 * Handle note images
 *
 * @param note
 * @param dirPath
 */
void JoplinImportDialog::handleImages(Note& note, const QString& dirPath) {
    QString noteText = note.getNoteText();

    // Handle format: [![](:/imageId "hover text")]
    //
    // The label uses `(?:\\.|[^\]])*` instead of `[^\]]*` so that an escaped
    // bracket (`\]`) inside the alt text -- common in web-clipped or OCR'd
    // content -- doesn't prematurely end the capture and make the whole
    // pattern fail to match.
    {
        auto i =
            QRegularExpression(R"regex(!\[((?:\\.|[^\]])*)\]\(:\/([\w\d]+)\s+"([^"]*)"\))regex")
                .globalMatch(noteText);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString imageTag = match.captured(0);
            QString imageName = match.captured(1);
            QString imageId = match.captured(2);
            QString hoverText = match.captured(3);

            // Use hover text as image name if no alt text is provided
            QString finalImageName = imageName.isEmpty() ? hoverText : imageName;
            importImage(note, dirPath, noteText, imageTag, imageId, finalImageName);
        }
    }

    // Handle format: ![alt text](:/imageId)
    {
        auto i =
            QRegularExpression(R"(!\[((?:\\.|[^\]])*)\]\(:\/([\w\d]+)\))").globalMatch(noteText);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString imageTag = match.captured(0);
            QString imageName = match.captured(1);
            QString imageId = match.captured(2);

            importImage(note, dirPath, noteText, imageTag, imageId, imageName);
        }
    }

    // Handle format: <img src=":/imageId" ... />
    {
        auto i = QRegularExpression(R"(<img\s+(?:[^>]*\s+)?src=\":\/([\w\d]+)\"[^>]*\/?>)")
                     .globalMatch(noteText);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString imageTag = match.captured(0);
            QString imageId = match.captured(1);

            importImage(note, dirPath, noteText, imageTag, imageId);
        }
    }

    note.setNoteText(noteText);
}

void JoplinImportDialog::importImage(Note& note, const QString& dirPath, QString& noteText,
                                     const QString& imageTag, const QString& imageId,
                                     const QString& imageName) {
    QString imageData = _imageData[imageId];

    // Joplin resources are classified as image vs. attachment purely by their
    // "mime:" field, but an `<img src=":/id">` tag can still point at a
    // resource that wasn't classified as an image (e.g. a saved web-clipper
    // page snapshot with `mime: text/html`). Fall back to the attachment
    // data so we can still find and import the underlying file.
    if (imageData.isEmpty()) {
        imageData = _attachmentData[imageId];
    }

    qDebug() << __func__ << " - 'imageName': " << imageName;
    qDebug() << __func__ << " - 'imageId': " << imageId;
    // qDebug() << __func__ << " - 'imageData': " << imageData;

    auto* mediaFile = findResourceFile(dirPath, imageId, imageData);

    qDebug() << __func__ << " - 'mediaFile': " << mediaFile;

    if (mediaFile == nullptr) {
        return;
    }

    QString mediaMarkdown = note.getInsertMediaMarkdown(mediaFile, false, false, imageName);

    qDebug() << __func__ << " - 'mediaMarkdown': " << mediaMarkdown;
    noteText.replace(imageTag, mediaMarkdown);
}

/**
 * Resolves the on-disk resource file for a Joplin resource id.
 *
 * Joplin's raw export doesn't always populate the "file_extension:" field (or a
 * usable original filename to derive it from) in a resource's metadata note --
 * this is common for resources created by older Joplin versions or imported
 * from other apps. When that happens we can't build the "<id>.<ext>" path from
 * metadata alone, so as a last resort we look directly in the resources/
 * directory for a file starting with the resource id.
 *
 * @param dirPath the Joplin export directory
 * @param id the resource id
 * @param metaData the resource's metadata note text (may be empty if the id
 *                 wasn't found in either the image or attachment data at all)
 * @return a QFile pointing at the resource, or nullptr if it can't be found
 */
QFile* JoplinImportDialog::findResourceFile(const QString& dirPath, const QString& id,
                                            const QString& metaData) {
    QString fileExtension;
    auto fileExtensionMatch =
        QRegularExpression("^file_extension: (.+)$", QRegularExpression::MultilineOption)
            .match(metaData);

    if (fileExtensionMatch.hasMatch()) {
        fileExtension = fileExtensionMatch.captured(1);
    } else if (!metaData.isEmpty()) {
        // if the extension wasn't set we'll try to get it from the original file
        auto metaDataLines =
            metaData.split(QRegularExpression(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));
        auto originalFileName = metaDataLines[0];
        auto fileInfo = QFileInfo(originalFileName);
        fileExtension = fileInfo.suffix();
    }

    if (!fileExtension.isEmpty()) {
        auto* mediaFile = new QFile(dirPath + "/resources/" + id + "." + fileExtension);

        if (mediaFile->exists()) {
            return mediaFile;
        }

        delete mediaFile;
    }

    // Last resort: search the resources directory for a file that starts
    // with the resource id, regardless of extension. Some resources (seen in
    // the wild with an empty "mime:"/"file_extension:" combination) are
    // exported with no extension at all, so the glob can't require a ".".
    QDir resourceDir(dirPath + "/resources");
    const QStringList matches = resourceDir.entryList(QStringList() << id + "*", QDir::Files);

    if (!matches.isEmpty()) {
        return new QFile(resourceDir.filePath(matches.first()));
    }

    return nullptr;
}

/**
 * Handle note attachments
 *
 * @param note
 * @param dirPath
 */
void JoplinImportDialog::handleAttachments(Note& note, const QString& dirPath) {
    QString noteText = note.getNoteText();

    // Use a zero-width negative lookbehind `(?<!!)` instead of a consuming
    // `[^!]` character class. The consuming version eats whatever character
    // precedes the link -- which is harmless for an isolated link, but when
    // attachment links are packed back-to-back with no separator, e.g.
    // `[a.pdf](:/id1)[b.pdf](:/id2)`, the closing `)` of the first link's
    // match gets consumed as the "not an image" check for the first link,
    // leaving no character left for the second link's check to consume, so
    // every other link in the chain silently fails to match. The label also
    // uses `(?:\\.|[^\]])*` instead of `[^\]]*` so an escaped bracket in the
    // link text doesn't prematurely end the capture (see handleImages()).
    auto i =
        QRegularExpression(R"((?<!!)(\[((?:\\.|[^\]])*)\]\(:\/([\w\d]+)\)))").globalMatch(noteText);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString attachmentTag = match.captured(1);
        QString attachmentName = match.captured(2);
        QString attachmentId = match.captured(3);
        QString attachmentData = _attachmentData[attachmentId];

        // The attachment may actually be classified as an image (e.g. an
        // image resource referenced via a plain `[label](:/id)` link instead
        // of `![](:/id)`) -- fall back to the image data so we can still
        // find and import the underlying file.
        if (attachmentData.isEmpty()) {
            attachmentData = _imageData[attachmentId];
        }

        qDebug() << __func__ << " - 'attachmentName': " << attachmentName;
        qDebug() << __func__ << " - 'attachmentId': " << attachmentId;

        QString mediaMarkdown;

        // This resource was already copied into the attachments folder
        // earlier in this import (the same resource referenced more than
        // once, e.g. linked twice in one note, or shared across notes) --
        // reuse that copy instead of writing another byte-identical
        // "<id>-1.ext" duplicate.
        const auto existingFileNameIt = _importedAttachmentFileNames.constFind(attachmentId);
        if (existingFileNameIt != _importedAttachmentFileNames.constEnd()) {
            const QString title = attachmentName.isEmpty() ? *existingFileNameIt : attachmentName;
            mediaMarkdown = QStringLiteral("[") + title + QStringLiteral("](") +
                            note.attachmentUrlStringForFileName(*existingFileNameIt) +
                            QStringLiteral(")");
        } else {
            auto* mediaFile = findResourceFile(dirPath, attachmentId, attachmentData);

            qDebug() << __func__ << " - 'mediaFile': " << mediaFile;

            if (mediaFile == nullptr) {
                continue;
            }

            // The destination filename getInsertAttachmentMarkdown() is
            // about to use is derived from mediaFile's own (source) file
            // name, which findResourceFile() always sets to "<id>.<ext>" --
            // unique per resource id, so no other note/resource can already
            // occupy it. Capturing it here (rather than parsing it back out
            // of the returned Markdown) is exact and avoids re-deriving
            // Note's URL-encoding/collision-suffix logic independently.
            const QString destinationFileName = QFileInfo(mediaFile->fileName()).fileName();

            mediaMarkdown = note.getInsertAttachmentMarkdown(mediaFile, attachmentName);

            // getInsertAttachmentMarkdown() silently no-ops (empty return)
            // for a zero-byte resource, in which case nothing was actually
            // copied -- don't cache a filename that was never written.
            if (!mediaMarkdown.isEmpty()) {
                _importedAttachmentFileNames.insert(attachmentId, destinationFileName);
            }
        }

        qDebug() << __func__ << " - 'mediaMarkdown': " << mediaMarkdown;
        noteText.replace(attachmentTag, mediaMarkdown);
    }

    note.setNoteText(noteText);
}
