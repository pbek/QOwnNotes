#include <utility>
#include <QFileDialog>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QTemporaryFile>
#include <utils/misc.h>
#include <entities/tag.h>
#include <QCryptographicHash>
#include "evernoteimportdialog.h"
#include "ui_evernoteimportdialog.h"
#include "filedialog.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QSettings>

EvernoteImportDialog::EvernoteImportDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::EvernoteImportDialog) {
    ui->setupUi(this);
    setupMetaDataTreeWidgetItems();
    ui->progressBar->hide();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    _importCount = 0;

    QSettings settings;
    ui->imageImportCheckBox->setChecked(settings.value(
            QStringLiteral("EvernoteImport/ImageImportCheckBoxChecked"), true).toBool());
    ui->attachmentImportCheckBox->setChecked(settings.value(
            QStringLiteral("EvernoteImport/AttachmentImportCheckBoxChecked"), true).toBool());
}

EvernoteImportDialog::~EvernoteImportDialog() {
    QSettings settings;
    settings.setValue(QStringLiteral("EvernoteImport/ImageImportCheckBoxChecked"),
            ui->imageImportCheckBox->isChecked());
    settings.setValue(QStringLiteral("EvernoteImport/AttachmentImportCheckBoxChecked"),
            ui->attachmentImportCheckBox->isChecked());

    storeMetaDataTreeWidgetItemsCheckedState();

    delete ui;
}

int EvernoteImportDialog::getImportCount() {
    return _importCount;
}

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
 * Counts the notes from the XML in data
 *
 * @param data
 */
int EvernoteImportDialog::countNotes(const QString& data) {
    QXmlQuery query;
    query.setFocus(data);
    query.setQuery(QStringLiteral("en-export/note"));
    int count = 0;

    QXmlResultItems result;
    if (query.isValid()) {
        query.evaluateTo(&result);

        while (!result.next().isNull()) {
            count++;
        }
    }

    return count;
}

/**
 * Initializes the progress bar with the count of notes from the XML in data
 *
 * @param data
 */
void EvernoteImportDialog::initNoteCount(const QString &data) {
    int count = countNotes(data);

    ui->progressBar->setMaximum(count);
    ui->progressBar->show();
}

/**
 * Imports and embeds images from an Evernote note
 *
 * @param content
 */
