#include <QFileDialog>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QTemporaryFile>
#include <helpers/htmlentities.h>
#include <utils/misc.h>
#include <entities/note.h>
#include "evernoteimportdialog.h"
#include "ui_evernoteimportdialog.h"

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
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Enex files (*.enex)"));
    dialog.setWindowTitle(tr("Select Evernote enex file to import"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            QString fileName = fileNames.at(0);
            ui->fileLineEdit->setText(fileName);

            QFile file(fileName);

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
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
    QHash<QString, QString> imageFileData;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;

    while (!result.next().isNull()) {
        query.setFocus(result.current());

        QString recognition;
        query.setQuery("recognition/text()");
        query.evaluateTo(&recognition);

        recognition.replace("\\\"", "\"");

        match = QRegularExpression(
                "objType=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        if (!match.hasMatch()) {
            continue;
        }

        // get the object type of the resource
        QString objectType = match.captured(1);

        // we only want to import images
        if (objectType != "image") {
            continue;
        }

        match = QRegularExpression(
                "objID=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        if (!match.hasMatch()) {
            continue;
        }

        // get the object id of the resource to match it with the
        // en-media hash
        QString objectId = match.captured(1);

        QString data;
        query.setQuery("data/text()");
        query.evaluateTo(&data);

        // store data in a QHash
        imageFileData[objectId] = data;
    }

    // match image tags
    QRegularExpression re(
            "<en-media.+?type=\"image/(.+)?\".+?hash=\"(.+?)\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);

    // find all images
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);
        QString imageSuffix = match.captured(1);
        QString imageHash = match.captured(2);

        if (!imageFileData.contains(imageHash)) {
            continue;
        }

        // get the base64 encoded image
        QString data = imageFileData[imageHash];

        // create a temporary file for the image
        QTemporaryFile *tempFile = new QTemporaryFile(
                QDir::tempPath() + QDir::separator() + "media-XXXXXX." +
                        imageSuffix);

        if (!tempFile->open()) {
            continue;
        }

        // write image to the temporary file
        tempFile->write(QByteArray::fromBase64(data.toLatin1()));

        // store the temporary image in the media folder and return the
        // markdown code
        QString markdownCode = Note::getInsertMediaMarkdown(tempFile);

        if (!markdownCode.isEmpty()) {
            // replace image tag with markdown code
            content.replace(imageTag, markdownCode);
        }
    }

    return content;
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

            // remove all html tags
            content.remove(QRegularExpression("<.+?>"));

            // remove multiple \n
            content.replace(QRegularExpression("\n\n+"), "\n\n");
            content.replace(QRegularExpression("\n\n\\s+"), "\n\n");

            QString noteText = Note::createNoteHeader(title) + content;

            Note note = Note();
//            note.setName(title);
            note.setNoteText(noteText);
            note.setNoteSubFolderId(noteSubFolder.getId());
            note.store();
            note.storeNoteTextFileToDisk();

            _importCount++;
            ui->progressBar->setValue(ui->progressBar->value() + 1);
            QCoreApplication::processEvents();
        }

        if (result.hasError()) {
            qWarning() << "Error importing notes";
        }
    }
}
