#include "owncloudsettingswidget.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPointer>
#include <QTimer>

#include "dialogs/settingsdialog.h"
#include "entities/cloudconnection.h"
#include "entities/notefolder.h"
#include "services/owncloudservice.h"
#include "services/settingsservice.h"
#include "ui_owncloudsettingswidget.h"
#include "utils/misc.h"

OwnCloudSettingsWidget::OwnCloudSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::OwnCloudSettingsWidget) {
    ui->setupUi(this);
}

OwnCloudSettingsWidget::~OwnCloudSettingsWidget() { delete ui; }

/**
 * Initializes the ownCloud settings page: stores HTML texts, replaces version placeholder,
 * optionally starts a connection test, and wires up form-field changes to
 * storeSelectedCloudConnection.
 */
void OwnCloudSettingsWidget::initialize() {
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

    // Wire form field changes to store the selected cloud connection
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

/**
 * Reads settings into the ownCloud page form fields.
 * The form fields (serverUrl, userName, password) are populated via
 * initCloudConnectionComboBox → on_cloudConnectionComboBox_currentIndexChanged.
 */
void OwnCloudSettingsWidget::readSettings() {
    ui->ownCloudSupportCheckBox->setChecked(OwnCloudService::isOwnCloudSupportEnabled());
    on_ownCloudSupportCheckBox_toggled();
}

/**
 * Stores the ownCloud settings.
 */
void OwnCloudSettingsWidget::storeSettings() {
    storeSelectedCloudConnection();

    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/supportEnabled"),
                      ui->ownCloudSupportCheckBox->isChecked());
}

/**
 * Replaces the "ownCloud" text by "ownCloud / NextCloud" for the widget's own UI elements.
 */
void OwnCloudSettingsWidget::replaceOwnCloudText() {
    ui->ownCloudSupportGroupBox->setTitle(
        Utils::Misc::replaceOwnCloudText(ui->ownCloudSupportGroupBox->title()));
    ui->ownCloudSupportCheckBox->setText(
        Utils::Misc::replaceOwnCloudText(ui->ownCloudSupportCheckBox->text()));
    ui->userNameEdit->setPlaceholderText(
        Utils::Misc::replaceOwnCloudText(ui->userNameEdit->placeholderText()));
    ui->passwordEdit->setPlaceholderText(
        Utils::Misc::replaceOwnCloudText(ui->passwordEdit->placeholderText()));
    ui->ownCloudGroupBox->setTitle(Utils::Misc::replaceOwnCloudText(ui->ownCloudGroupBox->title()));
    ui->ownCloudServerUrlLabel->setText(
        Utils::Misc::replaceOwnCloudText(ui->ownCloudServerUrlLabel->text(), true));
    ui->check2Label->setText(Utils::Misc::replaceOwnCloudText(ui->check2Label->text()));
    ui->ownCloudServerAppPageButton->setText(
        Utils::Misc::replaceOwnCloudText(ui->ownCloudServerAppPageButton->text(), true));
    ui->ownCloudServerAppPageButton->setToolTip(
        Utils::Misc::replaceOwnCloudText(ui->ownCloudServerAppPageButton->toolTip()));
    ui->ownCloudServerAppPasswordPageButton->setText(
        Utils::Misc::replaceOwnCloudText(ui->ownCloudServerAppPasswordPageButton->text(), true));
    ui->ownCloudServerAppPasswordPageButton->setToolTip(
        Utils::Misc::replaceOwnCloudText(ui->ownCloudServerAppPasswordPageButton->toolTip()));
    ui->connectButton->setText(Utils::Misc::replaceOwnCloudText(ui->connectButton->text(), true));
    ui->connectButton->setToolTip(Utils::Misc::replaceOwnCloudText(ui->connectButton->toolTip()));
    ui->installInfoTextLabel1->setText(
        Utils::Misc::replaceOwnCloudText(ui->installInfoTextLabel1->text()));
    ui->installInfoTextLabel2->setText(
        Utils::Misc::replaceOwnCloudText(ui->installInfoTextLabel2->text()));
    ui->installInfoTextLabel3->setText(
        Utils::Misc::replaceOwnCloudText(ui->installInfoTextLabel3->text()));
    ui->cloudInfoLabel->setText(Utils::Misc::replaceOwnCloudText(ui->cloudInfoLabel->text()));
}

/**
 * Populates the cloudConnectionComboBox and emits cloudConnectionsUpdated
 * for the dialog to forward to noteFolderSettingsWidget and todoSettingsWidget.
 */
