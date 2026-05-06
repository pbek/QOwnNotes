/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "cloudsettingswidget.h"

#include <entities/notefolder.h>
#include <services/cloudservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPointer>
#include <QTimer>
#include <QUrl>

#include "dialogs/settingsdialog.h"
#include "release.h"
#include "services/settingsservice.h"
#include "ui_cloudsettingswidget.h"

CloudSettingsWidget::CloudSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::CloudSettingsWidget) {
    ui->setupUi(this);

    ui->loginFlowCancelButton->hide();
    ui->connectionTestLabel->hide();
    ui->connectButton->setDefault(true);

    // Cancel connection test when credentials change
    connect(ui->cloudServerConnectionNameLineEdit, &QLineEdit::textChanged, this,
            [this] { cancelConnectionTest(); });
    connect(ui->serverUrlEdit, &QLineEdit::textChanged, this, [this] { cancelConnectionTest(); });
    connect(ui->userNameEdit, &QLineEdit::textChanged, this, [this] { cancelConnectionTest(); });
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, [this] { cancelConnectionTest(); });
    connect(ui->appQOwnNotesAPICheckBox, &QCheckBox::toggled, this,
            [this] { cancelConnectionTest(); });
    connect(ui->appNextcloudDeckCheckBox, &QCheckBox::toggled, this,
            [this] { cancelConnectionTest(); });
}

CloudSettingsWidget::~CloudSettingsWidget() { delete ui; }

/**
 * Lazy initialization called when the cloud page is first shown
 */
void CloudSettingsWidget::initialize() {
    resetOKLabelData();

    // Store original HTML texts for enable/disable state management
    _installInfoTextLabel1Html = ui->installInfoTextLabel1->text();
    _installInfoTextLabel2Html = ui->installInfoTextLabel2->text();
    _installInfoTextLabel3Html = ui->installInfoTextLabel3->text();

    // Add the QOwnNotesAPI minimum version number to the info text
    _installInfoTextLabel1Html.replace(QLatin1String("QOWNNOTESAPI_MIN_VERSION"),
                                       QOWNNOTESAPI_MIN_VERSION);
    ui->installInfoTextLabel1->setText(_installInfoTextLabel1Html);

    if (connectionTestCanBeStarted()) {
        startConnectionTest();
    }

    // Handle cloud connection storing on field changes
    connect(ui->cloudServerConnectionNameLineEdit, SIGNAL(textChanged(QString)), this,
            SLOT(storeSelectedCloudConnection()));
    connect(ui->serverUrlEdit, SIGNAL(textChanged(QString)), this,
            SLOT(storeSelectedCloudConnection()));
    connect(ui->userNameEdit, SIGNAL(textChanged(QString)), this,
            SLOT(storeSelectedCloudConnection()));
    connect(ui->passwordEdit, SIGNAL(textChanged(QString)), this,
            SLOT(storeSelectedCloudConnection()));
    connect(ui->appQOwnNotesAPICheckBox, SIGNAL(toggled(bool)), this,
            SLOT(storeSelectedCloudConnection()));
}

void CloudSettingsWidget::readSettings() {
    ui->cloudSupportCheckBox->setChecked(CloudService::isCloudSupportEnabled());
    on_cloudSupportCheckBox_toggled();

    // Populate the cloud connection combo box
    initCloudConnectionComboBox();
}

void CloudSettingsWidget::storeSettings() {
    storeSelectedCloudConnection();

    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/supportEnabled"),
                      ui->cloudSupportCheckBox->isChecked());
}

/**
 * Returns the server URL from the server URL edit field
 */
QString CloudSettingsWidget::serverUrl() const { return ui->serverUrlEdit->text(); }

/**
 * Returns true if we can start a connection test
 */
bool CloudSettingsWidget::connectionTestCanBeStarted() const {
    return ui->cloudSupportCheckBox->isChecked() && !ui->serverUrlEdit->text().isEmpty();
}

/**
 * Resets all OK label data to unknown state
 */
void CloudSettingsWidget::resetOKLabelData() {
    for (int i = 0; i <= 8; i++) {
        setOKLabelData(i, tr("unknown"), Unknown);
    }
}

/**
 * @brief Starts a connection test
 */
void CloudSettingsWidget::startConnectionTest() {
    ui->connectionTestLabel->hide();
    CloudService *cloud = CloudService::instance(true, _selectedCloudConnection.getId());
    connect(cloud, &CloudService::settingsConnectionTestFinished, this,
            &CloudSettingsWidget::onSettingsConnectionTestFinished, Qt::UniqueConnection);
    cloud->settingsConnectionTest(_settingsDialog);
    ui->check8Label->setText(
        tr("notes path <b>%1</b> found on server").arg(NoteFolder::currentRemotePath(false)));
}

