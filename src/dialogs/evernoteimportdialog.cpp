#include "evernoteimportdialog.h"

#include <entities/note.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QCryptographicHash>
#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QTemporaryFile>
#include <utility>

#include "filedialog.h"
#include "services/settingsservice.h"
#include "ui_evernoteimportdialog.h"

EvernoteImportDialog::EvernoteImportDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::EvernoteImportDialog) {
    ui->setupUi(this);
    afterSetupUI();
    setupMetaDataTreeWidgetItems();
    resetNoteCount();
    _mediaFileDataHash.clear();
    _attachmentFileDataHash.clear();
    _metaDataAttributeHash.clear();
    _metaDataTableText.clear();

    SettingsService settings;
    ui->imageImportCheckBox->setChecked(
        settings.value(QStringLiteral("EvernoteImport/ImageImportCheckBoxChecked"), true).toBool());
    ui->attachmentImportCheckBox->setChecked(
        settings.value(QStringLiteral("EvernoteImport/AttachmentImportCheckBoxChecked"), true)
            .toBool());

    // Load the last selected ENEX file
    QString lastFile = settings.value(QStringLiteral("EvernoteImport/LastFile")).toString();
    if (!lastFile.isEmpty()) {
        QFileInfo fileInfo(lastFile);
        if (fileInfo.exists() && fileInfo.isFile()) {
            ui->fileLineEdit->setText(lastFile);
        }
    }
}

void EvernoteImportDialog::resetNoteCount() {
    ui->progressLCDNumber->hide();
    ui->progressLCDNumber->display(0);
    _importCount = 0;
}

EvernoteImportDialog::~EvernoteImportDialog() {
    SettingsService settings;
    settings.setValue(QStringLiteral("EvernoteImport/ImageImportCheckBoxChecked"),
                      ui->imageImportCheckBox->isChecked());
    settings.setValue(QStringLiteral("EvernoteImport/AttachmentImportCheckBoxChecked"),
                      ui->attachmentImportCheckBox->isChecked());

    // Save the last selected ENEX file
    QString currentFile = ui->fileLineEdit->text();
    if (!currentFile.isEmpty()) {
        settings.setValue(QStringLiteral("EvernoteImport/LastFile"), currentFile);
    }

    storeMetaDataTreeWidgetItemsCheckedState();

    delete ui;
}

int EvernoteImportDialog::getImportCount() { return _importCount; }

/**
 * Imports the notes from a selected file
 */
void EvernoteImportDialog::on_fileButton_clicked() {
    FileDialog dialog(QStringLiteral("Evernote Import"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("Enex files") + " (*.enex)");
    dialog.setWindowTitle(tr("Select Evernote enex file to import"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            ui->fileLineEdit->setText(fileName);
        }
    }
}

/**
 * Imports the notes from the XML in data
 *
 * @param data
 */
void EvernoteImportDialog::importNotes(QXmlStreamReader &xml) {
    bool importMetaData = isMetaDataChecked();

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        // If token is just StartDocument, we'll go to next.
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        // If token is StartElement, we'll see if we can read it.
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("en-export")) {
                continue;
            }

            if (xml.name() == QStringLiteral("note")) {
                Note note = this->parseNote(xml, importMetaData);
                qDebug() << __func__ << " - 'note': " << note;
            }
        }
    }

    // Error handling.
    if (xml.hasError()) {
        qCritical() << "XML error: " << xml.errorString();
    }
}

