#pragma once

#include "masterdialog.h"
#include <entities/script.h>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QSplitter>
#include <QTreeWidgetItem>

namespace Ui {
class ScriptRepositoryDialog;
}

class ScriptRepositoryDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit ScriptRepositoryDialog(QWidget *parent = 0,
                                    bool checkForUpdates = false);
    ~ScriptRepositoryDialog();

    Script getLastInstalledScript();

private slots:
    void on_searchScriptEdit_returnPressed();

    void slotReplyFinished(QNetworkReply *);

    void on_scriptTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                QTreeWidgetItem *previous);

    void on_installButton_clicked();

    void scriptTreeWidgetSliderValueChanged(int value);

    void on_searchScriptEdit_textChanged(const QString &arg1);

    void on_loadMoreScriptsButton_clicked();

private:
    Ui::ScriptRepositoryDialog *ui;
    QNetworkAccessManager *_networkManager;
    QString _codeSearchUrl;
    QString _rawContentUrlPrefix;
    QSplitter *_mainSplitter;
    bool _checkForUpdates;
    int _page = 1;
    int _totalCount = 0;
    static const int _itemsPerPage = 30;
    QString _searchString;
    Script _lastInstalledScript;

    void searchScript(int page = 1);

    void setupMainSplitter();

    void parseCodeSearchReply(const QByteArray &arr);

    void storeSettings();

    void parseInfoQMLReply(const QByteArray &arr) const;

    void enableOverview(bool enable);

    QJsonObject getCurrentInfoJsonObject();

    QString getCurrentInfoJsonString();

    void reloadCurrentScriptInfo();

    void searchForUpdates();

    bool hasMoreItems() const;

    void loadMoreItems();
};
