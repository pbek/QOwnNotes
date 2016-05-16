#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QStatusBar>
#include <entities/notefolder.h>
#include <entities/script.h>
#include "masterdialog.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public MasterDialog {
Q_OBJECT

public:
    enum OKLabelStatus {
        Unknown,
        Warning,
        OK,
        Failure,
    };

    enum SettingsTabs {
        NoteFolderTab,
        OwnCloudTab,
        NetworkTab,
        TodoTab,
        InterfaceTab,
        ScriptingTab,
        GeneralTab,
        DebugTab
    };

    explicit SettingsDialog(int tab = 0, QWidget *parent = 0);

    ~SettingsDialog();

    void connectTestCallback(bool appIsValid,
                             QString appVersion,
                             QString serverVersion,
                             QString connectionErrorMessage);

    void setOKLabelData(int number, QString text, OKLabelStatus status);

    void refreshTodoCalendarList(QStringList items,
                                 bool forceReadCheckedState = false);

    void setNoteFolderRemotePathList(QStringList pathList);

private slots:

    void on_connectButton_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_ownCloudServerAppPageButton_clicked();

    void on_noteTextEditButton_clicked();

    void on_noteTextViewButton_clicked();

    void on_reloadCalendarListButton_clicked();

    void on_defaultOwnCloudCalendarRadioButton_toggled(bool checked);

    void on_reinitializeDatabaseButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_saveDebugInfoButton_clicked();

    void on_appMetricsCheckBox_toggled(bool checked);

    void on_clearAppDataAndExitButton_clicked();

    void on_noteTextEditCodeButton_clicked();

    void on_noteTextEditResetButton_clicked();

    void on_noteTextEditCodeResetButton_clicked();

    void on_noteTextViewResetButton_clicked();

    void on_noteTextViewCodeButton_clicked();

    void on_noteTextViewCodeResetButton_clicked();

    void on_setExternalEditorPathToolButton_clicked();

    void storeProxySettings();

    void on_ignoreSSLErrorsCheckBox_toggled(bool checked);

    void on_noteFolderListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_noteFolderAddButton_clicked();

    void on_noteFolderRemoveButton_clicked();

    void on_noteFolderNameLineEdit_editingFinished();

    void on_noteFolderRemotePathLineEdit_editingFinished();

    void on_noteFolderLocalPathButton_clicked();

    void on_noteFolderActiveCheckBox_stateChanged(int arg1);

    void on_noteFolderRemotePathButton_clicked();

    void on_noteFolderRemotePathTreeWidget_currentItemChanged(
            QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_useOwnCloudPathButton_clicked();

    void on_scriptAddButton_clicked();

    void on_scriptRemoveButton_clicked();

    void on_scriptPathButton_clicked();

    void on_scriptListWidget_currentItemChanged(
            QListWidgetItem *current, QListWidgetItem *previous);

    void on_scriptNameLineEdit_editingFinished();

    void on_scriptValidationButton_clicked();

    void on_scriptReloadEngineButton_clicked();

private:

    Ui::SettingsDialog *ui;
    QStatusBar *noteFolderRemotePathTreeStatusBar;
    QFont noteTextEditFont;
    QFont noteTextEditCodeFont;
    QFont noteTextViewFont;
    QFont noteTextViewCodeFont;
    bool appIsValid;
    QString appVersion;
    QString serverVersion;
    QString connectionErrorMessage;
    NoteFolder _selectedNoteFolder;
    Script _selectedScript;

    void readSettings();

    void storeSettings();

    void startConnectionTest();

    void setFontLabel(QLineEdit *label, QFont font);

    void outputSettings();

    QString prepareDebugInformationLine(QString headline, QString data);

    static void selectListWidgetValue(QListWidget *listWidget, QString value);

    static QString getSelectedListWidgetValue(QListWidget *listWidget);

    void setupProxyTab();

    void loadProxySettings();

    void setupNoteFolderTab();

    QTreeWidgetItem *findNoteFolderRemotePathTreeWidgetItem(
            QTreeWidgetItem *parent, QString text);

    void addPathToNoteFolderRemotePathTreeWidget(QTreeWidgetItem *parent,
                                                 QString path);

    QString generatePathFromCurrentNoteFolderRemotePathItem(
            QTreeWidgetItem *item);

    void setNoteFolderRemotePathTreeWidgetFrameVisibility(bool visi);

    void setupScriptingTab();

    void storeScriptListEnabledState();

    void validateCurrentScript();
};

#endif // SETTINGSDIALOG_H