Note EvernoteImportDialog::parseNote(QXmlStreamReader &xml, bool importMetaData) {
    Note note;

    // Let's check that we're really getting a note.
    if (xml.tokenType() != QXmlStreamReader::StartElement && xml.name() == QStringLiteral("note")) {
        return note;
    }

    // Import the note to the current note subfolder
    note.setNoteSubFolderId(NoteSubFolder::activeNoteSubFolderId());

    QString title;
    QString content;
    QStringList tagNames;
    _metaDataTableText.clear();
    _mediaFileDataHash.clear();
    _attachmentFileDataHash.clear();

    xml.readNext();

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == QStringLiteral("note"))) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("title")) {
                xml.readNext();
                title = xml.text().toString().trimmed();

                qDebug() << __func__ << " - 'title': " << title;
            }

            if (xml.name() == QStringLiteral("content")) {
                xml.readNext();
                content = xml.text().toString().trimmed();

                // Evernote now seems to place whitespaces in front of CDATA for the content,
                // those whitespaces are then used as content by QXmlStreamReader
                // We try to get around that by reading the next token of the XML
                // See: https://github.com/pbek/QOwnNotes/issues/2856
                if (content.isEmpty() && !xml.isCDATA()) {
                    xml.readNext();
                    content = xml.text().toString().trimmed();
                }

                // We don't need that with QXMLStreamReader anymore!
                //                Utils::Misc::unescapeEvernoteImportText(content);

                qDebug() << __func__ << " - 'content': " << content;
            }

            if ((ui->imageImportCheckBox->isChecked() ||
                 ui->attachmentImportCheckBox->isChecked()) &&
                xml.name() == QStringLiteral("resource")) {
                xml.readNext();

                parseResource(xml);
            }

            if (xml.name() == QStringLiteral("tag")) {
                xml.readNext();
                tagNames.append(xml.text().toString().trimmed());
            }

            if (importMetaData) {
                if (xml.name() == QStringLiteral("note-attributes")) {
                    xml.readNext();

                    parseNoteAttributes(xml);
                }

                if (xml.name() == QStringLiteral("updated") ||
                    xml.name() == QStringLiteral("created")) {
                    parseMetaDataItem(xml);
                }
            }
        } else if (xml.tokenType() == QXmlStreamReader::Invalid) {
            // Stop parsing on invalid tokens
            // See: https://github.com/pbek/QOwnNotes/issues/2858
            Utils::Gui::warning(this, tr("Invalid XML!"),
                                tr("Invalid XML found in note <b>%1</b>! The import might be "
                                   "stopped at this point.")
                                    .arg(title),
                                "evernote-import-invalid-xml");
            break;
        }

        xml.readNext();
    }

    if (ui->imageImportCheckBox->isChecked()) {
        // import images
        importImages(note, content);
    }

    if (ui->attachmentImportCheckBox->isChecked()) {
        // import attachments
        importAttachments(note, content);
    }

    // we need to do that after images and attachments are imported,
    // otherwise they will not be detected in-line
    Utils::Misc::transformEvernoteImportText(content, true);

#ifdef Q_OS_WIN32
    // removing or replacing some characters that are asking for
    // troubles
    title.replace("—", "-");
    title.replace("|", "-");
    title.replace("/", "-");
    title.replace("\\", "-");
    title.remove("#");
    title.remove("?");
    title.remove("'");
    title.remove("\"");
#endif

    QString noteText = Note::createNoteHeader(title);

    if (importMetaData) {
        noteText += compileMetaDataText();
    }

    noteText += content.trimmed();

    //            note.setName(title);
    note.setNoteText(noteText);

    // in case the user enabled that the filename can be different
    // from the note name
    // Attention: may already store the note
    note.handleNoteTextFileName();

    note.store();
    note.storeNoteTextFileToDisk();

    // tag the note if tags are found
    if (!tagNames.isEmpty()) {
        tagNote(note, tagNames);
    }

    ui->progressLCDNumber->display(++_importCount);
    QCoreApplication::processEvents();

    return note;
}

/**
 * Adds a metadata tree widget item
 *
 * @param name
 * @param attributeName
 * @param parentItem
 * @return
 */
QTreeWidgetItem *EvernoteImportDialog::addMetaDataTreeWidgetItem(const QString &name,
                                                                 const QString &attributeName,
                                                                 QTreeWidgetItem *parentItem) {
    auto *item = new QTreeWidgetItem();
    item->setText(0, name);

    if (parentItem == nullptr) {
        ui->metaDataTreeWidget->addTopLevelItem(item);
        item->setExpanded(true);
    } else {
        item->setToolTip(0, attributeName);
        item->setData(0, Qt::UserRole, attributeName);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

        SettingsService settings;
        auto metaDataUnCheckedList =
            settings.value(QStringLiteral("EvernoteImport/MetaDataUnCheckedList")).toStringList();
        item->setCheckState(
            0, metaDataUnCheckedList.contains(attributeName) ? Qt::Unchecked : Qt::Checked);

        parentItem->addChild(item);
    }

    return item;
}

