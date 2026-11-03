#include "linkdialog.h"

#include <entities/note.h>
#include <entities/tag.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLocale>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QRegularExpression>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QWindow>

#include "entities/notefolder.h"
#include "mainwindow.h"
#include "services/scriptingservice.h"
#include "services/settingsservice.h"
#include "ui_linkdialog.h"
#include "widgets/navigationwidget.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
#include <memory>
#endif

namespace {
enum NoteListDataRoles {
    NoteIdRole = Qt::UserRole,
    NoteNameRole,
    NoteModifiedRole,
};

QString noteTagLookupKey(const QString &subFolderPath, const QString &noteName) {
    return subFolderPath + QStringLiteral("/") + noteName;
}

class NoteListTreeWidgetItem : public QTreeWidgetItem {
   public:
    using QTreeWidgetItem::QTreeWidgetItem;

    bool operator<(const QTreeWidgetItem &other) const override {
        if (treeWidget() != nullptr && treeWidget()->sortColumn() == 3) {
            const QDateTime thisModified = data(3, NoteModifiedRole).toDateTime();
            const QDateTime otherModified = other.data(3, NoteModifiedRole).toDateTime();

            return thisModified < otherModified;
        }

        return QTreeWidgetItem::operator<(other);
    }
};
}    // namespace

