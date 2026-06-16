#include "websockettokendialog.h"

#include <utils/misc.h>

#include <QShowEvent>
#include <QtGui/QClipboard>

#include "services/settingsservice.h"
#include "ui_websockettokendialog.h"

WebSocketTokenDialog::WebSocketTokenDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::WebSocketTokenDialog) {
    ui->setupUi(this);
    afterSetupUI();
}

QString WebSocketTokenDialog::generateToken() const { return Utils::Misc::generateRandomString(8); }

WebSocketTokenDialog::~WebSocketTokenDialog() { delete ui; }

void WebSocketTokenDialog::loadTokenFromSettings() {
    SettingsService settings;
    _initialToken = settings.value(QStringLiteral("webSocketServerService/token")).toString();

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
    clipboard->setText(ui->tokenLineEdit->text());
}

void WebSocketTokenDialog::on_generateButton_clicked() {
    QString token = generateToken();
    ui->tokenLineEdit->setText(token);
    on_copyButton_clicked();
}

void WebSocketTokenDialog::on_buttonBox_accepted() {
    SettingsService settings;
    settings.setValue(QStringLiteral("webSocketServerService/token"), ui->tokenLineEdit->text());
}

void WebSocketTokenDialog::reject() {
    SettingsService settings;
    settings.setValue(QStringLiteral("webSocketServerService/token"), _initialToken);
    ui->tokenLineEdit->setText(_initialToken.isEmpty() ? generateToken() : _initialToken);

    MasterDialog::reject();
}

void WebSocketTokenDialog::showEvent(QShowEvent *event) {
    loadTokenFromSettings();
    MasterDialog::showEvent(event);
}
