#include "linkdialog.h"
#include "ui_linkdialog.h"
#include <QKeyEvent>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileDialog>
#include "entities/note.h"
#include "helpers/htmlentities.h"

LinkDialog::LinkDialog(QString dialogTitle, QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::LinkDialog) {
    ui->setupUi(this);

    this->firstVisibleNoteListRow = 0;

    if (dialogTitle != "") {
        this->setWindowTitle(dialogTitle);
    }

    QStringList nameList = Note::fetchNoteNames();
    ui->searchLineEdit->installEventFilter(this);
    ui->notesListWidget->addItems(nameList);
    ui->notesListWidget->setCurrentRow(0);
}

LinkDialog::~LinkDialog() {
    delete ui;
}

void LinkDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    // search notes when at least 2 characters were entered
    if (arg1.count() >= 2) {
        QList<QString> noteNameList = Note::searchAsNameList(arg1, true);
        this->firstVisibleNoteListRow = -1;

        for (int i = 0; i < this->ui->notesListWidget->count(); ++i) {
            QListWidgetItem *item = this->ui->notesListWidget->item(i);
            if (noteNameList.indexOf(item->text()) < 0) {
                item->setHidden(true);
            } else {
                if (this->firstVisibleNoteListRow < 0) {
                    this->firstVisibleNoteListRow = i;
                }
                item->setHidden(false);
            }
        }
    } else {  // show all items otherwise
        this->firstVisibleNoteListRow = 0;

        for (int i = 0; i < this->ui->notesListWidget->count(); ++i) {
            QListWidgetItem *item = this->ui->notesListWidget->item(i);
            item->setHidden(false);
        }
    }
}

QString LinkDialog::getSelectedNoteName() {
    return ui->notesListWidget->currentRow() > -1
           ? ui->notesListWidget->currentItem()->text() : "";
}

QString LinkDialog::getURL() {
    return ui->urlEdit->text();
}

//
// Event filters on the NoteSearchDialog
//
bool LinkDialog::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->searchLineEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            // set focus to the notes list if Key_Down or Key_Tab were pressed
            // in the search line edit
            if ((keyEvent->key() == Qt::Key_Down) ||
                (keyEvent->key() == Qt::Key_Tab)) {
                // choose an other selected item if current item is invisible
                QListWidgetItem *item = ui->notesListWidget->currentItem();
                if ((item != NULL) &&
                    ui->notesListWidget->currentItem()->isHidden() &&
                    (this->firstVisibleNoteListRow >= 0)) {
                    ui->notesListWidget->setCurrentRow(
                            this->firstVisibleNoteListRow);
                }

                // give the keyboard focus to the notes list widget
                ui->notesListWidget->setFocus();
                return true;
            }
        }
        return false;
    } else if (obj == ui->notesListWidget) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            // set focus to the note text edit if Key_Return or Key_Tab
            // were pressed in the notes list
            if ((keyEvent->key() == Qt::Key_Return) ||
                (keyEvent->key() == Qt::Key_Tab)) {
                // focusNoteTextEdit();
                return true;
            }
        }
        return false;
    }

    return LinkDialog::eventFilter(obj, event);
}

void LinkDialog::on_notesListWidget_doubleClicked(const QModelIndex &index) {
    Q_UNUSED(index);
    this->close();
    this->setResult(QDialog::Accepted);
}

extern "C" size_t decode_html_entities_utf8(char *dest, const char *src);

/**
 * @brief Fetches the title of a webpage
 * @param url
 * @return
 */
QString LinkDialog::getTitleForUrl(QUrl url) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 5 sec timeout for the request
    timer.start(5000);

    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    loop.exec();

    // if we didn't get a timeout let's fetch the title of the webpage
    if (timer.isActive()) {
        // get the text from the network reply
        QString html = reply->readAll();

        // parse title from webpage
        QRegularExpression regex("<title>(.*)</title>");
        QRegularExpressionMatch match = regex.match(html);
        QString title = match.captured(1);

        // decode HTML entities
        HTMLEntities htmlEntities;
        title = htmlEntities.decodeHtmlEntities(title);

        // replace some other characters we don't want
        title.replace("[", "(")
                .replace("]", ")")
                .replace("<", "(")
                .replace(">", ")");

        // trim whitespaces and return title
        return title.simplified();
    }

    // timer elapsed, no reply from network request
    return "";
}

/**
 * @brief Selects a local file to link to
 */
void LinkDialog::on_fileUrlButton_clicked() {
    QUrl fileUrl = QFileDialog::getOpenFileUrl(this,
                                               tr("Select file to link to"));
    QString fileUrlString = fileUrl.toString();

    if (fileUrlString != "") {
        ui->urlEdit->setText(fileUrlString);
    }
}