void OwnCloudSettingsWidget::initCloudConnectionComboBox(int selectedId) {
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

    // Notify the dialog so it can update noteFolderSettingsWidget and todoSettingsWidget
    Q_EMIT cloudConnectionsUpdated(connections, selectedId);
}

/**
 * Returns the current server URL from the form.
 */
QString OwnCloudSettingsWidget::serverUrl() const { return ui->serverUrlEdit->text(); }

/**
 * Handles the connection test callback from OwnCloudService.
 */
void OwnCloudSettingsWidget::handleConnectTestCallback(bool appIsValidParam,
                                                       const QString &appVersionParam,
                                                       const QString &serverVersionParam,
                                                       const QString &notesPathExistsTextParam,
                                                       const QString &connectionErrorMessageParam) {
    if (!_connectionTestInProgress) {
        return;
    }

    this->appIsValid = appIsValidParam;
    this->appVersion = appVersionParam;
    this->serverVersion = serverVersionParam;
    this->notesPathExistsText = notesPathExistsTextParam;
    this->connectionErrorMessage = connectionErrorMessageParam;

    // Store some data for Utils::Misc::generateDebugInformation
    storeOwncloudDebugData();

    if (appIsValid) {
        ui->connectionTestLabel->setStyleSheet(QStringLiteral("color: green;"));
        ui->connectionTestLabel->setText(tr("The connection was made successfully!\n"
                                            "Server version: %1\nQOwnNotesAPI version: %2")
                                             .arg(serverVersion, appVersion));
    } else {
        // Hide password in error message
        QString errorMsg = connectionErrorMessageParam;
        if (!ui->passwordEdit->text().isEmpty()) {
            errorMsg.replace(ui->passwordEdit->text(), QLatin1String("***"));
        }

        ui->connectionTestLabel->setStyleSheet(QStringLiteral("color: red;"));
        ui->connectionTestLabel->setText(
            Utils::Misc::replaceOwnCloudText(
                tr("There was an error connecting to the ownCloud Server!\n"
                   "You also need to have the QOwnNotesAPI app installed "
                   "and enabled!\n\nConnection error message: ")) +
            errorMsg);
    }

    ui->connectionTestLabel->adjustSize();
    ui->connectionTestLabel->show();
}

/**
 * Cancels the current connection test if one is in progress.
 */
void OwnCloudSettingsWidget::cancelConnectionTest() {
    if (!_connectionTestInProgress) {
        return;
    }

    OwnCloudService *ownCloud = OwnCloudService::currentInstance();
    if (ownCloud != nullptr) {
        ownCloud->abortSettingsConnectionTest();
    }

    setConnectionTestInProgress(false);
    resetOKLabelData();
    ui->connectionTestLabel->hide();
}

/**
 * Resets all OK label data to the unknown state.
 */
void OwnCloudSettingsWidget::resetOKLabelData() {
    for (int i = 0; i <= 8; i++) {
        setOKLabelData(i, tr("unknown"), Unknown);
    }
}

/**
 * Returns true if a connection test can be started.
 */
bool OwnCloudSettingsWidget::connectionTestCanBeStarted() const {
    return ui->ownCloudSupportCheckBox->isChecked() && !ui->serverUrlEdit->text().isEmpty();
}

/**
 * Starts a connection test.
 * OwnCloudService::settingsConnectionTest takes a SettingsDialog*, so we get the dialog
 * via window() and cast it.
 */
void OwnCloudSettingsWidget::startConnectionTest() {
    ui->connectionTestLabel->hide();
    OwnCloudService *ownCloud = OwnCloudService::instance(true, _selectedCloudConnection.getId());
    connect(ownCloud, &OwnCloudService::settingsConnectionTestFinished, this,
            &OwnCloudSettingsWidget::onSettingsConnectionTestFinished, Qt::UniqueConnection);
    ownCloud->settingsConnectionTest(qobject_cast<SettingsDialog *>(window()));
    ui->check8Label->setText(
        tr("notes path <b>%1</b> found on server").arg(NoteFolder::currentRemotePath(false)));
}

void OwnCloudSettingsWidget::setConnectionTestInProgress(bool inProgress) {
    _connectionTestInProgress = inProgress;
    ui->connectButton->setEnabled(!inProgress);
}

/**
 * Sets the text and color of an ok-label by number.
 */
