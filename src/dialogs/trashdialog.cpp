#include "trashdialog.h"

#include <services/cloudservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QJSValue>
#include <QJSValueIterator>
#include <QPushButton>
#include <QSplitter>

#include "libraries/qmarkdowntextedit/qplaintexteditsearchwidget.h"
#include "mainwindow.h"
#include "services/settingsservice.h"
#include "ui_trashdialog.h"

namespace {
enum ItemRole { NoteTextRole = Qt::UserRole + 1, TimestampRole, DateStringRole };
}

TrashDialog::TrashDialog(const QJSValue &notes, QWidget *parent) : TrashDialog(parent) {
    setNotes(notes);
}

TrashDialog::TrashDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::TrashDialog) {
    ui->setupUi(this);
    afterSetupUI();

    // init the note browser search frame
    ui->noteBrowser->initSearchFrame(ui->noteBrowserSearchFrame);

    setupMainSplitter();

    Utils::Gui::setSectionHeaderFont(ui->previewHeaderLabel);
    Utils::Gui::setMutedTextColor(ui->previewDetailLabel);
    ui->previewDetailLabel->setTextFormat(Qt::RichText);
    Utils::Gui::setMutedTextColor(ui->trashCountLabel);
    ui->noteBrowser->setPlaceholderText(tr("Select a trashed note to see its text"));

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Restore selected note on server"));
    button->setToolTip(
        tr("<h3>Slower, but with note versions</h3>"
           "<p>The note will be restored on your Nextcloud / ownCloud "
           "server with all versions.</p>"
           "<p>You will have to wait until it is synced to "
           "QOwnNotes by Nextcloud / ownCloud sync.</p>"));
    button->setProperty("ActionRole", RestoreOnServer);
    button->setDefault(false);
    _actionButtons.append(button);
    button->setIcon(QIcon::fromTheme(QStringLiteral("view-restore"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/view-restore.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Download selected note"));
    button->setToolTip(
        tr("<h3>Faster, but without versions</h3>"
           "<p>The note will be created with the text from the preview.</p>"
           "<p>The note versions on your Nextcloud / ownCloud server will not be "
           "restored and the note will remain in the trash.</p>"
           "<p>You can always restore the note and its versions later.</p>"));
    button->setProperty("ActionRole", Download);
    button->setDefault(false);
    _actionButtons.append(button);
    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-download"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Delete"));
    button->setToolTip(tr("Delete selected note on server"));
    button->setProperty("ActionRole", DeleteOnServer);
    button->setDefault(false);
    _actionButtons.append(button);
    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-delete.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Cancel"));
    button->setProperty("ActionRole", Cancel);
    button->setIcon(QIcon::fromTheme(QStringLiteral("dialog-cancel"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
    button->setDefault(true);
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));
    connect(this, SIGNAL(finished(int)), this, SLOT(storeSettings()));

    updatePreviewInfo();
}

/**
 * Fills the trash list, can also be called after the dialog was opened
 */
void TrashDialog::setNotes(const QJSValue &notes) {
    QString itemName;
    QString dateString;
    QString data;
    int timestamp;
    ui->trashListWidget->clear();

    // init the iterator for the versions
    QJSValueIterator notesIterator(notes);

    // iterate over the trashed notes
    while (notesIterator.hasNext()) {
        notesIterator.next();

        QJSValue property = notesIterator.value().property(QStringLiteral("noteName"));

        if (property.isUndefined()) {
            continue;
        }

        itemName = property.toString();

        if (itemName.isEmpty()) {
            continue;
        }

        dateString = notesIterator.value().property(QStringLiteral("dateString")).toString();
        data = notesIterator.value().property(QStringLiteral("data")).toString();
        timestamp = notesIterator.value().property(QStringLiteral("timestamp")).toInt();
        QString fileName = notesIterator.value().property(QStringLiteral("fileName")).toString();

        auto *item = new QListWidgetItem();
        item->setText(itemName);
        item->setData(Qt::UserRole, fileName);
        item->setData(NoteTextRole, data);
        item->setData(TimestampRole, timestamp);
        item->setData(DateStringRole, dateString);
        item->setToolTip(dateString);
        ui->trashListWidget->addItem(item);
    }

    ui->trashListWidget->setCurrentRow(0);
    updatePreviewInfo();
}

void TrashDialog::setupMainSplitter() {
    trashSplitter = new QSplitter;
    trashSplitter->setChildrenCollapsible(false);

    trashSplitter->addWidget(ui->listFrame);
    trashSplitter->addWidget(ui->noteBrowserFrame);

    // Restore the splitter sizes, give the preview more room by default
    SettingsService settings;
    QByteArray state = settings.value(QStringLiteral("trashSplitterSizes")).toByteArray();
    if (!trashSplitter->restoreState(state)) {
        trashSplitter->setStretchFactor(0, 1);
        trashSplitter->setStretchFactor(1, 2);
    }

    ui->gridLayout->layout()->addWidget(trashSplitter);
    ui->gridLayout->layout()->addWidget(ui->buttonBox);
}

void TrashDialog::storeSettings() {
    // store the splitter sizes
    SettingsService settings;
    settings.setValue(QStringLiteral("trashSplitterSizes"), trashSplitter->saveState());
}

TrashDialog::~TrashDialog() { delete ui; }

void TrashDialog::on_trashListWidget_currentRowChanged(int currentRow) {
    auto *item = ui->trashListWidget->item(currentRow);
    ui->noteBrowser->setPlainText(
        item != nullptr && !item->isHidden() ? item->data(NoteTextRole).toString() : QString());
    updatePreviewInfo();

    if (!ui->searchLineEdit->text().isEmpty()) {
        // Highlight and jump to the query without taking focus from the trash filter
        auto *searchWidget = ui->noteBrowser->searchWidget();
        searchWidget->setSearchMode(QPlainTextEditSearchWidget::PlainTextMode);
        searchWidget->setSearchText(ui->searchLineEdit->text());
        searchWidget->doSearchCount();
        searchWidget->activate(false);
        ui->noteBrowser->ensureCursorVisible();
    }
}

/**
 * Updates the preview header, the trashed note count and the action buttons
 */
void TrashDialog::updatePreviewInfo() {
    const int total = ui->trashListWidget->count();
    int visible = 0;
    for (int row = 0; row < total; ++row) {
        if (!ui->trashListWidget->item(row)->isHidden()) {
            ++visible;
        }
    }

    ui->trashCountLabel->setText(ui->searchLineEdit->text().isEmpty()
                                     ? tr("%n trashed note(s)", "", total)
                                     : tr("%1 of %n trashed note(s)", "", total).arg(visible));

    const auto *item = ui->trashListWidget->currentItem();
    const bool hasNote = item != nullptr && !item->isHidden();

    for (auto *button : Utils::asConst(_actionButtons)) {
        button->setEnabled(hasNote);
    }

    if (hasNote) {
        ui->previewHeaderLabel->setText(item->text());
        const QString dateString = item->data(DateStringRole).toString();
        ui->previewDetailLabel->setText(
            dateString.isEmpty() ? QString() : tr("Trashed: %1").arg(dateString.toHtmlEscaped()));
    } else {
        ui->previewHeaderLabel->setText(total == 0     ? tr("No trashed notes")
                                        : visible == 0 ? tr("No trashed note matches the search")
                                                       : tr("No trashed note selected"));
        ui->previewDetailLabel->clear();
    }

    ui->previewDetailLabel->setHidden(ui->previewDetailLabel->text().isEmpty());
}

void TrashDialog::dialogButtonClicked(QAbstractButton *button) {
    if (ui->trashListWidget->currentItem() != nullptr &&
        !ui->trashListWidget->currentItem()->isHidden()) {
        int actionRole = button->property("ActionRole").toInt();

        QString name = ui->trashListWidget->currentItem()->text();
        QString fileName = ui->trashListWidget->currentItem()->data(Qt::UserRole).toString();

        switch (actionRole) {
            case Download: {
#ifndef INTEGRATION_TESTS
                QString text = ui->trashListWidget->currentItem()->data(NoteTextRole).toString();
                MainWindow::instance()->createNewNote(name, text);
#endif
                break;
            }

            case RestoreOnServer: {
                int timestamp = ui->trashListWidget->currentItem()->data(TimestampRole).toInt();
                qDebug() << name << timestamp;

#ifndef INTEGRATION_TESTS
                MainWindow::instance()->restoreTrashedNoteOnServer(fileName, timestamp);
#endif
                break;
            }

            case DeleteOnServer: {
                if (Utils::Gui::question(this, tr("Delete note on server"),
                                         tr("Delete selected trashed note on server?"),
                                         "trashdialog-delete", QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No) == QMessageBox::No) {
                    return;
                }

                const int timestamp =
                    ui->trashListWidget->currentItem()->data(TimestampRole).toInt();
                auto currentItem = ui->trashListWidget->currentItem();
                CloudService *cloud = CloudService::instance();
                ui->trashListWidget->setDisabled(true);
                ui->buttonBox->setDisabled(true);

                // delete trashed note on server
                const int statusCode = cloud->deleteTrashedNoteOnServer(fileName, timestamp);

                if (statusCode >= 200 && statusCode < 300) {
                    delete currentItem;
                    on_searchLineEdit_textChanged(ui->searchLineEdit->text());
                } else {
                    Utils::Gui::warning(this, tr("Error while deleting note"),
                                        tr("Deleting trashed note failed with status code: %1")
                                            .arg(QString::number(statusCode)),
                                        "trashdialog-delete-failed");
                }

                ui->trashListWidget->setDisabled(false);
                ui->buttonBox->setDisabled(false);

                return;
            }

            default:
                break;
        }
    }

    this->close();
}

void TrashDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    if (arg1.isEmpty()) {
        ui->noteBrowser->searchWidget()->deactivate();
        ui->searchLineEdit->setFocus();
    }

    int firstMatchingRow = -1;

    // Keep note text and restoration metadata attached to each filtered item
    for (int row = 0; row < ui->trashListWidget->count(); ++row) {
        auto *item = ui->trashListWidget->item(row);
        const bool matches =
            item->text().contains(arg1, Qt::CaseInsensitive) ||
            item->data(NoteTextRole).toString().contains(arg1, Qt::CaseInsensitive);
        if (!matches) {
            item->setSelected(false);
        }
        item->setHidden(!matches);
        if (matches && firstMatchingRow == -1) {
            firstMatchingRow = row;
        }
    }

    auto *current = ui->trashListWidget->currentItem();
    if (current == nullptr || current->isHidden()) {
        ui->trashListWidget->setCurrentRow(firstMatchingRow);
    }
    on_trashListWidget_currentRowChanged(ui->trashListWidget->currentRow());
}
