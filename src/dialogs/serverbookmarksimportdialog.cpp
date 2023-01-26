#include "serverbookmarksimportdialog.h"

#include <mainwindow.h>

#include <QDebug>
#include <QJsonArray>
#include <QtQml/QJSValueIterator>

#include "services/websocketserverservice.h"
#include "ui_serverbookmarksimportdialog.h"

ServerBookmarksImportDialog::ServerBookmarksImportDialog(const QJSValue &bookmarks, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::ServerBookmarksImportDialog) {
    ui->setupUi(this);
    ui->infoLabel->clear();

    // init the iterator for the versions
    QJSValueIterator bookmarksIterator(bookmarks);

    QJsonArray bookmarkList;
    QString url;
    QString title;
    QString description;
    QStringList tags;
    int bookmarksCount = 0;
    qDebug() << __func__ << " - 'jsonObject': " << jsonObject;

    // iterate over the bookmarks
    while (bookmarksIterator.hasNext()) {
        bookmarksIterator.next();

        QJSValue property = bookmarksIterator.value().property(QStringLiteral("url"));

        if (property.isUndefined()) {
            continue;
        }

        url = property.toString();

        if (url == QString()) {
            continue;
        }

        title = bookmarksIterator.value().property(QStringLiteral("title")).toString();
        description = bookmarksIterator.value()
                          .property(QStringLiteral("description"))
                          .toString()
                          .remove(QStringLiteral("#"));
        tags =
            bookmarksIterator.value().property(QStringLiteral("tags")).toVariant().toStringList();

        Q_FOREACH (QString tag, tags) {
            description.prepend("#" + tag.replace(QLatin1String(" "), QLatin1String("-")) + " ");
        }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
        QJsonObject data{{"name", title}, {"url", url}, {"description", description.trimmed()}};
#else
        QJsonObject data;
        data.insert("name", title);
        data.insert("url", url);
        data.insert("description", description.trimmed());
#endif

        bookmarkList.append(data);
        bookmarksCount++;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    jsonObject = QJsonObject{{"type", "newBookmarks"}, {"data", bookmarkList}};
#else
    jsonObject = QJsonObject();
    jsonObject.insert("type", "newBookmarks");
    jsonObject.insert("data", bookmarkList);
#endif

    ui->progressBar->setMaximum(bookmarksCount);
    ui->infoLabel->setText(tr("<strong>%n bookmarks</strong> found on server", "", bookmarksCount));
    ui->importButton->setEnabled(bookmarksCount > 0);
    ui->progressBar->setEnabled(bookmarksCount > 0);
}

ServerBookmarksImportDialog::~ServerBookmarksImportDialog() { delete ui; }

void ServerBookmarksImportDialog::on_importButton_clicked() {
    QJsonArray bookmarkList = WebSocketServerService::createBookmarks(jsonObject);
    ui->progressBar->setValue(bookmarkList.count());
}