void OwnCloudSettingsWidget::setOKLabelData(int number, const QString &text, OKLabelStatus status) {
    QLabel *label = nullptr;

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
 * Stores some data for Utils::Misc::generateDebugInformation.
 */
void OwnCloudSettingsWidget::storeOwncloudDebugData() const {
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloudInfo/appIsValid"), appIsValid);
    settings.setValue(QStringLiteral("ownCloudInfo/notesPathExistsText"), notesPathExistsText);
    settings.setValue(QStringLiteral("ownCloudInfo/serverVersion"), serverVersion);
    settings.setValue(QStringLiteral("ownCloudInfo/connectionErrorMessage"),
                      connectionErrorMessage);
}

void OwnCloudSettingsWidget::onSettingsConnectionTestFinished() {
    setConnectionTestInProgress(false);
}

void OwnCloudSettingsWidget::on_connectButton_clicked() {
    setConnectionTestInProgress(true);
    ui->connectButton->repaint();
    qApp->processEvents();

    storeSettings();
    resetOKLabelData();

    startConnectionTest();
}

void OwnCloudSettingsWidget::storeSelectedCloudConnection() {
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

void OwnCloudSettingsWidget::on_ownCloudSupportCheckBox_toggled() {
    cancelConnectionTest();

    bool checked = ui->ownCloudSupportCheckBox->isChecked();
    ui->ownCloudGroupBox->setEnabled(checked);

    // Update labels with disabled color when unchecked
    QString disabledColor = palette().color(QPalette::Disabled, QPalette::Text).name();

    if (!_installInfoTextLabel1Html.isEmpty()) {
        if (checked) {
            ui->installInfoTextLabel1->setText(_installInfoTextLabel1Html);
            ui->installInfoTextLabel2->setText(_installInfoTextLabel2Html);
            ui->installInfoTextLabel3->setText(_installInfoTextLabel3Html);
        } else {
            ui->installInfoTextLabel1->setText(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                                                   .arg(disabledColor, _installInfoTextLabel1Html));
            ui->installInfoTextLabel2->setText(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                                                   .arg(disabledColor, _installInfoTextLabel2Html));
            ui->installInfoTextLabel3->setText(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                                                   .arg(disabledColor, _installInfoTextLabel3Html));
        }
    }
}

void OwnCloudSettingsWidget::on_cloudConnectionComboBox_currentIndexChanged(int index) {
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

void OwnCloudSettingsWidget::on_cloudConnectionAddButton_clicked() {
    cancelConnectionTest();

    CloudConnection cloudConnection;
    cloudConnection.setName(QObject::tr("New connection"));
    cloudConnection.setServerUrl(_selectedCloudConnection.getServerUrl());
    cloudConnection.setUsername(_selectedCloudConnection.getUsername());
    cloudConnection.setPassword(_selectedCloudConnection.getPassword());
    cloudConnection.store();

    initCloudConnectionComboBox(cloudConnection.getId());
}

void OwnCloudSettingsWidget::on_cloudConnectionRemoveButton_clicked() {
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

    _selectedCloudConnection.remove();
    initCloudConnectionComboBox();
}

void OwnCloudSettingsWidget::on_ownCloudServerAppPageButton_clicked() {
    QDesktopServices::openUrl(QUrl(ui->serverUrlEdit->text() + "/index.php/settings/apps"));
}

void OwnCloudSettingsWidget::on_ownCloudServerAppPasswordPageButton_clicked() {
    QDesktopServices::openUrl(
        QUrl(ui->serverUrlEdit->text() + "/index.php/settings/user/security"));
}

void OwnCloudSettingsWidget::on_loginFlowButton_clicked() {
    QJsonObject pollData;

    // Initiate the Nextcloud Login flow v2
    if (!OwnCloudService::initiateLoginFlowV2(ui->serverUrlEdit->text(), pollData)) {
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

        // If the cancel button was hidden by pressing it we want to stop the timer
        // After 720 retries (one hour) we also stop
        if (ui->loginFlowCancelButton->isHidden() || _loginFlowPollCount > 720) {
            timer->stop();
            delete timer;
            return;
        }

        QPointer<OwnCloudSettingsWidget> alive(this);

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
        QString accountId = OwnCloudService::fetchNextcloudAccountId(
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

    // Poll for login data every 5 seconds
    timer->start(5000);
}

void OwnCloudSettingsWidget::on_loginFlowCancelButton_clicked() {
    // Hide the cancel button so the login flow timer will be stopped
    ui->loginFlowCancelButton->hide();
    ui->loginFlowButton->show();
}

void OwnCloudSettingsWidget::on_appNextcloudDeckCheckBox_toggled(bool checked) {
    _selectedCloudConnection.setNextcloudDeckEnabled(checked);
}