void CloudSettingsWidget::setConnectionTestInProgress(bool inProgress) {
    _connectionTestInProgress = inProgress;
    ui->connectButton->setEnabled(!inProgress);
}

void CloudSettingsWidget::cancelConnectionTest() {
    if (!_connectionTestInProgress) {
        return;
    }

    CloudService *cloud = CloudService::currentInstance();
    if (cloud != nullptr) {
        cloud->abortSettingsConnectionTest();
    }

    setConnectionTestInProgress(false);
    resetOKLabelData();
    ui->connectionTestLabel->hide();
}

/**
 * @brief Handles the connect button click to start a connection test
 */
void CloudSettingsWidget::on_connectButton_clicked() {
    setConnectionTestInProgress(true);
    ui->connectButton->repaint();
    qApp->processEvents();

    storeSettings();
    resetOKLabelData();

    // Start a connection test
    startConnectionTest();
}

void CloudSettingsWidget::storeSelectedCloudConnection() {
    QString url = ui->serverUrlEdit->text();
    bool updateComboBox = false;

    // Remove trailing "/" of the server url
    if (url.endsWith(QLatin1String("/"))) {
        url.chop(1);
    }

    // Store previously selected cloud connection
    if (_selectedCloudConnection.isFetched()) {
        if (_selectedCloudConnection.getName() != ui->cloudServerConnectionNameLineEdit->text()) {
            updateComboBox = true;
        }
    }

    _selectedCloudConnection.setName(ui->cloudServerConnectionNameLineEdit->text());
    _selectedCloudConnection.setServerUrl(url);
    _selectedCloudConnection.setUsername(ui->userNameEdit->text());
    _selectedCloudConnection.setPassword(ui->passwordEdit->text());
    _selectedCloudConnection.setAppQOwnNotesAPIEnabled(ui->appQOwnNotesAPICheckBox->isChecked());
    _selectedCloudConnection.store();

    if (updateComboBox) {
        initCloudConnectionComboBox(_selectedCloudConnection.getId());
    }
}

void CloudSettingsWidget::on_cloudSupportCheckBox_toggled() {
    cancelConnectionTest();

    bool checked = ui->cloudSupportCheckBox->isChecked();
    ui->cloudGroupBox->setEnabled(checked);

    // Update labels with disabled color when unchecked
    QString disabledColor = palette().color(QPalette::Disabled, QPalette::Text).name();

    if (!_installInfoTextLabel1Html.isEmpty()) {
        if (checked) {
            ui->installInfoTextLabel1->setText(_installInfoTextLabel1Html);
            ui->installInfoTextLabel2->setText(_installInfoTextLabel2Html);
            ui->installInfoTextLabel3->setText(_installInfoTextLabel3Html);
        } else {
            // Wrap content in a span with disabled color
            ui->installInfoTextLabel1->setText(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                                                   .arg(disabledColor, _installInfoTextLabel1Html));
            ui->installInfoTextLabel2->setText(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                                                   .arg(disabledColor, _installInfoTextLabel2Html));
            ui->installInfoTextLabel3->setText(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                                                   .arg(disabledColor, _installInfoTextLabel3Html));
        }
    }
}

void CloudSettingsWidget::initCloudConnectionComboBox(int selectedId) {
    const QSignalBlocker blocker(ui->cloudConnectionComboBox);
    Q_UNUSED(blocker)
    ui->cloudConnectionComboBox->clear();
    int index = 0;
    int currentIndex = 0;
    if (selectedId == -1) {
        selectedId = NoteFolder::currentNoteFolder().getCloudConnectionId();
    }

    const auto connections = CloudConnection::fetchAll();

    Q_FOREACH (CloudConnection cloudConnection, connections) {
        ui->cloudConnectionComboBox->addItem(cloudConnection.getName(), cloudConnection.getId());

        if (cloudConnection.getId() == selectedId) {
            currentIndex = index;
        }

        index++;
    }

    ui->cloudConnectionComboBox->setCurrentIndex(currentIndex);
    on_cloudConnectionComboBox_currentIndexChanged(currentIndex);

    // Notify other widgets that need to populate their cloud connection combo boxes
    Q_EMIT cloudConnectionsChanged(connections);
}

