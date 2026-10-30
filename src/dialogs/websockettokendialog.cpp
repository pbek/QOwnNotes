#include "websockettokendialog.h"

#include <utils/misc.h>

#include <QFontDatabase>
#include <QPushButton>
#include <QShowEvent>
#include <QtGui/QClipboard>

#include "services/cryptoservice.h"
#include "services/settingsservice.h"
#include "ui_websockettokendialog.h"
#include "utils/gui.h"

WebSocketTokenDialog::WebSocketTokenDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::WebSocketTokenDialog) {
    ui->setupUi(this);
    afterSetupUI();

    // Show the token in a fixed width font, so similar looking characters can be told apart
    ui->tokenLineEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    Utils::Gui::setMutedTextColor(ui->copiedLabel);

    // An empty token can't be used by the browser extension
    connect(ui->tokenLineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        const bool hasToken = !text.trimmed().isEmpty();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(hasToken);
        ui->copyButton->setEnabled(hasToken);

        if (text != _copiedToken) {
            ui->copiedLabel->clear();
        }
    });
}

QString WebSocketTokenDialog::generateToken() const { return Utils::Misc::generateRandomString(8); }

WebSocketTokenDialog::~WebSocketTokenDialog() { delete ui; }

void WebSocketTokenDialog::loadTokenFromSettings() {
    SettingsService settings;
    _initialToken = CryptoService::instance()->decryptToStringWithPlaintextFallback(
        settings.value(QStringLiteral("webSocketServerService/token")).toString());

    if (_initialToken.isEmpty()) {
        ui->tokenLineEdit->setText(generateToken());
        on_copyButton_clicked();
    } else {
        ui->tokenLineEdit->setText(_initialToken);
        on_copyButton_clicked();
    }
}

void WebSocketTokenDialog::on_copyButton_clicked() {
    QClipboard *clipboard = QApplication::clipboard();
    _copiedToken = ui->tokenLineEdit->text();
    clipboard->setText(_copiedToken);
    ui->copiedLabel->setText(tr("The token was copied to the clipboard."));
}

void WebSocketTokenDialog::on_generateButton_clicked() {
    QString token = generateToken();
    ui->tokenLineEdit->setText(token);
    on_copyButton_clicked();
}

void WebSocketTokenDialog::on_buttonBox_accepted() {
    SettingsService settings;
    settings.setValue(
        QStringLiteral("webSocketServerService/token"),
        CryptoService::instance()->encryptToString(
            ui->tokenLineEdit->text(), QStringLiteral("settings/webSocketServerService/token")));
}

void WebSocketTokenDialog::reject() {
    SettingsService settings;
    settings.setValue(QStringLiteral("webSocketServerService/token"),
                      CryptoService::instance()->encryptToString(
                          _initialToken, QStringLiteral("settings/webSocketServerService/token")));
    ui->tokenLineEdit->setText(_initialToken.isEmpty() ? generateToken() : _initialToken);

    MasterDialog::reject();
}

void WebSocketTokenDialog::showEvent(QShowEvent *event) {
    loadTokenFromSettings();
    MasterDialog::showEvent(event);
}
