#include "websockettokendialog.h"

#include <utils/misc.h>

#include <QtGui/QClipboard>

#include "services/settingsservice.h"
#include "ui_websockettokendialog.h"

WebSocketTokenDialog::WebSocketTokenDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::WebSocketTokenDialog) {
    ui->setupUi(this);
    afterSetupUI();

    SettingsService settings;
    QString token = settings.value(QStringLiteral("webSocketServerService/token")).toString();

    if (token.isEmpty()) {
        on_generateButton_clicked();
    } else {
        ui->tokenLineEdit->setText(token);
        on_copyButton_clicked();
    }
}

QString WebSocketTokenDialog::generateToken() const { return Utils::Misc::generateRandomString(8); }

WebSocketTokenDialog::~WebSocketTokenDialog() { delete ui; }

void WebSocketTokenDialog::on_copyButton_clicked() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->tokenLineEdit->text());
}

void WebSocketTokenDialog::on_generateButton_clicked() {
    QString token = generateToken();
    SettingsService settings;
    settings.setValue(QStringLiteral("webSocketServerService/token"), token);
    ui->tokenLineEdit->setText(token);
    on_copyButton_clicked();
}

void WebSocketTokenDialog::on_buttonBox_accepted() {
    SettingsService settings;
    settings.setValue(QStringLiteral("webSocketServerService/token"), ui->tokenLineEdit->text());
}
