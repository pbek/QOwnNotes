#include "tagadddialog.h"

#include <QPushButton>

#include "ui_tagadddialog.h"

TagAddDialog::TagAddDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::TagAddDialog) {
    ui->setupUi(this);
    afterSetupUI();

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(tr("&Add tag"));
    okButton->setIcon(
        QIcon::fromTheme(QStringLiteral("tag"), QIcon(":/icons/breeze-qownnotes/16x16/tag.svg")));
    okButton->setEnabled(false);

    // A tag can't be added without a name
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this,
            [okButton](const QString &text) { okButton->setEnabled(!text.trimmed().isEmpty()); });

    ui->nameLineEdit->setFocus();
    setMinimumWidth(300);
    resize(1, 1);
}

TagAddDialog::~TagAddDialog() { delete ui; }

QString TagAddDialog::name() { return ui->nameLineEdit->text().trimmed(); }