LinkDialog::LinkDialog(int page, const QString &dialogTitle, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::LinkDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->tabWidget->setCurrentIndex(page);
    on_tabWidget_currentChanged(page);
    ui->titleFetchWidget->hide();
    Utils::Gui::setMutedTextColor(ui->titleFetchStatusLabel);
    Utils::Gui::setMutedTextColor(ui->noteCountLabel);
    Utils::Gui::setMutedTextColor(ui->noHeadingsLabel);
    ui->noHeadingsLabel->hide();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Insert link"));
    _markdownTextEdit = new QOwnNotesMarkdownTextEdit();
    _networkManager = new QNetworkAccessManager(this);
    _namePlaceholderText = ui->nameLineEdit->placeholderText();

    // The title fetch is deferred, so the dialog can open immediately and typing in the URL
    // field doesn't start a request for every keystroke
    _titleFetchTimer = new QTimer(this);
    _titleFetchTimer->setSingleShot(true);
    connect(_titleFetchTimer, &QTimer::timeout, this, &LinkDialog::runPendingTitleFetch);

    _titleFetchStatusHideTimer = new QTimer(this);
    _titleFetchStatusHideTimer->setSingleShot(true);
    connect(_titleFetchStatusHideTimer, &QTimer::timeout, ui->titleFetchWidget, &QWidget::hide);

    connect(ui->cancelTitleFetchButton, &QToolButton::clicked, this, &LinkDialog::cancelTitleFetch);
    connect(this, &QDialog::finished, this, &LinkDialog::cancelTitleFetch);

    // Show the wiki-link checkbox only when wiki-link support is enabled
    ui->wikiLinkCheckBox->setVisible(Note::isWikiLinkSupportEnabled());

    // disallow ] characters, because they will break Markdown links
    ui->nameLineEdit->setValidator(
        new QRegularExpressionValidator(QRegularExpression(R"([^\]]*)")));
    firstVisibleNoteListRow = 0;

    if (!dialogTitle.isEmpty()) {
        this->setWindowTitle(dialogTitle);
    }

    ui->searchLineEdit->installEventFilter(this);
    ui->headingSearchLineEdit->installEventFilter(this);
    ui->notesListWidget->installEventFilter(this);
    ui->notesListWidget->setRootIsDecorated(false);

    // Change the search icon between dark and light mode
    const QString searchIconFileName =
        SettingsService().value(QStringLiteral("darkModeColors")).toBool()
            ? QStringLiteral("search-notes-dark.svg")
            : QStringLiteral("search-notes.svg");
    static const QRegularExpression searchIconRegex(
        QStringLiteral("background-image: url\\(:.+\\);"));

    QString searchLineEditStyleSheet = ui->searchLineEdit->styleSheet();
    searchLineEditStyleSheet.replace(
        searchIconRegex,
        QStringLiteral("background-image: url(:/images/%1);").arg(searchIconFileName));
    ui->searchLineEdit->setStyleSheet(searchLineEditStyleSheet);

    QString headingSearchLineEditStyleSheet = ui->headingSearchLineEdit->styleSheet();
    headingSearchLineEditStyleSheet.replace(
        searchIconRegex,
        QStringLiteral("background-image: url(:/images/%1);").arg(searchIconFileName));
    ui->headingSearchLineEdit->setStyleSheet(headingSearchLineEditStyleSheet);

    const bool showSubfolders = NoteFolder::isCurrentShowSubfolders();
    ui->notesListWidget->setColumnHidden(1, !showSubfolders);
    ui->notesListWidget->setSortingEnabled(true);
    ui->notesListWidget->sortByColumn(3, Qt::DescendingOrder);
    ui->notesListWidget->header()->setSortIndicatorShown(true);
    ui->notesListWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->notesListWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->notesListWidget->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->notesListWidget->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->notesListWidget->header()->setStretchLastSection(false);
    ui->notesListWidget->setColumnWidth(1, 180);
    ui->notesListWidget->setColumnWidth(2, 180);
    Utils::Gui::initTreeWidgetHeaderOrderPersistence(
        ui->notesListWidget, QStringLiteral("LinkDialog/notesListWidgetHeaderOrder"));
    const auto tagNamesByNoteFilePath = Tag::fetchAllNamesByNoteFilePath();

    Q_FOREACH (Note note, Note::fetchAll()) {
        const QString noteName = note.getName();
        const QString subFolderPath = note.relativeNoteSubFolderPath();
        const QString tagText =
            tagNamesByNoteFilePath.value(noteTagLookupKey(subFolderPath, noteName))
                .join(QStringLiteral(", "));
        const QDateTime modified = note.getFileLastModified();
        const QString modifiedDisplay = QLocale().toString(modified, QLocale::ShortFormat);
        auto *item = new NoteListTreeWidgetItem(ui->notesListWidget);
        item->setText(0, noteName);
        item->setText(1, subFolderPath);
        item->setText(2, tagText);
        item->setText(3, modifiedDisplay);
        item->setToolTip(2, tagText);
        item->setData(0, NoteIdRole, note.getId());
        item->setData(0, NoteNameRole, noteName);
        item->setData(3, NoteModifiedRole, modified);
    }

    if (ui->notesListWidget->topLevelItemCount() > 0) {
        ui->notesListWidget->setCurrentItem(ui->notesListWidget->topLevelItem(0));
    }

    updateNoteCountLabel();

    if (page == LinkDialog::TextLinkPage) {
        QClipboard *clipboard = QApplication::clipboard();
        const QString text = clipboard->text().remove(QStringLiteral("\n")).trimmed();
        const QUrl url(text);

        // set text from clipboard
        if (url.isValid() && !url.scheme().isEmpty()) {
            ui->urlEdit->setText(text);
        }
    }

    setupFileUrlMenu();
    updateOkButton();
}

LinkDialog::~LinkDialog() {
    abortTitleReply();
    delete ui;
    delete _markdownTextEdit;
}

void LinkDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    // search notes when at least 2 characters were entered
    if (arg1.size() >= 2) {
        QVector<QString> noteNameList = Note::searchAsNameList(arg1, true);
        this->firstVisibleNoteListRow = -1;

        for (int i = 0; i < this->ui->notesListWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem *item = this->ui->notesListWidget->topLevelItem(i);
            if (noteNameList.indexOf(item->data(0, NoteNameRole).toString()) < 0) {
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

        for (int i = 0; i < this->ui->notesListWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem *item = this->ui->notesListWidget->topLevelItem(i);
            item->setHidden(false);
        }
    }

    // Select the first matching note if the selected note doesn't match the search anymore,
    // so a hidden note can't be linked by accident
    QTreeWidgetItem *currentItem = ui->notesListWidget->currentItem();
    if (currentItem == nullptr || currentItem->isHidden()) {
        QTreeWidgetItem *firstVisibleItem = nullptr;

        for (int i = 0; i < ui->notesListWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem *item = ui->notesListWidget->topLevelItem(i);
            if (!item->isHidden()) {
                firstVisibleItem = item;
                break;
            }
        }

        ui->notesListWidget->setCurrentItem(firstVisibleItem);

        if (firstVisibleItem == nullptr) {
            ui->notesListWidget->clearSelection();
            ui->headingListWidget->clear();
            ui->noHeadingsLabel->hide();
        }
    }

    updateNoteCountLabel();
    updateOkButton();
}

/**
 * Shows how many notes are in the list, and how many of them match the search
 */
void LinkDialog::updateNoteCountLabel() {
    const int total = ui->notesListWidget->topLevelItemCount();

    if (ui->searchLineEdit->text().size() < 2) {
        ui->noteCountLabel->setText(tr("%n note(s)", "", total));
        return;
    }

    int visible = 0;
    for (int i = 0; i < total; ++i) {
        if (!ui->notesListWidget->topLevelItem(i)->isHidden()) {
            ++visible;
        }
    }

    ui->noteCountLabel->setText(tr("%1 of %n note(s)", "", total).arg(visible));
}

/**
 * Only allows inserting a link if there is a URL or a note to link to
 */
void LinkDialog::updateOkButton() {
    bool canInsert;

    if (ui->tabWidget->currentWidget() == ui->noteTab) {
        const QTreeWidgetItem *item = ui->notesListWidget->currentItem();
        canInsert = item != nullptr && !item->isHidden();
    } else {
        canInsert = !ui->urlEdit->text().trimmed().isEmpty();
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(canInsert);
}

QString LinkDialog::getSelectedNoteName() const {
    const QTreeWidgetItem *item = ui->notesListWidget->currentItem();

    return item != nullptr && !item->isHidden() ? item->data(0, NoteNameRole).toString()
                                                : QString();
}

Note LinkDialog::getSelectedNote() const {
    if (ui->notesListWidget->currentItem() == nullptr) {
        return {};
    }

    const int noteId = ui->notesListWidget->currentItem()->data(0, NoteIdRole).toInt();

    return Note::fetch(noteId);
}

QString LinkDialog::getSelectedHeading() const {
    // Trim the heading text, in case there are trailing carriage return characters leaking in
    // Windows
    return ui->headingListWidget->selectedItems().isEmpty()
               ? ""
               : ui->headingListWidget->currentItem()->text().trimmed();
}

QString LinkDialog::getURL() const {
    QString url = ui->urlEdit->text().trimmed();

    if (!url.isEmpty() && !url.contains(QStringLiteral("://")) &&
        !url.startsWith(QStringLiteral("./"))) {
        url = QStringLiteral("https://") + url;
    }

    return url;
}

void LinkDialog::setURL(const QString &text) { ui->urlEdit->setText(text); }

QString LinkDialog::getLinkName() const { return ui->nameLineEdit->text().trimmed(); }

void LinkDialog::setLinkName(const QString &text) { ui->nameLineEdit->setText(text); }

QString LinkDialog::getLinkDescription() const { return ui->descriptionLineEdit->text().trimmed(); }

bool LinkDialog::isWikiLink() const {
    return Note::isWikiLinkSupportEnabled() && ui->wikiLinkCheckBox->isChecked();
}

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
                    ui->notesListWidget->setCurrentItem(
                        ui->notesListWidget->topLevelItem(this->firstVisibleNoteListRow));
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
                if (ui->headingSearchLineEdit->isEnabled()) {
                    ui->headingSearchLineEdit->setFocus();
                } else if (keyEvent->key() == Qt::Key_Return) {
                    // There are no headings to choose from, so the link can be inserted right away
                    doAccept();
                } else {
                    ui->nameLineEdit->setFocus();
                }

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
 * @brief Returns the title of an html page
 *
 * @param html
 * @return
 */
QString LinkDialog::getTitleFromHtml(const QString &html) {
    if (html.isEmpty()) {
        return {};
    }

    // parse title from webpage
    QRegularExpression regex(QStringLiteral(R"(<title.*>(.*)<\/title>)"),
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
        .replace(QStringLiteral("&#8211;"), QStringLiteral("-"))
        .replace(QStringLiteral("&#124;"), QStringLiteral("-"))
        .replace(QStringLiteral("&#038;"), QStringLiteral("&"))
        .replace(QStringLiteral("&#39;"), QStringLiteral("'"));

    // trim whitespaces and return title
    return title.simplified();
}

/**
 * Shows the download progress
 */
void LinkDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    // Many servers don't send a content length, keep the busy indicator in that case
    if (bytesTotal <= 0) {
        ui->downloadProgressBar->setRange(0, 0);
        ui->downloadProgressBar->setToolTip(Utils::Misc::toHumanReadableByteSize(bytesReceived));
        return;
    }

    ui->downloadProgressBar->setRange(0, static_cast<int>(bytesTotal / 1024));
    ui->downloadProgressBar->setValue(static_cast<int>(bytesReceived / 1024));
    ui->downloadProgressBar->setToolTip(Utils::Misc::toHumanReadableByteSize(bytesReceived) +
                                        QStringLiteral(" / ") +
                                        Utils::Misc::toHumanReadableByteSize(bytesTotal));
}

/**
 * Collects the downloaded data and stops the download as soon as the title is known,
 * so we don't have to wait for big webpages to be downloaded completely
 */
void LinkDialog::slotReplyReadyRead() {
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply == nullptr || reply != _titleReply) {
        return;
    }

    _titleReplyData += reply->readAll();

    if (!_titleReplyData.toLower().contains("</title>")) {
        return;
    }

    const QString title = getTitleFromHtml(QString::fromUtf8(_titleReplyData));
    abortTitleReply();
    finishTitleFetch(title);
}

void LinkDialog::slotReplyFinished(QNetworkReply *reply) {
    // Ignore replies of requests that were already superseded or cancelled
    if (reply == nullptr || reply != _titleReply) {
        return;
    }

    _titleReplyData += reply->readAll();
    const QNetworkReply::NetworkError error = reply->error();
    const QString errorString = reply->errorString();
    abortTitleReply();

    if (error != QNetworkReply::NoError) {
        qWarning() << QStringLiteral("Network error: %1").arg(errorString);
        finishTitleFetch(QString(), tr("Could not fetch the title: %1").arg(errorString));
        return;
    }

    const QString title = getTitleFromHtml(QString::fromUtf8(_titleReplyData));
    finishTitleFetch(title, title.isEmpty() ? tr("The webpage has no title") : QString());
}

/**
 * Selects a local file to link to
 */
void LinkDialog::addFileUrl(bool relative) {
    SettingsService settings;
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

        fileUrl = QUrl(fileUrlString);
        if (relative && fileUrl.isLocalFile()) {
            auto note = MainWindow::instance()->getCurrentNote();
            // Make path relative to the current note
            QString relativePath = note.relativeFilePath(fileUrl.toLocalFile());
            fileUrlString = QStringLiteral("./") + relativePath;

            // Also set the link name if it's empty
            if (ui->nameLineEdit->text().isEmpty()) {
                QFileInfo fileInfo(relativePath);
                ui->nameLineEdit->setText(fileInfo.fileName());
            }
        }

        // write the file-url to the url text-edit
        ui->urlEdit->setText(fileUrlString);
    }
}

