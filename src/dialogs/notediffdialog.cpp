#include "notediffdialog.h"
#include "ui_notediffdialog.h"
#include <QPushButton>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QDebug>
#include <QSettings>

NoteDiffDialog::NoteDiffDialog(QWidget *parent, QString html) :
        MasterDialog(parent),
        ui(new Ui::NoteDiffDialog) {
    ui->setupUi(this);

    this->ui->textEdit->setHtml(html);

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("Yes"));
    button->setProperty("ActionRole", Reload);
    button->setDefault(false);
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("No"));
    button->setProperty("ActionRole", Overwrite);
    button->setDefault(false);
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

//    button = new QPushButton(tr("&Ignore changes"));
//    button->setProperty("ActionRole", Ignore);
//    button->setDefault(true);
//    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);
//
//    button = new QPushButton(tr("&Cancel"));
//    button->setProperty("ActionRole", Cancel);
//    button->setDefault(false);
//    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));
}

NoteDiffDialog::~NoteDiffDialog() {
    delete ui;
}

void NoteDiffDialog::dialogButtonClicked(QAbstractButton *button) {
    this->actionRole = button->property("ActionRole").toInt();

    // set the setting to ignore all external changes
    if (ui->ignoreAllExternalChangesCheckBox->isChecked()) {
        QSettings settings;
        settings.setValue("ignoreAllExternalModifications", true);
    }

    // set the setting to accept all external changes
    if (ui->acceptAllExternalChangesCheckBox->isChecked()) {
        QSettings settings;
        settings.setValue("acceptAllExternalModifications", true);
    }

    this->close();
}

int NoteDiffDialog::resultActionRole() {
    return this->actionRole;
}

void NoteDiffDialog::on_ignoreAllExternalChangesCheckBox_toggled(bool checked) {
    // un-check and disable the acceptAllExternalChangesCheckBox
    const QSignalBlocker blocker(ui->acceptAllExternalChangesCheckBox);
    ui->acceptAllExternalChangesCheckBox->setEnabled(!checked);

    if (checked) {
        ui->acceptAllExternalChangesCheckBox->setChecked(false);
    }
}

void NoteDiffDialog::on_acceptAllExternalChangesCheckBox_toggled(bool checked) {
    // un-check and disable the ignoreAllExternalChangesCheckBox
    const QSignalBlocker blocker(ui->ignoreAllExternalChangesCheckBox);
    ui->ignoreAllExternalChangesCheckBox->setEnabled(!checked);

    if (checked) {
        ui->ignoreAllExternalChangesCheckBox->setChecked(false);
    }
}
