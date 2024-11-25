#include "textdiffdialog.h"
#include <QAbstractButton>
#include <QDebug>
#include <diff_match_patch.h>
#include "services/settingsservice.h"
#include "ui_textdiffdialog.h"

TextDiffDialog::TextDiffDialog(QWidget *parent, const QString &title, const QString &labelText,
                               const QString &text1, const QString &text2)
    : MasterDialog(parent), ui(new Ui::TextDiffDialog) {
    ui->setupUi(this);
    afterSetupUI();
    this->setWindowTitle(title);
    this->ui->label_2->setText(labelText);
    this->ui->plainTextEdit->setPlainText(text2);

    auto diff = new diff_match_patch();
    const QList<Diff> diffList = diff->diff_main(text1, text2);
    const QString html = diff->diff_prettyHtml(diffList);
    this->ui->textEdit->setHtml(html);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));
}

TextDiffDialog::~TextDiffDialog() { delete ui; }

void TextDiffDialog::dialogButtonClicked(QAbstractButton *button) {
    auto buttonRole = this->ui->buttonBox->buttonRole(button);
    this->accepted = buttonRole == QDialogButtonBox::AcceptRole;
    this->close();
}

bool TextDiffDialog::resultAccepted() { return this->accepted; }

QString TextDiffDialog::resultText() { return this->ui->plainTextEdit->toPlainText(); }

