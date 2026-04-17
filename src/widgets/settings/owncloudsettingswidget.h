#pragma once

#include <QWidget>

#include "entities/cloudconnection.h"

namespace Ui {
class OwnCloudSettingsWidget;
}

class OwnCloudSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit OwnCloudSettingsWidget(QWidget *parent = nullptr);
    ~OwnCloudSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();
    void replaceOwnCloudText();
    void initCloudConnectionComboBox(int selectedId = -1);
    QString serverUrl() const;

    void handleConnectTestCallback(bool appIsValid, const QString &appVersion,
                                   const QString &serverVersion, const QString &notesPathExistsText,
                                   const QString &connectionErrorMessage);

    void cancelConnectionTest();
    void resetOKLabelData();
    void setOKLabelData(int number, const QString &text, OKLabelStatus status);

    enum OKLabelStatus { Unknown, Warning, OK, Failure };

   signals:
    void cloudConnectionsUpdated(const QList<CloudConnection> &connections, int selectedId);

   private slots:
    void on_connectButton_clicked();
    void on_ownCloudSupportCheckBox_toggled();
    void on_cloudConnectionComboBox_currentIndexChanged(int index);
    void on_cloudConnectionAddButton_clicked();
    void on_cloudConnectionRemoveButton_clicked();
    void on_ownCloudServerAppPageButton_clicked();
    void on_ownCloudServerAppPasswordPageButton_clicked();
    void on_loginFlowButton_clicked();
    void on_loginFlowCancelButton_clicked();
    void on_appNextcloudDeckCheckBox_toggled(bool checked);
    void storeSelectedCloudConnection();
    void onSettingsConnectionTestFinished();

   private:
    Ui::OwnCloudSettingsWidget *ui;
    CloudConnection _selectedCloudConnection;
    bool _connectionTestInProgress = false;
    int _loginFlowPollCount = 0;
    QString _installInfoTextLabel1Html;
    QString _installInfoTextLabel2Html;
    QString _installInfoTextLabel3Html;
    bool appIsValid = false;
    QString appVersion;
    QString serverVersion;
    QString notesPathExistsText;
    QString connectionErrorMessage;

    bool connectionTestCanBeStarted() const;
    void startConnectionTest();
    void setConnectionTestInProgress(bool inProgress);
    void storeOwncloudDebugData() const;
};
