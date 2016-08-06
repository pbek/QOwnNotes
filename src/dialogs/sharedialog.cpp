#include <services/owncloudservice.h>
#include "sharedialog.h"
#include "ui_sharedialog.h"

ShareDialog::ShareDialog(Note note, QWidget *parent) :
    QDialog(parent),
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
    Q_UNUSED(blocker);
    const QSignalBlocker blocker2(ui->linkUrlLineEdit);
    Q_UNUSED(blocker2);

    ui->linkCheckBox->setChecked(note.isShared());
    ui->linkUrlLineEdit->setVisible(note.isShared());
    ui->linkUrlLineEdit->setText(note.getShareUrl());
}

/**
 * Shares or removes the share from the current note
 */
void ShareDialog::on_linkCheckBox_toggled(bool checked) {
    OwnCloudService *ownCloud = new OwnCloudService(this);

    const QSignalBlocker blocker(ui->linkCheckBox);
    Q_UNUSED(blocker);

    // the checked status will be set by the callback function updateDialog()
    ui->linkCheckBox->setChecked(!checked);

    if (checked) {
        // share the note file
        ownCloud->shareNote(note, this);
    } else {
        // remove the share
        ownCloud->removeNoteShare(note, this);
    }
}