/**
 * Setup the metadata tree widget items
 */
void EvernoteImportDialog::setupMetaDataTreeWidgetItems() {
    auto *basicAttributesItem = addMetaDataTreeWidgetItem(tr("Basic attributes"));
    addMetaDataTreeWidgetItem(tr("Created date"), QStringLiteral("created"), basicAttributesItem);
    addMetaDataTreeWidgetItem(tr("Updated date"), QStringLiteral("updated"), basicAttributesItem);

    auto *noteAttributesItem = addMetaDataTreeWidgetItem(tr("Note attributes"));
    addMetaDataTreeWidgetItem(tr("Subject date"), QStringLiteral("note-attributes/subject-date"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Latitude"), QStringLiteral("note-attributes/latitude"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Longitude"), QStringLiteral("note-attributes/longitude"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Altitude"), QStringLiteral("note-attributes/altitude"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Author"), QStringLiteral("note-attributes/author"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Source"), QStringLiteral("note-attributes/source"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Source URL"), QStringLiteral("note-attributes/source-url"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Source application"),
                              QStringLiteral("note-attributes/source-application"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Reminder order"),
                              QStringLiteral("note-attributes/reminder-order"), noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Reminder time"), QStringLiteral("note-attributes/reminder-time"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Reminder done time"),
                              QStringLiteral("note-attributes/reminder-done-time"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Place name"), QStringLiteral("note-attributes/place-name"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Content class"), QStringLiteral("note-attributes/content-class"),
                              noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Application data"),
                              QStringLiteral("note-attributes/application-data"),
                              noteAttributesItem);
}

/**
 * Setup the metadata tree widget items
 */
void EvernoteImportDialog::storeMetaDataTreeWidgetItemsCheckedState() {
    QList<QTreeWidgetItem *> items = ui->metaDataTreeWidget->findItems(
        QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive);
    SettingsService settings;
    QStringList metaDataUnCheckedList;

    Q_FOREACH (QTreeWidgetItem *item, items) {
        QString attributeName = item->data(0, Qt::UserRole).toString();

        if (item->checkState(0) != Qt::Checked) {
            metaDataUnCheckedList << attributeName;
        }
    }

    settings.setValue(QStringLiteral("EvernoteImport/MetaDataUnCheckedList"),
                      metaDataUnCheckedList);
}

/**
 * Checks if a metadata attribute is checked
 */
bool EvernoteImportDialog::isMetaDataChecked() {
    QList<QTreeWidgetItem *> items = ui->metaDataTreeWidget->findItems(
        QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive);

    Q_FOREACH (QTreeWidgetItem *item, items) {
        if (item->checkState(0) == Qt::Checked) {
            return true;
        }
    }

    return false;
}

/**
 * Reads the enex files and imports the notes
 */
void EvernoteImportDialog::on_importButton_clicked() {
    auto fileName = ui->fileLineEdit->text();

    if (fileName.isEmpty()) {
        return;
    }

    auto file = new QFile(fileName);

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << file->errorString() << " (path: " << fileName << ")";
        return;
    }

    QXmlStreamReader xml(file);

    ui->importButton->setEnabled(false);
    QCoreApplication::processEvents();

    determineMetaDataAttributes();
    resetNoteCount();
    ui->progressLCDNumber->show();
    importNotes(xml);

    ui->importButton->setEnabled(true);
}

/**
 * Returns the Markdown code for an image file data entry
 *
 * @param mediaFileData
 * @return
 */
QString EvernoteImportDialog::getMarkdownForMediaFileData(
    Note note, const EvernoteImportDialog::MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString imageSuffix = mediaFileData.suffix;

    return note.importMediaFromBase64(data, imageSuffix);
}

/**
 * Returns the Markdown code for an attachment file data entry
 *
 * @param mediaFileData
 * @return
 */
