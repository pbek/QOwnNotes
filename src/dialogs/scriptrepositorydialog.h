#pragma once

#include <entities/script.h>
#include <QDateTime>
#include "masterdialog.h"

namespace Ui {
class ScriptRepositoryDialog;
}

class QSplitter;
class QTreeWidgetItem;

class ScriptRepositoryDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit ScriptRepositoryDialog(QWidget *parent = nullptr, bool checkForUpdates = false);
    ~ScriptRepositoryDialog();

    Script getLastInstalledScript();

    void searchForUpdates();
   signals:
    void updateFound();

   private slots:
    void on_scriptTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                QTreeWidgetItem *previous);

    void on_installButton_clicked();

    void on_searchScriptEdit_textChanged(const QString &arg1);

   private:
    Ui::ScriptRepositoryDialog *ui;
    QString _codeSearchUrl;
    QString _rawContentUrlPrefix;
    QSplitter *_mainSplitter;
    bool _checkForUpdates;
    QString _searchString;
    Script _lastInstalledScript;
    QHash<QString, ScriptInfoJson> _scriptMetaDataCache;
    QHash<QString, QString> _scriptMetaDataJsonTextCache;
    QDateTime _lastScriptMetaDataCacheUpdateTime;

    void searchScript();

    void setupMainSplitter();

    void storeSettings();

    void enableOverview(bool enable);

    ScriptInfoJson getCurrentScriptInfoJson();

    QString getCurrentInfoJsonString();

    void reloadCurrentScriptInfo();

    bool isScriptCacheExpired();
    bool loadScriptRepositoryMetaData();
    void parseScriptRepositoryMetaData(const QByteArray &arr);
    void addScriptTreeWidgetItem(const ScriptInfoJson &scriptInfoJson);
};
