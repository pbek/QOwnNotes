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
    tags = QStringList();
    description = "";
}

Bookmark::Bookmark(QString url, QString name, QStringList tagList,
        QString description) {
    this->url = std::move(url);
    this->name = std::move(name);
    this->tags = std::move(tagList);
    this->description = std::move(description);
};


QJsonObject Bookmark::jsonObject() {
    QJsonObject bookmarkObject;
    bookmarkObject.insert("name", QJsonValue::fromVariant(name));
    bookmarkObject.insert("url", QJsonValue::fromVariant(url));
    bookmarkObject.insert("tags", QJsonValue::fromVariant(tags));
    bookmarkObject.insert("description", QJsonValue::fromVariant(description));
    return bookmarkObject;
};

QDebug operator<<(QDebug dbg, const Bookmark &bookmark) {
    dbg.nospace() << "Bookmark: <name>" << bookmark.name <<
                  " <url>" << bookmark.url << " <tags>" << bookmark.tags <<
                  " <description>" << bookmark.description;
    return dbg.space();
}

/**
 * Parses bookmarks from a text
 *
 * @param text
 * @return
 */
QList<Bookmark> Bookmark::parseBookmarks(QString text, bool withBasicUrls) {
    QRegularExpressionMatchIterator i;
    QStringList urlList;
    QList<Bookmark> bookmarks;

    // parse bookmark links like `- [name](http://link) #tag1 #tag2 the description text`
    // with optional tags and description
    i = QRegularExpression(R"([-*] \[(.+)\]\((http[s]?://.+)\)(.+)$)", QRegularExpression::MultilineOption).globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString name = match.captured(1);
        QString url = match.captured(2);
        QString additionalText = match.captured(3);

        // check if we already have added the url
        if (!urlList.contains(url)) {
            QStringList tags;
            QString description;

            if (!additionalText.isEmpty()) {

                QRegularExpressionMatchIterator addIterator =
                        QRegularExpression(R"(#([^\s#]+))").globalMatch(
                                additionalText);
                while (addIterator.hasNext()) {
                    QRegularExpressionMatch addMatch = addIterator.next();
                    QString tag = addMatch.captured(1).trimmed();

                    if (!tags.contains(tag)) {
                        tags << tag;
                        additionalText.remove("#" + tag);
                    }
                }

                description = additionalText.trimmed();
            }

            urlList << url;
            bookmarks << Bookmark(url, name, tags, description);
        }
    }

    if (withBasicUrls) {
        // parse named links like [name](http://my.site.com)
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

        // parse links like <http://my.site.com>
        i = QRegularExpression(R"(<(http[s]?://.+)>)").globalMatch(text);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString url = match.captured(1);
            bookmarks << Bookmark(url);
        }
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
QString Bookmark::parsedBookmarksWebServiceJsonText(
        QString text, bool withBasicUrls) {
    return bookmarksWebServiceJsonText(parseBookmarks(
            std::move(text), withBasicUrls));
}
