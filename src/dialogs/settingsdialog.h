#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QStatusBar>
#include <QSignalMapper>
#include <QSplitter>
#include <entities/notefolder.h>
#include <entities/script.h>
#include <services/owncloudservice.h>
#include "masterdialog.h"

namespace Ui {
    class SettingsDialog;
}

struct CalDAVCalendarData;

class SettingsDialog : public MasterDialog {
Q_OBJECT

public:
    enum OKLabelStatus {
        Unknown,
        Warning,
        OK,
        Failure,
    };

    enum SettingsPages {
        NoteFolderPage,
        OwnCloudPage,
        NetworkPage,
        TodoPage,
        InterfacePage,
        ShortcutPage,
        ScriptingPage,
        GeneralPage,
        DebugPage,
        EditorFontColorPage,
        PortableModePage,
        PreviewFontPage,
        ToolbarPage,
        DebugOptionPage,
        EditorPage
    };

    explicit SettingsDialog(int page = 0,
                            QWidget *parent = 0);

    ~SettingsDialog();

    void connectTestCallback(bool appIsValid, QString appVersion,
                             QString serverVersion,
                             QString notesPathExistsText,
                             QString connectionErrorMessage);

    void setOKLabelData(int number, QString text, OKLabelStatus status);

    void refreshTodoCalendarList(QList<CalDAVCalendarData> items,
                                 bool forceReadCheckedState = false);

    void setNoteFolderRemotePathList(QStringList pathList);

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void on_connectButton_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_ownCloudServerAppPageButton_clicked();

    void on_noteTextEditButton_clicked();

    void on_noteTextViewButton_clicked();

    void on_reloadCalendarListButton_clicked();

    void on_defaultOwnCloudCalendarRadioButton_toggled(bool checked);

    void on_reinitializeDatabaseButton_clicked();

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

    void on_notifyAllExternalModificationsCheckBox_toggled(bool checked);

    void on_ignoreAllExternalModificationsCheckBox_toggled(bool checked);

    void on_addCustomNoteFileExtensionButton_clicked();

    void on_removeCustomNoteFileExtensionButton_clicked();

    void on_defaultNoteFileExtensionListWidget_itemChanged(QListWidgetItem *item);

    void on_defaultNoteFileExtensionListWidget_currentRowChanged(int currentRow);

    void on_darkModeCheckBox_toggled();

    void on_noteFolderShowSubfoldersCheckBox_toggled(bool checked);

    void on_gitHubLineBreaksCheckBox_toggled(bool checked);

    void on_shortcutSearchLineEdit_textChanged(const QString &arg1);

    void on_markdownHighlightingCheckBox_toggled(bool checked);

    void on_settingsTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                  QTreeWidgetItem *previous);

    void on_settingsStackedWidget_currentChanged(int index);

    void on_calDavCalendarRadioButton_toggled(bool checked);

    void on_calendarPlusRadioButton_toggled(bool checked);

    void on_emptyCalendarCachePushButton_clicked();

    void on_itemHeightResetButton_clicked();

    void on_toolbarIconSizeResetButton_clicked();

    void on_ignoreNonTodoCalendarsCheckBox_toggled(bool checked);

    void on_applyToolbarButton_clicked();

    void on_resetToolbarPushButton_clicked();

    void on_imageScaleDownCheckBox_toggled(bool checked);

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_fileLoggingCheckBox_toggled(bool checked);

    void on_clearLogFileButton_clicked();

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
    QString notesPathExistsText;
    QString connectionErrorMessage;
    NoteFolder _selectedNoteFolder;
    Script _selectedScript;
    QSignalMapper *_keyWidgetSignalMapper;
    static const int _defaultMarkdownHighlightingInterval = 200;
    QSplitter *_mainSplitter;

    void readSettings();

    void storeSettings();

    void startConnectionTest();

    void setFontLabel(QLineEdit *label, QFont font);

    void outputSettings();

    QString prepareDebugInformationLine(QString headline, QString data);

    static void selectListWidgetValue(QListWidget *listWidget, QString value);

    static QString getSelectedListWidgetValue(QListWidget *listWidget);

    void setupProxyPage();

    void loadProxySettings();

    void setupNoteFolderPage();

    QTreeWidgetItem *findNoteFolderRemotePathTreeWidgetItem(
            QTreeWidgetItem *parent, QString text);

    void addPathToNoteFolderRemotePathTreeWidget(QTreeWidgetItem *parent,
                                                 QString path);

    QString generatePathFromCurrentNoteFolderRemotePathItem(
            QTreeWidgetItem *item);

    void setNoteFolderRemotePathTreeWidgetFrameVisibility(bool visi);

    void setupScriptingPage();

    void storeScriptListEnabledState();

    void validateCurrentScript();

    QListWidgetItem *addCustomeNoteFileExtension(
            const QString &fileExtension);

    void loadShortcutSettings();

    void storeShortcutSettings();

    QTreeWidgetItem *findSettingsTreeWidgetItemByPage(int page);

    void initMainSplitter();

    void storeSplitterSettings();

    void storeFontSettings();

    void reloadCalendarList();

    void initPortableModePage();

    int findSettingsPageIndexOfWidget(QWidget *widget);

    void addToSearchIndexList(QWidget *widget, QList<int> &pageIndexList);

    void removeLogFile() const;
};

#endif // SETTINGSDIALOG_H