/**
 * Selects a local directory to link to
 */
void LinkDialog::addDirectoryUrl(bool relative) {
    SettingsService settings;
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

        directoryUrl = QUrl(directoryUrlString);
        if (relative && directoryUrl.isLocalFile()) {
            auto note = MainWindow::instance()->getCurrentNote();
            // Make path relative to the current note
            QString relativePath = note.relativeFilePath(directoryUrl.toLocalFile());
            directoryUrlString = QStringLiteral("./") + relativePath;

            // Also set the link name if it's empty
            if (ui->nameLineEdit->text().isEmpty()) {
                QFileInfo directoryInfo(directoryUrl.toLocalFile());
                ui->nameLineEdit->setText(directoryInfo.fileName());
            }
        }

        // write the directory-url to the url text-edit
        ui->urlEdit->setText(directoryUrlString);
    }
}

void LinkDialog::on_urlEdit_textChanged(const QString &arg1) {
    updateOkButton();
    const QUrl url(arg1.trimmed());

    // Try to get the title of the webpage if no link name was set
    if (url.isValid() && url.scheme().startsWith(QStringLiteral("http")) && !url.host().isEmpty() &&
        ui->nameLineEdit->text().isEmpty()) {
        // Fetch right after the dialog was shown for prefilled URLs, but wait for the user
        // to stop typing otherwise
        scheduleTitleFetch(url, isVisible() ? 500 : 0);
    } else {
        cancelTitleFetch();
    }
}

