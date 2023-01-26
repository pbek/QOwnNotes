#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSplitter>
#include <QTreeWidgetItem>

#include "masterdialog.h"

namespace Ui {
class DictionaryManagerDialog;
}

class DictionaryManagerDialog : public MasterDialog {
    Q_OBJECT

    struct Dictionary {
        QString name;
        QString pathPart;
        QString fileNamePart;
    };

   public:
    explicit DictionaryManagerDialog(QWidget *parent = nullptr);
    ~DictionaryManagerDialog();

   private slots:
    void slotReplyFinished(QNetworkReply *);

    void on_downloadButton_clicked();

    void on_downloadCancelButton_clicked();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void on_deleteLocalDictionaryButton_clicked();

    void on_remoteDictionaryTreeWidget_itemSelectionChanged();

    void on_localDictionaryTreeWidget_itemSelectionChanged();

    void on_searchDictionaryEdit_textChanged(const QString &arg1);

    void on_remoteDictionaryTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_disableExternalDictionariesCheckBox_toggled(bool checked);

   private:
    Ui::DictionaryManagerDialog *ui;
    QNetworkAccessManager *_networkManager;
    QSplitter *_mainSplitter;
    QList<Dictionary> _dictionaryList;

    void setupMainSplitter();

    void storeSettings();
    void addDictionaryItem(const QString &name, const QString &pathPart,
                           QString fileNamePart = QString());
    void downloadFile(const QString &url);
    void loadLocalDictionaries();
    QString getDictionaryName(const QString &fileNamePart);
    bool deleteLocalDictionaryFile(const QString &fileName);
};
