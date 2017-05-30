#include <QFileDialog>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QTemporaryFile>
#include <helpers/htmlentities.h>
#include <utils/misc.h>
#include <entities/tag.h>
#include "evernoteimportdialog.h"
#include "ui_evernoteimportdialog.h"
#include "filedialog.h"

EvernoteImportDialog::EvernoteImportDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::EvernoteImportDialog) {
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    _importCount = 0;
}

EvernoteImportDialog::~EvernoteImportDialog() {
    delete ui;
}

int EvernoteImportDialog::getImportCount() {
    return _importCount;
}

/**
 * Imports the notes from a selected file
 */
void EvernoteImportDialog::on_fileButton_clicked() {
    FileDialog dialog("EvernoteImport");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("Enex files") + " (*.enex)");
    dialog.setWindowTitle(tr("Select Evernote enex file to import"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            ui->fileLineEdit->setText(fileName);

            QFile file(fileName);

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qCritical() << file.errorString() <<
                                          " (path: " << fileName << ")";
                return;
            }

            QString data = file.readAll();

            QCoreApplication::processEvents();
            initNoteCount(data);
            QCoreApplication::processEvents();
            importNotes(data);
        }
    }
}

/**
 * Counts the notes from the XML in data
 *
 * @param data
 */
int EvernoteImportDialog::countNotes(QString data) {
    QXmlQuery query;
    query.setFocus(data);
    query.setQuery("en-export/note");
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
void EvernoteImportDialog::initNoteCount(QString data) {
    int count = countNotes(data);

    ui->progressBar->setMaximum(count);
    ui->progressBar->show();
}

/**
 * Imports and embeds images from an Evernote note
 *
 * @param content
 */
QString EvernoteImportDialog::importImages(QString content, QXmlQuery query) {
    query.setQuery("resource");

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;
    int missingImageCount = 0;

    while (!result.next().isNull()) {
        QString objectType;
        QString suffix;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery("mime/text()");
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
                "(.+)?\\/(.+)?", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
            suffix = mimeMatch.captured(2);
        }

        // we only want to import images
        if (objectType != "image") {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery("recognition/text()");
        query.evaluateTo(&recognition);

        recognition.replace("\\\"", "\"");

        match = QRegularExpression(
                "objID=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        // if no object id was found we will use a fallback id so we can
        // import all missing images
        if (objectId.isEmpty()) {
            objectId = "obj" + QString::number(++missingImageCount);
        }

        QString data;
        query.setQuery("data/text()");
        query.evaluateTo(&data);

        // store data in the QHash
        mediaFileDataHash[objectId].data = data;
        mediaFileDataHash[objectId].suffix = suffix;
    }

    if (mediaFileDataHash.count() == 0) {
        return content;
    }

    // match image tags
    QRegularExpression re("<en-media.+?type=\"image/.+?\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all images
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString imageTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2("hash=\"(.+?)\"",
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
        QString markdownCode = getMarkdownForMediaFileData(mediaFileData);

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
        QString markdownCode = getMarkdownForMediaFileData(mediaFileData);

        content += "\n" + markdownCode;
    }

    return content;
}

/**
 * Imports and embeds attachments from an Evernote note
 *
 * @param content
 */
QString EvernoteImportDialog::importAttachments(QString content,
                                                QXmlQuery query) {
    query.setQuery("resource");

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;
    int missingAttachmentCount = 0;

    while (!result.next().isNull()) {
        QString objectType;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery("mime/text()");
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
                "(.+)?\\/(.+)?", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
        }

        // we don't want to import images
        if (objectType == "image") {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery("recognition/text()");
        query.evaluateTo(&recognition);

        recognition.replace("\\\"", "\"");

        match = QRegularExpression(
                "objID=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        // if no object id was found we will use a fallback id so we can
        // import all missing attachments
        if (objectId.isEmpty()) {
            objectId = "obj" + QString::number(++missingAttachmentCount);
        }

        QString data;
        query.setQuery("data/text()");
        query.evaluateTo(&data);

        // find the filename
        QString fileName;
        query.setQuery("resource-attributes/file-name/text()");
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
    QRegularExpression re("<en-media.+?type=\".+?\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all files
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString mediaTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2("hash=\"(.+?)\"",
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
        QString markdownCode = getMarkdownForAttachmentFileData(mediaFileData);

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
        QString markdownCode = getMarkdownForAttachmentFileData(mediaFileData);

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
        EvernoteImportDialog::MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString imageSuffix = mediaFileData.suffix;

    // create a temporary file for the image
    QTemporaryFile *tempFile = new QTemporaryFile(
            QDir::tempPath() + QDir::separator() + "media-XXXXXX." +
            imageSuffix);

    if (!tempFile->open()) {
        return "";
    }

    // write image to the temporary file
    tempFile->write(QByteArray::fromBase64(data.toLatin1()));

    // store the temporary image in the media folder and return the
    // markdown code
    QString markdownCode = Note::getInsertMediaMarkdown(tempFile);
    return markdownCode;
}

/**
 * Returns the markdown code for an attachment file data entry
 *
 * @param mediaFileData
 * @return
 */
QString EvernoteImportDialog::getMarkdownForAttachmentFileData(
        EvernoteImportDialog::MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString suffix = mediaFileData.suffix;
    QString fileName = mediaFileData.fileName;

    // create a temporary file for the attachment
    QTemporaryFile *tempFile = new QTemporaryFile(
            QDir::tempPath() + QDir::separator() + "media-XXXXXX." + suffix);

    if (!tempFile->open()) {
        return "";
    }

    // write file data to the temporary file
    tempFile->write(QByteArray::fromBase64(data.toLatin1()));

    // store the temporary file in the media folder and return the
    // markdown code
    QString markdownCode = Note::getInsertAttachmentMarkdown(tempFile,
                                                             fileName);
    return markdownCode;
}

/**
 * Imports the notes from the XML in data
 *
 * @param data
 */
void EvernoteImportDialog::importNotes(QString data) {
    QXmlQuery query;
    query.setFocus(data);
    query.setQuery("en-export/note");

    QXmlResultItems result;
    if (query.isValid()) {
        query.evaluateTo(&result);
        NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();
        HTMLEntities htmlEntities;

        while (!result.next().isNull()) {
            query.setFocus(result.current());

            QString title;
            query.setQuery("title/text()");
            query.evaluateTo(&title);
            title = title.trimmed();

            QString content;
            query.setQuery("content/text()");

            // content seems to be html encoded
            query.evaluateTo(&content);

            content.replace("\\\"", "\"");

            // decode HTML entities
            content = htmlEntities.decodeHtmlEntities(content);

            // add a newline in front of lists
//            content.replace(QRegularExpression("<ul.*?>"), "\n<ul>");
//            content.replace(QRegularExpression("<ol.*?>"), "\n<ol>");

            // add a linebreak instead of div-containers
            content.replace(QRegularExpression("<\\/div>"), "\n");

            // convert html tags to markdown
            content = Utils::Misc::htmlToMarkdown(content);

            if (ui->imageImportCheckBox->isChecked()) {
                // import images
                content = importImages(content, query);
            }

            if (ui->attachmentImportCheckBox->isChecked()) {
                // import attachments
                content = importAttachments(content, query);
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

            QString noteText = Note::createNoteHeader(title) + content;

            Note note = Note();
//            note.setName(title);
            note.setNoteText(noteText);
            note.setNoteSubFolderId(noteSubFolder.getId());
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
    query.setQuery("tag");
    QXmlResultItems result;
    query.evaluateTo(&result);

    while (!result.next().isNull()) {
        query.setFocus(result.current());

        QString tagName;
        query.setQuery("text()");
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
