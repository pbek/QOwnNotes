#include "linkdialog.h"

#include <entities/note.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QSettings>
#include <QTimer>

#include "ui_linkdialog.h"

LinkDialog::LinkDialog(int page, const QString &dialogTitle, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::LinkDialog) {
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(page);
    on_tabWidget_currentChanged(page);

    // disallow ] characters, because they will break markdown links
    ui->nameLineEdit->setValidator(
        new QRegularExpressionValidator(QRegularExpression(R"([^\]]*)")));
    firstVisibleNoteListRow = 0;

    if (!dialogTitle.isEmpty()) {
        this->setWindowTitle(dialogTitle);
    }

    QStringList nameList = Note::fetchNoteNames();
    ui->searchLineEdit->installEventFilter(this);
    ui->headingSearchLineEdit->installEventFilter(this);
    ui->notesListWidget->installEventFilter(this);

    Q_FOREACH (Note note, Note::fetchAll()) {
        auto *item = new QListWidgetItem(note.getName());
        item->setData(Qt::UserRole, note.getId());
        ui->notesListWidget->addItem(item);
    }

    ui->notesListWidget->setCurrentRow(0);

    QClipboard *clipboard = QApplication::clipboard();
    const QString text = clipboard->text();
    const QUrl url(text);

    // set text from clipboard
    if (url.isValid() && !url.scheme().isEmpty()) {
        ui->urlEdit->setText(text);
    }

    setupFileUrlMenu();
}

LinkDialog::~LinkDialog() { delete ui; }

void LinkDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    // search notes when at least 2 characters were entered
    if (arg1.count() >= 2) {
        QVector<QString> noteNameList = Note::searchAsNameList(arg1, true);
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
    } else {    // show all items otherwise
        this->firstVisibleNoteListRow = 0;

        for (int i = 0; i < this->ui->notesListWidget->count(); ++i) {
            QListWidgetItem *item = this->ui->notesListWidget->item(i);
            item->setHidden(false);
        }
    }
}

QString LinkDialog::getSelectedNoteName() const {
    return ui->notesListWidget->currentRow() > -1 ? ui->notesListWidget->currentItem()->text()
                                                  : QString();
}

Note LinkDialog::getSelectedNote() const {
    if (ui->notesListWidget->currentRow() == -1) {
        return {};
    }

    const int noteId = ui->notesListWidget->currentItem()->data(Qt::UserRole).toInt();

    return Note::fetch(noteId);
}

QString LinkDialog::getSelectedHeading() const {
    return ui->headingListWidget->selectedItems().isEmpty()
               ? ""
               : ui->headingListWidget->currentItem()->text();
}

QString LinkDialog::getURL() const {
    QString url = ui->urlEdit->text().trimmed();

    if (!url.isEmpty() && !url.contains(QStringLiteral("://"))) {
        url = QStringLiteral("http://") + url;
    }

    return url;
}

QString LinkDialog::getLinkName() const { return ui->nameLineEdit->text().trimmed(); }

void LinkDialog::setLinkName(const QString &text) { ui->nameLineEdit->setText(text); }

QString LinkDialog::getLinkDescription() const { return ui->descriptionLineEdit->text().trimmed(); }

