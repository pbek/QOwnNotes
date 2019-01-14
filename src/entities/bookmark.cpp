/**
 * Bookmark class
 */

#include <utility>
#include "bookmark.h"
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QJsonArray>
#include <QJsonDocument>


Bookmark::Bookmark() {
    url = "";
    name = "";
}

Bookmark::Bookmark(QString url, QString name) {
    this->url = std::move(url);
    this->name = std::move(name);
};


QJsonObject Bookmark::jsonObject() {
    QJsonObject bookmarkObject;
    bookmarkObject.insert("name", QJsonValue::fromVariant(name));
    bookmarkObject.insert("url", QJsonValue::fromVariant(url));
    return bookmarkObject;
};

QDebug operator<<(QDebug dbg, const Bookmark &bookmark) {
    dbg.nospace() << "Bookmark: <name>" << bookmark.name <<
                  " <url>" << bookmark.url;
    return dbg.space();
}

/**
 * Parses bookmarks from a text
 *
 * @param text
 * @return
 */
QList<Bookmark> Bookmark::parseBookmarks(const QString &text) {
    QRegularExpressionMatchIterator i;
    QStringList urlList;
    QList<Bookmark> bookmarks;

    i = QRegularExpression(R"(\[(.+)\]\((http[s]?://.+)\))").globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString name = match.captured(1);
        QString url = match.captured(2);

        // check if we already have added the url
        if (!urlList.contains(url)) {
            urlList << url;
            bookmarks << Bookmark(url, name);
        }
    }

    i = QRegularExpression(R"(<(http[s]?://.+)>)").globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString url = match.captured(1);
        bookmarks << Bookmark(url);
    }

    return bookmarks;
}

/**
 * Returns json text of a bookmark list for the WebSocketServerService
 *
 * @return
 */
QString Bookmark::bookmarksWebServiceJsonText(QList<Bookmark> bookmarks) {
    QJsonArray bookmarkObjectList;

    Q_FOREACH(Bookmark bookmark, bookmarks) {
            bookmarkObjectList.push_back(bookmark.jsonObject());
        }

    QJsonObject bookmarkResultObject;
    bookmarkResultObject.insert("type", QJsonValue::fromVariant("bookmarks"));
    bookmarkResultObject.insert("data", bookmarkObjectList);

    QJsonDocument doc(bookmarkResultObject);

    return doc.toJson(QJsonDocument::Compact);
}

/**
 * Returns json text parsed from links of a text for the WebSocketServerService
 *
 * @return
 */
QString Bookmark::parsedBookmarksWebServiceJsonText(QString text) {
    return bookmarksWebServiceJsonText(parseBookmarks(text));
}