void LinkDialog::on_nameLineEdit_textEdited(const QString &text) {
    // The user is entering a name themselves, so the fetched title isn't needed anymore
    if (!text.isEmpty()) {
        cancelTitleFetch();
    }
}

void LinkDialog::setupFileUrlMenu() {
    // std::make_unique needs C++14
    auto addMenu = std::unique_ptr<QMenu>(new QMenu(this));

    QAction *addFileRelativeAction = addMenu->addAction(tr("Select file to link to (relative)"));
    addFileRelativeAction->setIcon(
        QIcon::fromTheme(QStringLiteral("document-open"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/document-open.svg"))));
    connect(addFileRelativeAction, &QAction::triggered, this, [this]() { addFileUrl(true); });

    QAction *addFileAbsoluteAction = addMenu->addAction(tr("Select file to link to (absolute)"));
    addFileAbsoluteAction->setIcon(
        QIcon::fromTheme(QStringLiteral("document-open"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/document-open.svg"))));
    connect(addFileAbsoluteAction, SIGNAL(triggered()), this, SLOT(addFileUrl()));

    QAction *addDirectoryRelativeAction =
        addMenu->addAction(tr("Select directory to link to (relative)"));
    addDirectoryRelativeAction->setIcon(
        QIcon::fromTheme(QStringLiteral("folder"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/folder.svg"))));
    connect(addDirectoryRelativeAction, &QAction::triggered, this,
            [this]() { addDirectoryUrl(true); });

    QAction *addDirectoryAbsoluteAction =
        addMenu->addAction(tr("Select directory to link to (absolute)"));
    addDirectoryAbsoluteAction->setIcon(
        QIcon::fromTheme(QStringLiteral("folder"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/folder.svg"))));
    connect(addDirectoryAbsoluteAction, SIGNAL(triggered()), this, SLOT(addDirectoryUrl()));

    ui->fileUrlButton->setMenu(addMenu.release());
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
    _markdownTextEdit->setPlainText(note.getNoteText());
    auto nodes = NavigationWidget::parseDocument(_markdownTextEdit->document());
    QStringList headingTexts;
    std::transform(nodes.begin(), nodes.end(), std::back_inserter(headingTexts),
                   [](const Node &node) { return node.text; });

    ui->headingListWidget->clear();
    ui->headingListWidget->addItems(headingTexts);

    // Show a hint instead of an empty heading list
    const bool hasHeadings = !headingTexts.isEmpty();
    ui->headingSearchLineEdit->setEnabled(hasHeadings);
    ui->headingListWidget->setVisible(hasHeadings);
    ui->noHeadingsLabel->setVisible(!hasHeadings && note.isFetched());
}

void LinkDialog::on_notesListWidget_currentItemChanged(QTreeWidgetItem *current,
                                                       QTreeWidgetItem *previous) {
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

    updateOkButton();
}

void LinkDialog::scheduleTitleFetch(const QUrl &url, int delayMs) {
    abortTitleReply();
    _pendingTitleUrl = url;
    _titleFetchTimer->start(delayMs);
}

void LinkDialog::runPendingTitleFetch() {
    if (_pendingTitleUrl.isEmpty()) {
        return;
    }

    // Wait until the dialog is really on screen, so the user never waits for a hidden dialog
    const QWindow *window = windowHandle();
    if (!isVisible() || window == nullptr || !window->isExposed()) {
        _titleFetchTimer->start(20);
        return;
    }

    const QUrl url = _pendingTitleUrl;
    _pendingTitleUrl.clear();
    startTitleFetchRequest(url);
}

/**
 * Shows the title fetch progress below the URL
 */
void LinkDialog::setTitleFetchInProgress(const QUrl &url) {
    _titleFetchStatusHideTimer->stop();
    ui->downloadProgressBar->setRange(0, 0);
    ui->downloadProgressBar->setToolTip(QString());
    ui->downloadProgressBar->show();
    ui->titleFetchStatusLabel->setText(tr("Fetching the title from %1…").arg(url.host()));
    ui->titleFetchStatusLabel->setToolTip(url.toString());
    ui->cancelTitleFetchButton->show();
    ui->titleFetchWidget->show();
    ui->nameLineEdit->setPlaceholderText(tr("Fetching the title of the webpage…"));
}

/**
 * Ends the title fetch progress, sets the title and shows an error for a few seconds if
 * there was one
 */
void LinkDialog::finishTitleFetch(const QString &title, const QString &errorText) {
    ui->nameLineEdit->setPlaceholderText(_namePlaceholderText);

    if (!title.isEmpty() && getLinkName().isEmpty()) {
        setLinkName(title);
    }

    if (errorText.isEmpty()) {
        ui->titleFetchWidget->hide();
        return;
    }

    ui->downloadProgressBar->hide();
    ui->cancelTitleFetchButton->hide();
    ui->titleFetchStatusLabel->setText(errorText);
    ui->titleFetchStatusLabel->setToolTip(errorText);
    ui->titleFetchWidget->show();
    _titleFetchStatusHideTimer->start(5000);
}

void LinkDialog::abortTitleReply() {
    if (_titleReply.isNull()) {
        _titleReplyData.clear();
        return;
    }

    QNetworkReply *reply = _titleReply;
    _titleReply = nullptr;
    disconnect(reply, nullptr, this, nullptr);
    reply->abort();
    reply->deleteLater();
}

void LinkDialog::cancelTitleFetch() {
    const bool wasFetching = _titleFetchTimer->isActive() || !_titleReply.isNull();
    _titleFetchTimer->stop();
    _pendingTitleUrl.clear();
    abortTitleReply();
    _titleReplyData.clear();

    if (wasFetching) {
        finishTitleFetch(QString());
    }
}

void LinkDialog::startTitleFetchRequest(const QUrl &url) {
    abortTitleReply();
    _titleReplyData.clear();
    setTitleFetchInProgress(url);

    // Scripts are called synchronously, so paint the progress before calling them
    repaint();

    const QString title =
        ScriptingService::instance()->callFetchUrlTitleHook(url.toString(QUrl::FullyEncoded));

    // The user might have changed the URL or typed a name in the meantime
    if (QUrl(ui->urlEdit->text().trimmed()) != url || !getLinkName().isEmpty()) {
        finishTitleFetch(QString());
        return;
    }

    if (!title.isEmpty()) {
        finishTitleFetch(title);
        return;
    }

    QNetworkRequest networkRequest(url);

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    _networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#else
    networkRequest.setTransferTimeout(15000);
#endif

    QNetworkReply *reply = _networkManager->get(networkRequest);
    _titleReply = reply;

    connect(reply, &QNetworkReply::readyRead, this, &LinkDialog::slotReplyReadyRead);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { slotReplyFinished(reply); });
    connect(reply, &QNetworkReply::downloadProgress, this, &LinkDialog::downloadProgress);
}

void LinkDialog::on_refreshButton_clicked() {
    ui->nameLineEdit->clear();
    const QUrl url(ui->urlEdit->text().trimmed());

    if (url.isValid() && url.scheme().startsWith(QStringLiteral("http"))) {
        scheduleTitleFetch(url, 0);
    }
}