void CloudSettingsWidget::on_cloudConnectionComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index)
    cancelConnectionTest();

    const int id = ui->cloudConnectionComboBox->currentData().toInt();
    _selectedCloudConnection = CloudConnection::fetch(id);

    const QSignalBlocker blocker(ui->cloudServerConnectionNameLineEdit);
    Q_UNUSED(blocker)
    const QSignalBlocker blocker2(ui->serverUrlEdit);
    Q_UNUSED(blocker2)
    const QSignalBlocker blocker3(ui->userNameEdit);
    Q_UNUSED(blocker3)
    const QSignalBlocker blocker4(ui->passwordEdit);
    Q_UNUSED(blocker4)
    const QSignalBlocker blocker5(ui->appQOwnNotesAPICheckBox);
    Q_UNUSED(blocker5)
    const QSignalBlocker blocker6(ui->appNextcloudDeckCheckBox);
    Q_UNUSED(blocker6)

    ui->cloudServerConnectionNameLineEdit->setText(_selectedCloudConnection.getName());
    ui->serverUrlEdit->setText(_selectedCloudConnection.getServerUrl());
    ui->userNameEdit->setText(_selectedCloudConnection.getUsername());
    ui->passwordEdit->setText(_selectedCloudConnection.getPassword());
    ui->appQOwnNotesAPICheckBox->setChecked(_selectedCloudConnection.getAppQOwnNotesAPIEnabled());
    ui->appNextcloudDeckCheckBox->setChecked(_selectedCloudConnection.getNextcloudDeckEnabled());
    ui->cloudConnectionRemoveButton->setDisabled(
        CloudConnection::fetchUsedCloudConnectionsIds().contains(id));
}

void CloudSettingsWidget::on_cloudConnectionAddButton_clicked() {
    cancelConnectionTest();

    // Create a new cloud connection
    CloudConnection cloudConnection;
    cloudConnection.setName(QObject::tr("New connection"));
    cloudConnection.store();

    initCloudConnectionComboBox(cloudConnection.getId());
}