//
// Event filters on the NoteSearchDialog
//
bool LinkDialog::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->searchLineEdit) {
        if (event->type() == QEvent::KeyPress) {
            auto *keyEvent = dynamic_cast<QKeyEvent *>(event);

            // set focus to the notes list if Key_Down or Key_Tab were pressed
            // in the search line edit
            if ((keyEvent->key() == Qt::Key_Down) || (keyEvent->key() == Qt::Key_Tab)) {
                // choose another selected item if current item is invisible
                auto item = ui->notesListWidget->currentItem();
                if ((item != nullptr) && ui->notesListWidget->currentItem()->isHidden() &&
                    (this->firstVisibleNoteListRow >= 0)) {
                    ui->notesListWidget->setCurrentRow(this->firstVisibleNoteListRow);
                }

                // give the keyboard focus to the notes list widget
                ui->notesListWidget->setFocus();
                return true;
            }
        }
        return false;
    } else if (obj == ui->headingSearchLineEdit) {
        if (event->type() == QEvent::KeyPress) {
            auto *keyEvent = dynamic_cast<QKeyEvent *>(event);

            // set focus to the notes list if Key_Down or Key_Tab were pressed
            // in the search line edit
            if ((keyEvent->key() == Qt::Key_Down) || (keyEvent->key() == Qt::Key_Tab)) {
                // give the keyboard focus to the heading list widget
                ui->headingListWidget->setFocus();
                return true;
            }
        }
        return false;
    } else if (obj == ui->notesListWidget) {
        if (event->type() == QEvent::KeyPress) {
            auto *keyEvent = dynamic_cast<QKeyEvent *>(event);

            // set focus to the note text edit if Key_Return or Key_Tab
            // were pressed in the notes list
            if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Tab)) {
                ui->headingSearchLineEdit->setFocus();

                return true;
            }
        }
        return false;
    }

    return MasterDialog::eventFilter(obj, event);
}

void LinkDialog::doAccept() {
    ui->urlEdit->clear();
    this->close();
    this->setResult(QDialog::Accepted);
}

void LinkDialog::on_notesListWidget_doubleClicked(const QModelIndex &index) {
    Q_UNUSED(index)
    doAccept();
}

void LinkDialog::on_headingListWidget_doubleClicked(const QModelIndex &index) {
    Q_UNUSED(index)
    doAccept();
}

/**
 * @brief Fetches the title of a webpage
 * @param url
 * @return
 */
QString LinkDialog::getTitleForUrl(const QUrl &url) {
    const QString html = Utils::Misc::downloadUrl(url);

    if (html.isEmpty()) {
        return {};
    }

    // parse title from webpage
    QRegularExpression regex(QStringLiteral(R"(<title>(.*)<\/title>)"),
                             QRegularExpression::MultilineOption |
                                 QRegularExpression::DotMatchesEverythingOption |
                                 QRegularExpression::InvertedGreedinessOption);
    QRegularExpressionMatch match = regex.match(html);
    QString title = match.captured(1);

    // decode HTML entities
    title = Utils::Misc::unescapeHtml(std::move(title));

    // replace some other characters we don't want
    title.replace(QStringLiteral("["), QStringLiteral("("))
        .replace(QStringLiteral("]"), QStringLiteral(")"))
        .replace(QStringLiteral("<"), QStringLiteral("("))
        .replace(QStringLiteral(">"), QStringLiteral(")"))
        .replace(QStringLiteral("&#8211;"), QStringLiteral("-"))
        .replace(QStringLiteral("&#124;"), QStringLiteral("-"))
        .replace(QStringLiteral("&#038;"), QStringLiteral("&"))
        .replace(QStringLiteral("&#39;"), QStringLiteral("'"));

    // trim whitespaces and return title
    return title.simplified();
}

/**
 * Selects a local file to link to
 */
void LinkDialog::addFileUrl() {
    QSettings settings;
    // load last url
    QUrl fileUrl = settings.value(QStringLiteral("LinkDialog/lastSelectedFileUrl")).toUrl();

    if (Utils::Misc::isInPortableMode()) {
        fileUrl = QUrl(QStringLiteral("file://") +
                       Utils::Misc::prependPortableDataPathIfNeeded(Utils::Misc::removeIfStartsWith(
                           fileUrl.toLocalFile(), QStringLiteral("/"))));
    }

    fileUrl = QFileDialog::getOpenFileUrl(this, tr("Select file to link to"), fileUrl);
    QString fileUrlString = fileUrl.toString(QUrl::FullyEncoded);

    if (Utils::Misc::isInPortableMode()) {
        fileUrlString =
            QStringLiteral("file://") +
            QUrl(QStringLiteral("../") +
                 Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(fileUrl.toLocalFile()))
                .toString(QUrl::FullyEncoded);
    }

    if (!fileUrlString.isEmpty()) {
        // store url for the next time
        settings.setValue(QStringLiteral("LinkDialog/lastSelectedFileUrl"), fileUrlString);

        // write the file-url to the url text-edit
        ui->urlEdit->setText(fileUrlString);
    }
}

