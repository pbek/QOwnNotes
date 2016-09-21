#include <QFileDialog>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <helpers/htmlentities.h>
#include <utils/misc.h>
#include <entities/note.h>
#include "evernoteimportdialog.h"
#include "ui_evernoteimportdialog.h"

EvernoteImportDialog::EvernoteImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EvernoteImportDialog) {
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
}

EvernoteImportDialog::~EvernoteImportDialog() {
    delete ui;
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
            qDebug() << __func__ << " - 'content': " << data;

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

        while (!result.next().isNull()) {
            query.setFocus(result.current());

            QString title;
            query.setQuery("title/text()");
            query.evaluateTo(&title);
            title = title.trimmed();
            qDebug() << __func__ << " - 'title': " << title;

            QString content;
            query.setQuery("content/text()");

            // content seems to be html encoded
            query.evaluateTo(&content);

            content.replace("\\\"", "\"");

            // decode HTML entities
            HTMLEntities htmlEntities;
            content = htmlEntities.decodeHtmlEntities(content);

            // convert html tags to markdown
            content = Utils::Misc::htmlToMarkdown(content);

            // remove all html tags
            content.remove(QRegularExpression("<.+?>"));

            // remove multiple \n
            content.replace(QRegularExpression("\n+"), "\n");
            content.replace(QRegularExpression("\n\\s+"), "\n");

//            qDebug() << __func__ << " - 'content': " << content;

            QString noteText = Note::createNoteHeader(title) + content;
            qDebug() << __func__ << " - 'noteText': " << noteText;

            Note note = Note();
//            note.setName(title);
            note.setNoteText(noteText);
            note.setNoteSubFolderId(noteSubFolder.getId());
            note.store();
            note.storeNoteTextFileToDisk();

            qDebug() << __func__ << " - 'note': " << note;

            ui->progressBar->setValue(ui->progressBar->value() + 1);
            QCoreApplication::processEvents();
        }

        if (result.hasError()) {
            qWarning() << "Error importing notes";
        }
    }
}
