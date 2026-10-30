#include "notediffdialog.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QDebug>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTimer>

#include "services/settingsservice.h"
#include "ui_notediffdialog.h"
#include "utils/gui.h"

NoteDiffDialog::NoteDiffDialog(QWidget *parent, const QString &html)
    : MasterDialog(parent), ui(new Ui::NoteDiffDialog) {
    ui->setupUi(this);
    afterSetupUI();

    _notificationButtonGroup = new QButtonGroup(this);
    _notificationButtonGroup->addButton(ui->ignoreAllExternalChangesCheckBox);
    _notificationButtonGroup->addButton(ui->acceptAllExternalChangesCheckBox);

    // create a hidden checkbox so we can un-check above checkboxes
    _notificationNoneCheckBox = new QCheckBox(this);
    _notificationNoneCheckBox->setHidden(true);
    _notificationButtonGroup->addButton(_notificationNoneCheckBox);
    connect(_notificationButtonGroup, SIGNAL(buttonPressed(QAbstractButton *)), this,
            SLOT(notificationButtonGroupPressed(QAbstractButton *)));

    Utils::Gui::setSectionHeaderFont(ui->label_2);
    Utils::Gui::setMutedTextColor(ui->legendLabel);
    this->ui->textEdit->setHtml(Utils::Gui::readableDiffHtml(html));

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton(tr("&Reload from disk"), ui->buttonBox);
    button->setToolTip(tr("Accept the external changes and load the note text from disk"));
    button->setProperty("ActionRole", Reload);
    button->setDefault(false);
    button->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/view-refresh.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Keep current text"), ui->buttonBox);
    button->setToolTip(
        tr("Ignore the external changes and overwrite the note on disk with the current text"));
    button->setProperty("ActionRole", Overwrite);
    button->setDefault(false);
    button->setIcon(QIcon::fromTheme(QStringLiteral("document-save"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/document-save.svg")));
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

NoteDiffDialog::~NoteDiffDialog() { delete ui; }

/**
 * Check the _notificationNoneCheckBox when the checkboxes should all be
 * unchecked
 *
 * @param button
 */
void NoteDiffDialog::notificationButtonGroupPressed(QAbstractButton *button) {
    if (button->isChecked()) {
        QTimer::singleShot(100, this, SLOT(notificationNoneCheckBoxCheck()));
    }
}

/**
 * Check the _notificationNoneCheckBox
 */
void NoteDiffDialog::notificationNoneCheckBoxCheck() {
    _notificationNoneCheckBox->setChecked(true);
}

void NoteDiffDialog::dialogButtonClicked(QAbstractButton *button) {
    this->actionRole = button->property("ActionRole").toInt();

    // set the setting to ignore all external changes
    if (ui->ignoreAllExternalChangesCheckBox->isChecked()) {
        SettingsService settings;
        settings.setValue(QStringLiteral("ignoreAllExternalModifications"), true);
    }

    // set the setting to accept all external changes
    if (ui->acceptAllExternalChangesCheckBox->isChecked()) {
        SettingsService settings;
        settings.setValue(QStringLiteral("acceptAllExternalModifications"), true);
    }

    this->close();
}

int NoteDiffDialog::resultActionRole() { return this->actionRole; }