QString EvernoteImportDialog::importImages(const Note &note,
                                           QString content,
                                           QXmlQuery query) {
    query.setQuery(QStringLiteral("resource"));

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;

    while (!result.next().isNull()) {
        QString objectType;
        QString suffix;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery(QStringLiteral("mime/text()"));
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
                QStringLiteral("(.+)?\\/(.+)?"), QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
            suffix = mimeMatch.captured(2);
        }

        // we only want to import images
        if (objectType != QStringLiteral("image")) {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery(QStringLiteral("recognition/text()"));
        query.evaluateTo(&recognition);

        recognition.replace(QStringLiteral("\\\""), QStringLiteral("\""));

        match = QRegularExpression(
                QStringLiteral("objID=\"(.+?)\""),
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        QString data;
        query.setQuery(QStringLiteral("data/text()"));
        query.evaluateTo(&data);

        // if there is no object id set we generate the hash ourselves
        if (objectId.isEmpty()) {
            objectId = QString(QCryptographicHash::hash(
                    QByteArray::fromBase64(data.toLatin1()),
                    QCryptographicHash::Md5).toHex());
        }

        // store data in the QHash
        mediaFileDataHash[objectId].data = data;
        mediaFileDataHash[objectId].suffix = suffix;
    }

    if (mediaFileDataHash.count() == 0) {
        return content;
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

        if (!mediaFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded image
        MediaFileData mediaFileData = mediaFileDataHash[objectId];

        // get the markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(note, mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace image tag with markdown code
            content.replace(imageTag, markdownCode);
        }
    }

    // get the image markdown code for missing images
    QHashIterator<QString, MediaFileData> hashIterator(mediaFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(note, mediaFileData);

        content += QStringLiteral("\n") + markdownCode;
    }

    return content;
}

/**
 * Imports and embeds attachments from an Evernote note
 *
 * @param content
 */
QString EvernoteImportDialog::importAttachments(const Note &note,
                                                QString content,
                                                QXmlQuery query) {
    query.setQuery(QStringLiteral("resource"));

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;

    while (!result.next().isNull()) {
        QString objectType;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery(QStringLiteral("mime/text()"));
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
               QStringLiteral( "(.+)?\\/(.+)?"), QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
        }

        // we don't want to import images
        if (objectType == QStringLiteral("image")) {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery(QStringLiteral("recognition/text()"));
        query.evaluateTo(&recognition);

        recognition.replace(QStringLiteral("\\\""), QStringLiteral("\""));

        match = QRegularExpression(
                QStringLiteral("objID=\"(.+?)\""),
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        QString data;
        query.setQuery(QStringLiteral("data/text()"));
        query.evaluateTo(&data);

        // if there is no object id set we generate the hash ourselves
        if (objectId.isEmpty()) {
            objectId = QString(QCryptographicHash::hash(
                    QByteArray::fromBase64(data.toLatin1()),
                    QCryptographicHash::Md5).toHex());
        }

        // find the filename
        QString fileName;
        query.setQuery(QStringLiteral("resource-attributes/file-name/text()"));
        query.evaluateTo(&fileName);
        fileName = fileName.trimmed();

        // get the file suffix from the filename
        QFileInfo fileInfo(fileName);
        QString suffix = fileInfo.suffix();

        // store data in the QHash
        mediaFileDataHash[objectId].data = data;
        mediaFileDataHash[objectId].suffix = suffix;
        mediaFileDataHash[objectId].fileName = fileName;
    }

    if (mediaFileDataHash.count() == 0) {
        return content;
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

        if (!mediaFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded file
        MediaFileData mediaFileData = mediaFileDataHash[objectId];

        // get the markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(note, mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace media tag with markdown code
            content.replace(mediaTag, markdownCode);
        }
    }

    // get the file markdown code for missing attachments
    QHashIterator<QString, MediaFileData> hashIterator(mediaFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(note, mediaFileData);

        content += "\n" + markdownCode;
    }

    return content;
}

/**
 * Returns the markdown code for an image file data entry
 *
 * @param mediaFileData
 * @return
 */
QString EvernoteImportDialog::getMarkdownForMediaFileData(
        Note note,
        const EvernoteImportDialog::MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString imageSuffix = mediaFileData.suffix;

    return note.importMediaFromBase64(data, imageSuffix);
}

/**
 * Returns the markdown code for an attachment file data entry
 *
 * @param mediaFileData
 * @return
 */
QString EvernoteImportDialog::getMarkdownForAttachmentFileData(
        Note note,
        const EvernoteImportDialog::MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString suffix = mediaFileData.suffix;
    QString fileName = mediaFileData.fileName;

    // create a temporary file for the attachment
    auto *tempFile = new QTemporaryFile(
            QDir::tempPath() + QDir::separator() + QStringLiteral("media-XXXXXX.") + suffix);

    if (!tempFile->open()) {
        return QString();
    }

    // write file data to the temporary file
    tempFile->write(QByteArray::fromBase64(data.toLatin1()));

    // store the temporary file in the media folder and return the
    // markdown code
    QString markdownCode = note.getInsertAttachmentMarkdown(tempFile, fileName);

    return markdownCode;
}

/**
 * Imports the notes from the XML in data
 *
 * @param data
 */
void EvernoteImportDialog::importNotes(const QString& data) {
    QXmlQuery query;
    query.setFocus(data);
    query.setQuery(QStringLiteral("en-export/note"));

    bool importMetaData = isMetaDataChecked();

    QXmlResultItems result;
    if (query.isValid()) {
        query.evaluateTo(&result);
        NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();

        while (!result.next().isNull()) {
            Note note = Note();
            note.setNoteSubFolderId(noteSubFolder.getId());

            query.setFocus(result.current());

            QString title;
            query.setQuery(QStringLiteral("title/text()"));
            query.evaluateTo(&title);
            title = title.trimmed();

            QString content;
            query.setQuery(QStringLiteral("content/text()"));

            // content seems to be html encoded
            query.evaluateTo(&content);

            content.replace(QStringLiteral("\\\""), QStringLiteral("\""));

            // decode HTML entities
            content = Utils::Misc::unescapeHtml(std::move(content));

            // add a newline in front of lists
//            content.replace(QRegularExpression("<ul.*?>"), "\n<ul>");
//            content.replace(QRegularExpression("<ol.*?>"), "\n<ol>");

            // remove web-clip code
//            content.remove("<div style=\"-evernote-webclip:true;\">");

            // replace code blocks
            content.replace(QRegularExpression(QStringLiteral(
                    R"(<div style="box-sizing.+?-en-codeblock:\s*true;"><div>(.+?)<\/div><\/div>)"),
                    QRegularExpression::MultilineOption),
                            QStringLiteral("\n```\n\\1\n```\n"));
            content.replace(QRegularExpression(QStringLiteral(
                    R"(<div style="-en-codeblock:\s*true;.+?"><div>(.+?)<\/div><\/div>)"),
                    QRegularExpression::MultilineOption),
                            QStringLiteral("\n```\n\\1\n```\n"));

            // add a linebreak instead of div-containers
            content.replace(QRegularExpression("<\\/div>"), QStringLiteral("\n"));

            // convert remaining special characters
            content = Utils::Misc::unescapeHtml(std::move(content));

            // convert html tags to markdown
            content = Utils::Misc::htmlToMarkdown(std::move(content));

            if (ui->imageImportCheckBox->isChecked()) {
                // import images
                content = importImages(note, std::move(content), query);
            }

            if (ui->attachmentImportCheckBox->isChecked()) {
                // import attachments
                content = importAttachments(note, std::move(content), query);
            }

            // remove all html tags
            content.remove(QRegularExpression("<.+?>"));

            // remove multiple \n
            content.replace(QRegularExpression("\n\n+"), "\n\n");
            content.replace(QRegularExpression("\n\n\\s+"), "\n\n");

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
                noteText += generateMetaDataMarkdown(query);
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
            tagNote(query, note);

            _importCount++;
            ui->progressBar->setValue(ui->progressBar->value() + 1);
            QCoreApplication::processEvents();
        }

        if (result.hasError()) {
            qWarning() << "Error importing notes";
        }
    }
}

/**
 * Tags the note if tags are found
 *
 * @param query
 * @param note
 */
void EvernoteImportDialog::tagNote(QXmlQuery &query, Note &note) {
    query.setQuery(QStringLiteral("tag"));
    QXmlResultItems result;
    query.evaluateTo(&result);

    while (!result.next().isNull()) {
        query.setFocus(result.current());

        QString tagName;
        query.setQuery(QStringLiteral("text()"));
        query.evaluateTo(&tagName);
        tagName = tagName.trimmed();

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
 * Adds a metadata tree widget item
 *
 * @param name
 * @param attributeName
 * @param parentItem
 * @return
 */
QTreeWidgetItem *EvernoteImportDialog::addMetaDataTreeWidgetItem(
        const QString& name,
        const QString& attributeName,
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

        QSettings settings;
        auto metaDataUnCheckedList = settings.value(
                QStringLiteral("EvernoteImport/MetaDataUnCheckedList")).toStringList();
        item->setCheckState(0,
                metaDataUnCheckedList.contains(attributeName) ?
                Qt::Unchecked : Qt::Checked);

        parentItem->addChild(item);
    }

    return item;
}

/**
 * Setup the metadata tree widget items
 */
void EvernoteImportDialog::setupMetaDataTreeWidgetItems() {
    auto *basicAttributesItem = addMetaDataTreeWidgetItem(
            tr("Basic attributes"));
    addMetaDataTreeWidgetItem(tr("Created date"), "created",
            basicAttributesItem);
    addMetaDataTreeWidgetItem(tr("Updated date"), "updated",
            basicAttributesItem);

    auto *noteAttributesItem = addMetaDataTreeWidgetItem(tr("Note attributes"));
    addMetaDataTreeWidgetItem(tr("Subject date"),
            "note-attributes/subject-date", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Latitude"),
            "note-attributes/latitude", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Longitude"),
            "note-attributes/longitude", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Altitude"),
            "note-attributes/altitude", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Author"),
            "note-attributes/author", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Source"),
            "note-attributes/source", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Source URL"),
            "note-attributes/source-url", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Source application"),
            "note-attributes/source-application", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Reminder order"),
            "note-attributes/reminder-order", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Reminder time"),
            "note-attributes/reminder-time", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Reminder done time"),
            "note-attributes/reminder-done-time", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Place name"),
            "note-attributes/place-name", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Content class"),
            "note-attributes/content-class", noteAttributesItem);
    addMetaDataTreeWidgetItem(tr("Application data"),
            "note-attributes/application-data", noteAttributesItem);
}

/**
 * Setup the metadata tree widget items
 */
void EvernoteImportDialog::storeMetaDataTreeWidgetItemsCheckedState() {
    QList<QTreeWidgetItem *> items = ui->metaDataTreeWidget->findItems(
            QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard |
                          Qt::MatchRecursive);
    QSettings settings;
    QStringList metaDataUnCheckedList;

    Q_FOREACH(QTreeWidgetItem *item, items) {
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
            QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard |
                          Qt::MatchRecursive);

    Q_FOREACH(QTreeWidgetItem *item, items) {
            if (item->checkState(0) == Qt::Checked) {
                return true;
            }
        }

    return false;
}

/**
 * Generates the metadata markdown table for a note
 */
QString EvernoteImportDialog::generateMetaDataMarkdown(QXmlQuery query) {
    QString resultText;
    QString tableText;
    QList<QTreeWidgetItem *> items = ui->metaDataTreeWidget->findItems(
            QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard |
                          Qt::MatchRecursive);

    Q_FOREACH(QTreeWidgetItem *item, items) {
            if (item->checkState(0) != Qt::Checked) {
                continue;
            }

            QString name = item->text(0);
            QString attributeName = item->data(0, Qt::UserRole).toString();

            QString attribute;
            query.setQuery(attributeName + QStringLiteral("/text()"));
            query.evaluateTo(&attribute);
            attribute = attribute.trimmed();

            if (attribute.isEmpty()) {
                continue;
            }

            tableText += QStringLiteral("| ") + name + (" | ") + attribute + QStringLiteral(" |\n");
        }

    if (!tableText.isEmpty()) {
        resultText = "| " + tr("Attribute") + " | " + tr("Value") + " |\n"
                     "|---|---|\n" +
                     tableText;
    }

    return resultText + QStringLiteral("\n");
}

/**
 * Reads the enex files and imports the notes
 */
void EvernoteImportDialog::on_importButton_clicked() {
    auto fileName = ui->fileLineEdit->text();

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << file.errorString() << " (path: " << fileName << ")";
        return;
    }

    ui->importButton->setEnabled(false);

    QString data = file.readAll();

    QCoreApplication::processEvents();
    initNoteCount(data);
    QCoreApplication::processEvents();
    importNotes(data);

    ui->importButton->setEnabled(true);
}
