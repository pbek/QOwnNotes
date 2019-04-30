#include <QtQml/QJSValueIterator>
#include "serverbookmarksimportdialog.h"
#include "ui_serverbookmarksimportdialog.h"

ServerBookmarksImportDialog::ServerBookmarksImportDialog(
        QJSValue bookmarks, MainWindow *mainWindow, QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::ServerBookmarksImportDialog) {
    ui->setupUi(this);

    // init the iterator for the versions
    QJSValueIterator bookmarksIterator(bookmarks);

    QString url;
    QString title;
    QString description;
    QStringList tags;

    // iterate over the bookmarks
    while (bookmarksIterator.hasNext()) {
        bookmarksIterator.next();

        QJSValue property = bookmarksIterator.value().property("url");

        if (property.isUndefined()) {
            continue;
        }

        url = property.toString();

        qDebug() << __func__ << " - 'url': " << url;

        if (url == "") {
            continue;
        }

        title = bookmarksIterator.value().property("title").toString();
        description = bookmarksIterator.value().property("description").toString();
        tags = bookmarksIterator.value().property("tags").toVariant().toStringList();

        qDebug() << __func__ << " - 'tags': " << tags;
    }
}

ServerBookmarksImportDialog::~ServerBookmarksImportDialog() {
    delete ui;
}
