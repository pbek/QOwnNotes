#include <QtQml/QJSValueIterator>
#include <QtCore/QJsonArray>
#include "serverbookmarksimportdialog.h"
#include "ui_serverbookmarksimportdialog.h"

ServerBookmarksImportDialog::ServerBookmarksImportDialog(
        QJSValue bookmarks, MainWindow *mainWindow, QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::ServerBookmarksImportDialog) {
    ui->setupUi(this);

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

        QJSValue property = bookmarksIterator.value().property("url");

        if (property.isUndefined()) {
            continue;
        }

        url = property.toString();

        if (url == "") {
            continue;
        }

        title = bookmarksIterator.value().property("title").toString();
        description = bookmarksIterator.value().property("description")
                .toString().remove("#");
        tags = bookmarksIterator.value().property("tags").toVariant()
                .toStringList();

        Q_FOREACH(QString tag, tags) {
                description.prepend("#" + tag.replace(" ", "-") + " ");
        }

        QJsonObject data {
                {"name", title},
                {"url", url},
                {"description", description.trimmed()}
        };
        
        bookmarkList.append(data);
        bookmarksCount++;
    }

    jsonObject = QJsonObject{
            {"type", "newBookmarks"},
            {"data", bookmarkList}
    };

    ui->progressBar->setMaximum(bookmarksCount);
}

ServerBookmarksImportDialog::~ServerBookmarksImportDialog() {
    delete ui;
}

void ServerBookmarksImportDialog::on_importButton_clicked() {
    QJsonArray bookmarkList = WebSocketServerService::createBookmarks(jsonObject);
    ui->progressBar->setValue(bookmarkList.count());
}