void CloudSettingsWidget::on_cloudConnectionRemoveButton_clicked() {
    cancelConnectionTest();

    if (CloudConnection::countAll() <= 1) {
        return;
    }

    // Check if cloud connection is in use
    if (CloudConnection::fetchUsedCloudConnectionsIds().contains(
            _selectedCloudConnection.getId())) {
        ui->cloudConnectionRemoveButton->setDisabled(true);
        return;
    }

    const QString connectionName = _selectedCloudConnection.getName();
    const auto result = Utils::Gui::question(
        this, tr("Remove cloud connection"),
        tr("Do you really want to remove the cloud connection <strong>%1</strong>?")
            .arg(connectionName.toHtmlEscaped()),
        QStringLiteral("remove-cloud-connection"), QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (result != QMessageBox::Yes) {
        return;
    }

    _selectedCloudConnection.remove();
    initCloudConnectionComboBox();
}

void CloudSettingsWidget::on_cloudServerAppPageButton_clicked() {
    QDesktopServices::openUrl(QUrl(ui->serverUrlEdit->text() + "/index.php/settings/apps"));
}

void CloudSettingsWidget::on_cloudServerAppPasswordPageButton_clicked() {
    QDesktopServices::openUrl(
        QUrl(ui->serverUrlEdit->text() + "/index.php/settings/user/security"));
}

void CloudSettingsWidget::on_loginFlowButton_clicked() {
    QJsonObject pollData;

    // Initiate the Nextcloud Login flow v2
    if (!CloudService::initiateLoginFlowV2(ui->serverUrlEdit->text(), pollData)) {
        return;
    }

    ui->loginFlowButton->hide();
    ui->loginFlowCancelButton->show();

    auto pollUrl = pollData.value(QStringLiteral("endpoint")).toString();
    auto token = pollData.value(QStringLiteral("token")).toString();

    auto timer = new QTimer(this);
    _loginFlowPollCount = 0;

    connect(timer, &QTimer::timeout, this, [this, pollUrl, token, timer] {
        _loginFlowPollCount++;

        // If the cancel button was hidden by pressing it we want to stop the timer.
        // After 720 retries (one hour) we also stop.
        if (ui->loginFlowCancelButton->isHidden() || _loginFlowPollCount > 720) {
            timer->stop();
            delete timer;

            return;
        }

        QPointer<CloudSettingsWidget> alive(this);

        auto postData = QStringLiteral("token=%1").arg(token).toLocal8Bit();
        auto data = Utils::Misc::downloadUrl(pollUrl, true, postData);

        if (!alive) {
            return;
        }

        // Wait until there is a JSON result
        if (!data.startsWith('{')) {
            return;
        }

        timer->stop();

        // Parse login data
        auto jsonObject = QJsonDocument::fromJson(data).object();
        ui->serverUrlEdit->setText(jsonObject.value(QStringLiteral("server")).toString());
        ui->userNameEdit->setText(jsonObject.value(QStringLiteral("loginName")).toString());
        ui->passwordEdit->setText(jsonObject.value(QStringLiteral("appPassword")).toString());

        // Try to fetch the account id
        QString accountId = CloudService::fetchNextcloudAccountId(
            ui->serverUrlEdit->text(), ui->userNameEdit->text(), ui->passwordEdit->text());

        if (!alive) {
            return;
        }

        _selectedCloudConnection.setAccountId(accountId);
        _selectedCloudConnection.store();

        QMessageBox::information(this, QObject::tr("Login flow succeeded"),
                                 QObject::tr("Username and password were set successfully!"));

        ui->loginFlowButton->show();
        ui->loginFlowCancelButton->hide();
        delete timer;
    });

    // We want to poll for the login data every 5 seconds
    timer->start(5000);
}

void CloudSettingsWidget::on_loginFlowCancelButton_clicked() {
    // Hide the login flow cancel button so the login flow timer will be stopped
    ui->loginFlowCancelButton->hide();
    ui->loginFlowButton->show();
}

void CloudSettingsWidget::on_appNextcloudDeckCheckBox_toggled(bool checked) {
    _selectedCloudConnection.setNextcloudDeckEnabled(checked);
}

void CloudSettingsWidget::onSettingsConnectionTestFinished() { setConnectionTestInProgress(false); }

/**
 * Called by CloudService when a connection test result arrives
 */
void CloudSettingsWidget::connectTestCallback(bool appIsValid, QString appVersion,
                                              QString serverVersion, QString notesPathExistsText,
                                              QString connectionErrorMessage) {
    if (!_connectionTestInProgress) {
        return;
    }

    _appIsValid = appIsValid;
    _appVersion = appVersion;
    _serverVersion = serverVersion;
    _notesPathExistsText = std::move(notesPathExistsText);
    _connectionErrorMessage = connectionErrorMessage;

    // Store some data for Utils::Misc::generateDebugInformation
    storeCloudDebugData();

    if (appIsValid) {
        ui->connectionTestLabel->setStyleSheet(QStringLiteral("color: green;"));
        ui->connectionTestLabel->setText(tr("The connection was made successfully!\n"
                                            "Server version: %1\nQOwnNotesAPI version: %2")
                                             .arg(serverVersion, appVersion));
    } else {
        // Hide password from error message
        if (!ui->passwordEdit->text().isEmpty()) {
            connectionErrorMessage.replace(ui->passwordEdit->text(), QLatin1String("***"));
        }

        ui->connectionTestLabel->setStyleSheet(QStringLiteral("color: red;"));
        ui->connectionTestLabel->setText(
            tr("There was an error connecting to the Nextcloud / ownCloud Server!\n"
               "You also need to have the QOwnNotesAPI app installed "
               "and enabled!\n\nConnection error message: ") +
            connectionErrorMessage);
    }

    ui->connectionTestLabel->adjustSize();
    ui->connectionTestLabel->show();
}

/**
 * @brief Sets text and color of an OK-label
 * @param number
 * @param text
 * @param status
 */
void CloudSettingsWidget::setOKLabelData(int number, const QString &text, OKLabelStatus status) {
    QLabel *label;

    switch (number) {
        case 1:
            label = ui->ok1Label;
            break;
        case 2:
            label = ui->ok2Label;
            break;
        case 3:
            label = ui->ok3Label;
            break;
        case 4:
            label = ui->ok4Label;
            break;
        case 6:
            label = ui->ok6Label;
            break;
        case 7:
            label = ui->ok7Label;
            break;
        case 8:
            label = ui->ok8Label;
            break;
        default:
            return;
    }

    QString color;
    switch (status) {
        case Unknown:
            color = QLatin1String("gray");
            break;
        case OK:
            color = QLatin1String("green");
            break;
        case Warning:
            color = QLatin1String("orange");
            break;
        case Failure:
            color = QLatin1String("red");
            break;
        default:
            color = QLatin1String("white");
    }

    label->setText(text);
    label->setStyleSheet("color: " + color);
}

/**
 * Stores ownCloud connection test results for the debug information generator
 */
void CloudSettingsWidget::storeCloudDebugData() const {
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloudInfo/appIsValid"), _appIsValid);
    settings.setValue(QStringLiteral("ownCloudInfo/notesPathExistsText"), _notesPathExistsText);
    settings.setValue(QStringLiteral("ownCloudInfo/serverVersion"), _serverVersion);
    settings.setValue(QStringLiteral("ownCloudInfo/connectionErrorMessage"),
                      _connectionErrorMessage);
}

/**
 * Sets the back-pointer to the SettingsDialog, required by CloudService::settingsConnectionTest
 */
void CloudSettingsWidget::setSettingsDialog(SettingsDialog *dialog) { _settingsDialog = dialog; }
