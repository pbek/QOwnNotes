#include <services/owncloudservice.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include "sharedialog.h"
#include "ui_sharedialog.h"

ShareDialog::ShareDialog(Note note, QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::ShareDialog) {
    ui->setupUi(this);
    this->note = note;

    // update the share link checkbox and link url line edit
    updateDialog();
}

ShareDialog::~ShareDialog() {
    delete ui;
}

/**
 * Updates the share link checkbox and link url line edit
 */
void ShareDialog::updateDialog() {
    note.refetch();
    qDebug() << __func__ << " - 'note': " << note;

    const QSignalBlocker blocker(ui->linkCheckBox);
    Q_UNUSED(blocker)
    const QSignalBlocker blocker2(ui->linkUrlLineEdit);
    Q_UNUSED(blocker2);

    ui->linkCheckBox->setChecked(note.isShared());
    ui->linkUrlLineEdit->setVisible(note.isShared());
    ui->linkUrlLineEdit->setText(note.getShareUrl());
    ui->infoLabel1->setText(Utils::Misc::replaceOwnCloudText(
            ui->infoLabel1->text()));
}

/**
 * Shares or removes the share from the current note
 */
void ShareDialog::on_linkCheckBox_toggled(bool checked) {
    auto *ownCloud = OwnCloudService::instance();

    const QSignalBlocker blocker(ui->linkCheckBox);
    Q_UNUSED(blocker)

    // the checked status will be set by the callback function updateDialog()
    ui->linkCheckBox->setChecked(!checked);

    if (checked) {
        // share the note file
        ownCloud->shareNote(note, this);

        Utils::Gui::information(
                this, "",
                Utils::Misc::replaceOwnCloudText(
                        tr("Keep in mind that you still have to sync your "
                           "notes with your server by using the ownCloud "
                           "desktop sync tool to be able to share notes with "
                           "others!")), "share-sync-information");
    } else {
        // remove the share
        ownCloud->removeNoteShare(note, this);
    }
}
