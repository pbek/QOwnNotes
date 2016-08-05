#include "sharedialog.h"
#include "ui_sharedialog.h"

ShareDialog::ShareDialog(Note note, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareDialog) {
    ui->setupUi(this);
    this->note = note;
    QString shareUrl = note.getShareUrl();

    const QSignalBlocker blocker(ui->linkCheckBox);
    Q_UNUSED(blocker);
    const QSignalBlocker blocker2(ui->linkUrlLineEdit);
    Q_UNUSED(blocker2);

    // init the share link checkbox and link url line edit
    bool isShared = note.isShared();
    ui->linkCheckBox->setChecked(isShared);
    ui->linkUrlLineEdit->setVisible(isShared);
    ui->linkUrlLineEdit->setText(shareUrl);
}

ShareDialog::~ShareDialog() {
    delete ui;
}

void ShareDialog::on_linkCheckBox_toggled(bool checked) {
    const QSignalBlocker blocker(ui->linkCheckBox);
    Q_UNUSED(blocker);
    const QSignalBlocker blocker2(ui->linkUrlLineEdit);
    Q_UNUSED(blocker2);

    // TODO(pbek): remove this when we actually do something here
    ui->linkCheckBox->setChecked(!checked);

    if (checked) {
        // TODO(pbek): share the file
    } else {
        // TODO(pbek): remove the share
//        ui->linkUrlLineEdit->setVisible(false);
    }
}