QString EvernoteImportDialog::getMarkdownForAttachmentFileData(
    Note note, const EvernoteImportDialog::MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString suffix = mediaFileData.suffix;
    QString fileName = mediaFileData.fileName;

    // create a temporary file for the attachment
    auto *tempFile = new QTemporaryFile(QDir::tempPath() + QDir::separator() +
                                        QStringLiteral("media-XXXXXX.") + suffix);

    if (!tempFile->open()) {
        return QString();
    }

    // write file data to the temporary file
    tempFile->write(QByteArray::fromBase64(data.toLatin1()));

    // store the temporary file in the media folder and return the
    // Markdown code
    QString markdownCode = note.getInsertAttachmentMarkdown(
        tempFile, fileName, false, Note::extendedCleanupFileName(fileName));

    return markdownCode;
}

/**
 * Place the attachments found in _attachmentFileDataHash into the note content
 *
 * @param xml
 */
void EvernoteImportDialog::importAttachments(const Note &note, QString &content) {
    if (_attachmentFileDataHash.count() == 0) {
        return;
    }

    // match media tags
    QRegularExpression re(QStringLiteral("<en-media.+?type=\".+?\".*?>"),
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all files
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString mediaTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2(QStringLiteral("hash=\"(.+?)\""),
                               QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch hashMatch = re2.match(mediaTag);

        if (!hashMatch.hasMatch()) {
            continue;
        }

        QString objectId = hashMatch.captured(1);

        if (!_attachmentFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded file
        MediaFileData mediaFileData = _attachmentFileDataHash[objectId];

        // get the Markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(note, mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace media tag with Markdown code
            content.replace(mediaTag, markdownCode);
        }
    }

    // get the file's Markdown code for missing attachments
    QHashIterator<QString, MediaFileData> hashIterator(_attachmentFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the Markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(note, mediaFileData);

        content += "\n" + markdownCode;
    }
}

/**
 * Place the images found in _mediaFileDataHash into the note content
 *
 * @param xml
 */
void EvernoteImportDialog::importImages(const Note &note, QString &content) {
    if (_mediaFileDataHash.count() == 0) {
        return;
    }

    // match image tags
    QRegularExpression re(QStringLiteral("<en-media.+?type=\"image/.+?\".*?>"),
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all images
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString imageTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2(QStringLiteral("hash=\"(.+?)\""),
                               QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch hashMatch = re2.match(imageTag);

        if (!hashMatch.hasMatch()) {
            continue;
        }

        QString objectId = hashMatch.captured(1);

        if (!_mediaFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded image
        MediaFileData mediaFileData = _mediaFileDataHash[objectId];

        // get the Markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(note, mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace image tag with Markdown code (and a leading newline, since
            // Evernote doesn't support inline images, and we like to add a spacer)
            content.replace(imageTag, QStringLiteral("\n") + markdownCode);
        }
    }

    // get the image Markdown code for missing images
    QHashIterator<QString, MediaFileData> hashIterator(_mediaFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the Markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(note, mediaFileData);

        content += QStringLiteral("\n") + markdownCode;
    }
}

/**
 * Parses images and attachment from the XML
 *
 * @param content
 */
void EvernoteImportDialog::parseResource(QXmlStreamReader &xml) {
    if (xml.tokenType() != QXmlStreamReader::StartElement &&
        xml.name() == QStringLiteral("resource")) {
        return;
    }

    QRegularExpressionMatch match;
    QString objectType;
    QString suffix;
    QString objectId;
    QString data;
    QString fileName;
    bool isImage = false;
    Q_UNUSED(isImage)

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == QStringLiteral("resource"))) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            // Parse the mime data of the object
            if (xml.name() == QStringLiteral("mime")) {
                xml.readNext();
                QString mime = xml.text().toString().trimmed();
                QRegularExpression mimeExpression(QStringLiteral("(.+)?\\/(.+)?"),
                                                  QRegularExpression::CaseInsensitiveOption);
                QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

                if (mimeMatch.hasMatch()) {
                    objectType = mimeMatch.captured(1);
                    suffix = mimeMatch.captured(2);
                }

                if (objectType == QStringLiteral("image")) {
                    isImage = true;
                }
                qDebug() << __func__ << " - 'mime': " << mime;
                qDebug() << __func__ << " - 'objectType': " << objectType;
                qDebug() << __func__ << " - 'suffix': " << suffix;
            }

            // Check the recognition attribute for an object type
            if (xml.name() == QStringLiteral("recognition")) {
                xml.readNext();
                QString recognition = xml.text().toString().trimmed();
                recognition.replace(QStringLiteral("\\\""), QStringLiteral("\""));

                match = QRegularExpression(QStringLiteral("objID=\"(.+?)\""),
                                           QRegularExpression::CaseInsensitiveOption)
                            .match(recognition);

                if (match.hasMatch()) {
                    // get the object id of the resource to match it with the
                    // en-media hash
                    objectId = match.captured(1);
                }

                qDebug() << __func__ << " - 'objectId': " << objectId;
            }

            if (xml.name() == QStringLiteral("data")) {
                xml.readNext();
                data = xml.text().toString().trimmed();

                //                qDebug() << __func__ << " - 'data': " << data;
            }

            if (xml.name() == QStringLiteral("resource-attributes")) {
                xml.readNext();

                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == QStringLiteral("resource-attributes"))) {
                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == QStringLiteral("file-name")) {
                            xml.readNext();
                            fileName = xml.text().toString().trimmed();
                            qDebug() << __func__ << " - 'fileName': " << fileName;
                        }
                    }
                    xml.readNext();
                }
            }
        }

        xml.readNext();
    }

    // if there is no object id set we generate the hash ourselves
    if (objectId.isEmpty()) {
        objectId = QString(QCryptographicHash::hash(QByteArray::fromBase64(data.toLatin1()),
                                                    QCryptographicHash::Md5)
                               .toHex());
    }

    if (isImage) {
        // store data in the QHash
        _mediaFileDataHash[objectId].data = data;
        _mediaFileDataHash[objectId].suffix = suffix;
    } else {
        // get the file suffix from the filename
        QFileInfo fileInfo(fileName);
        suffix = fileInfo.suffix();

        // store data in the QHash
        _attachmentFileDataHash[objectId].data = data.trimmed();
        _attachmentFileDataHash[objectId].suffix = suffix;
        _attachmentFileDataHash[objectId].fileName = fileName;
    }
}

