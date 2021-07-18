#include "joplinimportdialog.h"
#include "filedialog.h"
#include <entities/note.h>
#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include "ui_joplinimportdialog.h"

JoplinImportDialog::JoplinImportDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::JoplinImportDialog) {
    ui->setupUi(this);

    ui->groupBox->setVisible(false);
    ui->tagImportCheckBox->setDisabled(true);
    ui->imageImportCheckBox->setDisabled(true);
    ui->attachmentImportCheckBox->setDisabled(true);

    ui->progressBar->hide();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    _importCount = 0;

    QSettings settings;
    ui->imageImportCheckBox->setChecked(
        settings
            .value(QStringLiteral("JoplinImport/ImageImportCheckBoxChecked"),
                   true)
            .toBool());
    ui->attachmentImportCheckBox->setChecked(
        settings
            .value(QStringLiteral(
                       "JoplinImport/AttachmentImportCheckBoxChecked"),
                   true)
            .toBool());
}

JoplinImportDialog::~JoplinImportDialog() {
    QSettings settings;
    settings.setValue(
        QStringLiteral("JoplinImport/ImageImportCheckBoxChecked"),
        ui->imageImportCheckBox->isChecked());
    settings.setValue(
        QStringLiteral("JoplinImport/AttachmentImportCheckBoxChecked"),
        ui->attachmentImportCheckBox->isChecked());

    delete ui;
}

/**
 * Imports the notes from a selected file
 */
void JoplinImportDialog::on_directoryButton_clicked() {
    FileDialog dialog(QStringLiteral("Joplin Import"));
    dialog.setFileMode(QFileDialog::DirectoryOnly);
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

    auto directoryName = ui->directoryLineEdit->text();
    QDir dir(directoryName);

    if (directoryName.isEmpty() || !dir.exists()) {
        return;
    }

    QStringList files = dir.entryList(QStringList() << "*.md", QDir::Files);
    qDebug() << __func__ << " - 'files': " << files;
    ui->importButton->setEnabled(false);
    ui->progressBar->setMaximum(files.count());
    ui->progressBar->show();

    foreach(QString fileName, files) {
        QCoreApplication::processEvents();
        QString filePath = dir.path() + "/" + fileName;
        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << file.errorString() << " (path: " << directoryName << ")";
            continue;
        }

        QTextStream ts(&file);
        const auto text = ts.readAll();
        auto textLines = text.split(QRegularExpression(
            QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));
        const auto id = fileName.remove(QRegularExpression("\\.md$"));

        // check if text contains a note
        if (text.contains(QRegularExpression(
                "^type_: 1$", QRegularExpression::MultilineOption))) {
            _noteData[id] = text;

            // don't increase the progress bar yet
            continue;
        }
        // check if text contains an image or attachment
        else if (text.contains(QRegularExpression(
                       "^type_: 4$", QRegularExpression::MultilineOption))) {
            if (text.contains(QRegularExpression(
                    "^mime: image/", QRegularExpression::MultilineOption))) {
                _imageData[id] = textLines[0];
            } else {
                _attachmentData[id] = textLines[0];
            }
        }
        // check if text contains a tag
        else if (text.contains(QRegularExpression(
            "^type_: 5$", QRegularExpression::MultilineOption))) {
            _tagData[id] = textLines[0];
        }
        // check if text contains a tag assignment
        else if (text.contains(QRegularExpression(
            "^type_: 6$", QRegularExpression::MultilineOption))) {
            auto noteIdMatch = QRegularExpression("^note_id: (.+)$",
                QRegularExpression::MultilineOption).match(text);
            QString noteId;

            if (noteIdMatch.hasMatch()) {
                noteId = noteIdMatch.captured(1);
            }

            auto tagIdMatch = QRegularExpression("^tag_id: (.+)$",
                QRegularExpression::MultilineOption).match(text);
            QString tagId;

            if (tagIdMatch.hasMatch()) {
                tagId = tagIdMatch.captured(1);
            }

            _tagAssignmentData[noteId].append(tagId);
        }

        ui->progressBar->setValue(ui->progressBar->value() + 1);
        QCoreApplication::processEvents();
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
    auto textLines = text.split(QRegularExpression(
        QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)")));

    auto title = textLines.at(0);
    title.remove(QRegularExpression("^[\\d+\\.]+"));
    auto noteText = Note::createNoteHeader(title.trimmed());
    noteText += text.trimmed();

    Note note = Note();
    note.setNoteText(noteText);

    // in case the user enabled that the filename can be different
    // from the note name
    // Attention: may already store the note
    note.handleNoteTextFileName();

    note.store();
    note.storeNoteTextFileToDisk();

    // tag the note if tags are found
    tagNote(id, note);

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

    foreach(QString tagId, tagAssignmentData) {
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