/**
 * Selects a local directory to link to
 */
void LinkDialog::addDirectoryUrl() {
    QSettings settings;
    // load last url
    QUrl directoryUrl =
        settings.value(QStringLiteral("LinkDialog/lastSelectedDirectoryUrl")).toUrl();

    if (Utils::Misc::isInPortableMode()) {
        directoryUrl =
            QUrl(QStringLiteral("file://") +
                 Utils::Misc::prependPortableDataPathIfNeeded(Utils::Misc::removeIfStartsWith(
                     directoryUrl.toLocalFile(), QStringLiteral("/"))));
    }

    directoryUrl =
        QFileDialog::getExistingDirectoryUrl(this, tr("Select directory to link to"), directoryUrl);
    QString directoryUrlString = directoryUrl.toString(QUrl::FullyEncoded);

    if (Utils::Misc::isInPortableMode()) {
        directoryUrlString =
            QStringLiteral("file://") +
            QUrl(QStringLiteral("../") + Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                                             directoryUrl.toLocalFile()))
                .toString(QUrl::FullyEncoded);
    }

    if (!directoryUrlString.isEmpty()) {
        // store url for the next time
        settings.setValue(QStringLiteral("LinkDialog/lastSelectedDirectoryUrl"),
                          directoryUrlString);

        // write the directory-url to the url text-edit
        ui->urlEdit->setText(directoryUrlString);
    }
}

void LinkDialog::on_urlEdit_textChanged(const QString &arg1) {
    auto url = QUrl(arg1);

    if (!url.isValid()) {
        return;
    }

    // try to get the title of the webpage if no link name was set
    if (url.scheme().startsWith(QStringLiteral("http")) && ui->nameLineEdit->text().isEmpty()) {
        const QString title = getTitleForUrl(url);

        if (!title.isEmpty()) {
            ui->nameLineEdit->setText(title);
        }
    }
}

void LinkDialog::setupFileUrlMenu() {
    auto *addMenu = new QMenu(this);

    QAction *addFileAction = addMenu->addAction(tr("Select file to link to"));
    addFileAction->setIcon(
        QIcon::fromTheme(QStringLiteral("document-open"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/document-open.svg"))));
    connect(addFileAction, SIGNAL(triggered()), this, SLOT(addFileUrl()));

    QAction *addDirectoryAction = addMenu->addAction(tr("Select directory to link to"));
    addDirectoryAction->setIcon(
        QIcon::fromTheme(QStringLiteral("folder"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/folder.svg"))));
    connect(addDirectoryAction, SIGNAL(triggered()), this, SLOT(addDirectoryUrl()));

    ui->fileUrlButton->setMenu(addMenu);
}

void LinkDialog::on_buttonBox_accepted() {
    if (ui->tabWidget->currentWidget() == ui->noteTab) {
        ui->urlEdit->clear();
    }
}

void LinkDialog::on_headingSearchLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInListWidget(ui->headingListWidget, arg1);
}

void LinkDialog::loadNoteHeadings() const {
    auto note = getSelectedNote();

    ui->headingListWidget->clear();
    ui->headingListWidget->addItems(note.getHeadingList());
}

void LinkDialog::on_notesListWidget_currentItemChanged(QListWidgetItem *current,
                                                       QListWidgetItem *previous) {
    Q_UNUSED(current)
    Q_UNUSED(previous)

    loadNoteHeadings();
}

void LinkDialog::on_tabWidget_currentChanged(int index) {
    if (index == 0) {
        ui->urlEdit->setFocus();
    } else {
        ui->searchLineEdit->setFocus();
    }
}