void EvernoteImportDialog::parseMetaDataItem(QXmlStreamReader &xml, bool isNoteAttribute) {
    QString attributeName = xml.name().toString();

    if (isNoteAttribute) {
        attributeName = QStringLiteral("note-attributes/") + attributeName;
    }

    if (!_metaDataAttributeHash.contains(attributeName)) {
        return;
    }

    xml.readNext();
    QString attribute = xml.text().toString().trimmed();

    if (attribute.isEmpty()) {
        return;
    }

    QString name = _metaDataAttributeHash[attributeName];
    _metaDataTableText +=
        QStringLiteral("| ") + name + (" | ") + attribute + QStringLiteral(" |\n");
}

void EvernoteImportDialog::parseNoteAttributes(QXmlStreamReader &xml) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == QStringLiteral("note-attributes"))) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            parseMetaDataItem(xml, true);
        }
        xml.readNext();
    }
}

QString EvernoteImportDialog::compileMetaDataText() {
    if (_metaDataTableText.isEmpty()) {
        return {};
    }

    return "| " + tr("Attribute") + " | " + tr("Value") +
           " |\n"
           "|---|---|\n" +
           _metaDataTableText + QStringLiteral("\n");
}

void EvernoteImportDialog::determineMetaDataAttributes() {
    _metaDataAttributeHash.clear();
    _metaDataTableText.clear();

    QList<QTreeWidgetItem *> items = ui->metaDataTreeWidget->findItems(
        QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive);

    Q_FOREACH (QTreeWidgetItem *item, items) {
        if (item->checkState(0) != Qt::Checked) {
            continue;
        }

        QString name = item->text(0);
        QString attributeName = item->data(0, Qt::UserRole).toString();

        _metaDataAttributeHash[attributeName] = name;
    }
}

/**
 * Tags the note if tagNames are found
 *
 * @param note
 * @param tagNames
 */
void EvernoteImportDialog::tagNote(Note &note, QStringList &tagNames) {
    // Loop tagNames
    Q_FOREACH (const QString &tagName, tagNames) {
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
