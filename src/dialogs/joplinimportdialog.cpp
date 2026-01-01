#include "joplinimportdialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/tag.h>

#include <QDebug>
#include <QFileDialog>
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

    auto directoryName = ui->directoryLineEdit->text();
    QDir dir(directoryName);

    if (directoryName.isEmpty() || !dir.exists()) {
        return;
    }

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

    return true;
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
    auto i = QRegularExpression(R"(!\[([^\]]*)\]\(:\/([\w\d]+)\))").globalMatch(noteText);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);
        QString imageName = match.captured(1);
        QString imageId = match.captured(2);

        importImage(note, dirPath, noteText, imageTag, imageId, imageName);
    }

    i = QRegularExpression(R"(<img src=\":\/([\w\d]+)\"\/>)").globalMatch(noteText);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);
        QString imageId = match.captured(1);

        importImage(note, dirPath, noteText, imageTag, imageId);
    }

    note.setNoteText(noteText);
}

void JoplinImportDialog::importImage(Note& note, const QString& dirPath, QString& noteText,
                                     const QString& imageTag, const QString& imageId,
                                     const QString& imageName) {
    QString imageData = _imageData[imageId];

    qDebug() << __func__ << " - 'imageName': " << imageName;
    qDebug() << __func__ << " - 'imageId': " << imageId;
    // qDebug() << __func__ << " - 'imageData': " << imageData;

    QString fileExtension;
    auto fileExtensionMatch =
        QRegularExpression("^file_extension: (.+)$", QRegularExpression::MultilineOption)
            .match(imageData);

    if (fileExtensionMatch.hasMatch()) {
        fileExtension = fileExtensionMatch.captured(1);
    } else {
        // if the extension wasn't set we'll try to get it from the original file
        auto imageDataLines =
            imageData.split(QRegularExpression(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));
        auto originalFileName = imageDataLines[0];
        auto fileInfo = QFileInfo(originalFileName);
        fileExtension = fileInfo.suffix();
    }

    auto* mediaFile = new QFile(dirPath + "/resources/" + imageId + "." + fileExtension);

    qDebug() << __func__ << " - 'mediaFile': " << mediaFile;

    if (!mediaFile->exists()) {
        return;
    }

    QString mediaMarkdown = note.getInsertMediaMarkdown(mediaFile, false, false, imageName);

    qDebug() << __func__ << " - 'mediaMarkdown': " << mediaMarkdown;
    noteText.replace(imageTag, mediaMarkdown);
}

/**
 * Handle note attachments
 *
 * @param note
 * @param dirPath
 */
void JoplinImportDialog::handleAttachments(Note& note, const QString& dirPath) {
    QString noteText = note.getNoteText();
    auto i = QRegularExpression(R"([^!](\[([^\]]*)\]\(:\/([\w\d]+)\)))").globalMatch(noteText);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString attachmentTag = match.captured(1);
        QString attachmentName = match.captured(2);
        QString attachmentId = match.captured(3);
        QString attachmentData = _attachmentData[attachmentId];

        qDebug() << __func__ << " - 'attachmentName': " << attachmentName;
        qDebug() << __func__ << " - 'attachmentId': " << attachmentId;

        QString fileExtension;
        auto fileExtensionMatch =
            QRegularExpression("^file_extension: (.+)$", QRegularExpression::MultilineOption)
                .match(attachmentData);

        if (fileExtensionMatch.hasMatch()) {
            fileExtension = fileExtensionMatch.captured(1);
        } else {
            // if the extension wasn't set we'll try to get it from the original file
            auto attachmentDataLines =
                attachmentData.split(QRegularExpression(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));
            auto originalFileName = attachmentDataLines[0];
            auto fileInfo = QFileInfo(originalFileName);
            fileExtension = fileInfo.suffix();
        }

        auto* mediaFile = new QFile(dirPath + "/resources/" + attachmentId + "." + fileExtension);

        qDebug() << __func__ << " - 'mediaFile': " << mediaFile;

        if (!mediaFile->exists()) {
            continue;
        }

        QString mediaMarkdown = note.getInsertAttachmentMarkdown(mediaFile, attachmentName);

        qDebug() << __func__ << " - 'mediaMarkdown': " << mediaMarkdown;
        noteText.replace(attachmentTag, mediaMarkdown);
    }

    note.setNoteText(noteText);
}
