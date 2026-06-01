#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <entities/notefolder.h>
#include <enums/oklabelstatus.h>

#include <QSet>

#include "masterdialog.h"
#include "widgets/settings/cloudsettingswidget.h"
#include "widgets/settings/generalsettingswidget.h"
#include "widgets/settings/interfacesettingswidget.h"
#include "widgets/settings/mcpserversettingswidget.h"
#include "widgets/settings/notefoldersettingswidget.h"
#include "widgets/settings/panelssettingswidget.h"
#include "widgets/settings/scriptingsettingswidget.h"

namespace Ui {
class SettingsDialog;
}

class QListWidgetItem;
class QListWidget;
class QKeySequenceWidget;
class QTreeWidgetItem;
class QTreeWidget;
class QLineEdit;
class QStatusBar;
class QButtonGroup;
class QCheckBox;
class NoteFolder;
class QSplitter;
class QMenu;
class SettingsService;

struct CalDAVCalendarData;

class SettingsDialog : public MasterDialog {
    Q_OBJECT

   public:
    // OKLabelStatus is a global enum defined in enums/oklabelstatus.h
    using OKLabelStatus = ::OKLabelStatus;
    // Expose enum values for backward compatibility (cloudservice.cpp uses SettingsDialog::OK
    // etc.)
    static constexpr OKLabelStatus Unknown = ::Unknown;
    static constexpr OKLabelStatus Warning = ::Warning;
    static constexpr OKLabelStatus OK = ::OK;
    static constexpr OKLabelStatus Failure = ::Failure;

    enum SettingsPages {
        NoteFolderPage,
        CloudPage,
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
        EditorPage,
        GitPage,
        PanelsPage,
        LocalTrashPage,
        LayoutPresetsPage,
        WebCompanionPage,
        WebApplicationPage,
        ExperimentalPage,
        AiPage,
        LanguageToolPage,
        HarperPage,
        ColorModesPage,
        McpServerPage,
        MarkdownLspPage
    };

    explicit SettingsDialog(int page = 0, QWidget *parent = 0);

    ~SettingsDialog();

    void connectTestCallback(bool appIsValid, QString appVersion, QString serverVersion,
                             QString notesPathExistsText, QString connectionErrorMessage);

    void setOKLabelData(int number, const QString &text, OKLabelStatus status);

    void refreshTodoCalendarList(const QList<CalDAVCalendarData> &items,
                                 bool forceReadCheckedState = false);

    void setNoteFolderRemotePathList(QStringList pathList);

    void setCurrentPage(int page);

    void readSettings();

    void accept() override;

    void searchScriptInRepository();

   protected:
    void closeEvent(QCloseEvent *event) override;

   private slots:

    void on_shortcutSearchLineEdit_textChanged(const QString &arg1);

    void on_settingsTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                  QTreeWidgetItem *previous);

    void on_settingsStackedWidget_currentChanged(int index);

    void on_applyToolbarButton_clicked();

    void on_resetToolbarPushButton_clicked();

    void on_searchLineEdit_textChanged(const QString &arg1);

    void needRestart();

    void onLayoutStored(const QString &layoutUuid);

    void keySequenceEvent(const QString &objectName);

   private:
    Ui::SettingsDialog *ui;
    static const int _defaultMarkdownHighlightingInterval = 200;
    QSplitter *_mainSplitter;
    QHash<int, bool> _pageInitialized;
    QHash<QString, QString> _pendingLocalShortcutValues;
    QHash<QString, QString> _pendingGlobalShortcutValues;
    QSet<QString> _pendingLocalShortcutRemovals;
    QSet<QString> _pendingGlobalShortcutRemovals;
    bool _initialDarkMode = false;
    bool _initialDarkModeColors = false;
    bool _initialDarkModeTrayIcon = false;
    bool _initialDarkModeIconTheme = false;
    QString _initialSchemaKey;
    QList<QWidget *> _searchMatchedWidgets;
    QHash<QWidget *, QString> _searchMatchedWidgetOriginalTexts;

    void storeSettings();

    static void selectListWidgetValue(QListWidget *listWidget, const QString &value);

    static bool listWidgetValueExists(QListWidget *listWidget, const QString &value);

    static QString getSelectedListWidgetValue(QListWidget *listWidget);

    void loadShortcutSettings();

    void buildShortcutTreeForMenu(
        const QMenu *menu, QTreeWidgetItem *parentItem, SettingsService &settings,
        const QColor &shortcutButtonActiveColor, const QColor &shortcutButtonInactiveColor,
        const QIcon &disableShortcutButtonIcon, const QIcon &clearButtonIcon,
        const QStringList &disabledMenuNames, QSet<QString> &processedActionObjectNames);

    void storeShortcutSettings();

    QTreeWidgetItem *findSettingsTreeWidgetItemByPage(int page);

    void initMainSplitter();

    void storeSplitterSettings();

    void initPortableModePage();

    int findSettingsPageIndexOfWidget(QWidget *widget);

    void addToSearchIndexList(QWidget *widget, QList<int> &pageIndexList,
                              const QString &searchText = QString());

    void highlightSearchMatchedWidget(QWidget *widget, const QString &searchText);

    void clearSearchHighlights();

    QKeySequenceWidget *findKeySequenceWidget(const QString &objectName);
    QKeySequenceWidget *findGlobalKeySequenceWidget(const QString &objectName);

    void handleDarkModeCheckBoxToggled(bool updateCheckBoxes = false, bool updateSchema = false);

    void updateSearchLineEditIcons();

    bool hasDarkModeSettingChanges() const;

    void applyDarkModeSettings();

    bool initializePage(int index);
};

#endif    // SETTINGSDIALOG_H
