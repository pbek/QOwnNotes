#include "textdiffdialog.h"

#include <diff_match_patch.h>

#include <QAbstractButton>
#include <QDebug>
#include <QTimer>

#include "services/settingsservice.h"
#include "ui_textdiffdialog.h"

TextDiffDialog::TextDiffDialog(QWidget *parent, const QString &title, const QString &labelText,
                               const QString &text1, const QString &text2,
                               const QString &identifier)
    : MasterDialog(parent), ui(new Ui::TextDiffDialog) {
    ui->setupUi(this);
    afterSetupUI();
    this->setWindowTitle(title);
    this->text1 = text1;
    this->identifier = identifier;
    this->ui->label_2->setText(labelText);
    this->ui->plainTextEdit->setPlainText(text2);

    // Show or hide the "don't show again" checkbox based on identifier
    if (identifier.isEmpty()) {
        this->ui->dontShowAgainCheckBox->setVisible(false);
    } else {
        this->ui->dontShowAgainCheckBox->setVisible(true);

        // Check if user has previously chosen to not show this dialog
        QString settingsKey = QStringLiteral("MessageBoxOverride/") + identifier;
        QVariant storedResult = SettingsService::instance().value(settingsKey);

        if (storedResult.isValid()) {
            // Use the stored result and close immediately
            this->accepted = storedResult.toBool();
            QTimer::singleShot(0, this, &TextDiffDialog::close);
        }
    }

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));
}

TextDiffDialog::~TextDiffDialog() { delete ui; }

void TextDiffDialog::dialogButtonClicked(QAbstractButton *button) {
    auto buttonRole = this->ui->buttonBox->buttonRole(button);
    this->accepted = buttonRole == QDialogButtonBox::AcceptRole;

    // Save the user's preference if "don't show again" is checked
    if (!this->identifier.isEmpty() && this->ui->dontShowAgainCheckBox->isChecked()) {
        QString settingsKey = QStringLiteral("MessageBoxOverride/") + this->identifier;
        SettingsService::instance().setValue(settingsKey, this->accepted);
    }

    this->close();
}

bool TextDiffDialog::resultAccepted() { return this->accepted; }

QString TextDiffDialog::resultText() { return this->ui->plainTextEdit->toPlainText(); }

void TextDiffDialog::on_plainTextEdit_textChanged() {
    auto diff = new diff_match_patch();
    const QList<Diff> diffList =
        diff->diff_main(this->text1, this->ui->plainTextEdit->toPlainText());
    const QString html = diff->diff_prettyHtml(diffList);
    this->ui->textEdit->setHtml(html);
}
