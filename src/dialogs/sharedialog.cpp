#include "sharedialog.h"

#include <services/cloudservice.h>
#include <utils/gui.h>

#include <QAction>
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QToolTip>
#include <QUrl>

#include "ui_sharedialog.h"

ShareDialog::ShareDialog(const Note &note, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::ShareDialog) {
    ui->setupUi(this);
    afterSetupUI();
    this->note = note;

    // Allow copying and opening the shared link right from the link field
    QAction *copyAction = ui->linkUrlLineEdit->addAction(
        QIcon::fromTheme(QStringLiteral("edit-copy"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))),
        QLineEdit::TrailingPosition);
    copyAction->setToolTip(tr("Copy link to clipboard"));
    connect(copyAction, &QAction::triggered, this, [this]() {
        QGuiApplication::clipboard()->setText(ui->linkUrlLineEdit->text());
        QToolTip::showText(
            ui->linkUrlLineEdit->mapToGlobal(QPoint(0, ui->linkUrlLineEdit->height())),
            tr("The link was copied to the clipboard"), ui->linkUrlLineEdit);
    });

    QAction *openAction = ui->linkUrlLineEdit->addAction(
        QIcon::fromTheme(QStringLiteral("text-html"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/text-html.svg"))),
        QLineEdit::TrailingPosition);
    openAction->setToolTip(tr("Open link in the browser"));
    connect(openAction, &QAction::triggered, this,
            [this]() { QDesktopServices::openUrl(QUrl(ui->linkUrlLineEdit->text())); });

    // update the share link checkbox and link url line edit
    updateDialog();
}

ShareDialog::~ShareDialog() { delete ui; }

/**
 * Updates the share link checkbox and link url line edit
 */
void ShareDialog::updateDialog() {
    note.refetch();
    qDebug() << __func__ << " - 'note': " << note;

    const QSignalBlocker blocker(ui->linkCheckBox);
    Q_UNUSED(blocker)
    const QSignalBlocker blocker2(ui->linkUrlLineEdit);
    Q_UNUSED(blocker2)
    const QSignalBlocker blocker3(ui->editCheckBox);
    Q_UNUSED(blocker3)

    ui->linkCheckBox->setChecked(note.isShared());

    // The password can only be set when creating a new share
    ui->passwordLineEdit->setVisible(!note.isShared());

    if (note.isShared()) {
        ui->passwordLineEdit->clear();
    }

    ui->linkUrlLineEdit->setVisible(note.isShared());
    ui->linkUrlLineEdit->setText(note.getShareUrl());

    // The link actions only make sense if there is a link
    const bool hasLink = !note.getShareUrl().isEmpty();
    for (QAction *action : ui->linkUrlLineEdit->actions()) {
        action->setEnabled(hasLink);
    }

    ui->editCheckBox->setVisible(note.isShared());
    ui->editCheckBox->setChecked(note.isShareEditAllowed());
}

/**
 * Shares or removes the share from the current note
 */
void ShareDialog::on_linkCheckBox_toggled(bool checked) {
    auto *cloud = CloudService::instance();

    const QSignalBlocker blocker(ui->linkCheckBox);
    Q_UNUSED(blocker)

    // the checked status will be set by the callback function updateDialog()
    ui->linkCheckBox->setChecked(!checked);

    if (checked) {
        // share the note file
        cloud->shareNote(note, this, ui->passwordLineEdit->text());

        Utils::Gui::information(
            this, QString(),
            tr("Keep in mind that you still have to sync your notes with your server by using "
               "the Nextcloud / ownCloud desktop sync tool to be able to share notes with others!"),
            QStringLiteral("share-sync-information"));
    } else {
        // remove the share
        cloud->removeNoteShare(note, this);
    }
}

/**
 * Focuses the password line edit, used if sharing failed, because the server
 * might enforce passwords for shared links
 */
void ShareDialog::focusPasswordLineEdit() {
    if (!ui->passwordLineEdit->isVisible()) {
        return;
    }

    ui->passwordLineEdit->setFocus();
    ui->passwordLineEdit->selectAll();
}

void ShareDialog::on_editCheckBox_toggled(bool checked) {
    auto *cloud = CloudService::instance();
    unsigned int permissions = note.getSharePermissions();

    // set 2. bit
    const uint8_t bit = 1;

    if (checked) {
        permissions |= 1UL << bit;
    } else {
        permissions &= ~(1UL << bit);
    }

    note.setSharePermissions(permissions);
    note.store();

    qDebug() << __func__ << " - 'permissions': " << permissions;

    cloud->setPermissionsOnSharedNote(note, this);
}
