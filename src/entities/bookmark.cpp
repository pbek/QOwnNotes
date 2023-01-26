/**
 * Bookmark class
 */

#include "bookmark.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <utility>

#include "notefolder.h"

Bookmark::Bookmark() {
    url = QString();
    name = QString();
    tags = QStringList();
    description = QString();
}

Bookmark::Bookmark(QString url, QString name, QStringList tagList, QString description) {
    this->url = std::move(url);
    this->name = std::move(name);
    this->tags = std::move(tagList);
    this->description = std::move(description);
}

QJsonObject Bookmark::jsonObject() const {
    QJsonObject bookmarkObject;
    bookmarkObject.insert(QStringLiteral("name"), QJsonValue::fromVariant(name));
    bookmarkObject.insert(QStringLiteral("url"), QJsonValue::fromVariant(url));
    bookmarkObject.insert(QStringLiteral("tags"), QJsonValue::fromVariant(tags));
    bookmarkObject.insert(QStringLiteral("description"), QJsonValue::fromVariant(description));
    return bookmarkObject;
}

QDebug operator<<(QDebug dbg, const Bookmark &bookmark) {
    dbg.nospace() << "Bookmark: <name>" << bookmark.name << " <url>" << bookmark.url << " <tags>"
                  << bookmark.tags << " <description>" << bookmark.description;
    return dbg.space();
}

bool Bookmark::operator==(const Bookmark &bookmark) const { return url == bookmark.url; }

/**
 * Parses bookmarks from a text
 *
 * @param text
 * @return
 */
QVector<Bookmark> Bookmark::parseBookmarks(const QString &text, bool withBasicUrls) {
    QRegularExpressionMatchIterator i;
    QVector<Bookmark> bookmarks;

    // parse bookmark links like `- [name](http://link) #tag1 #tag2 the
    // description text` with optional tags and description
    i = QRegularExpression(QStringLiteral(R"([-*]\s+\[(.+?)\]\(([\w-]+://.+?)\)(.*)$)"),
                           QRegularExpression::MultilineOption)
            .globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString name = match.captured(1);
        QString url = match.captured(2);
        QString additionalText = match.captured(3);
        QStringList tags;
        QString description;

        if (!additionalText.isEmpty()) {
            QRegularExpressionMatchIterator addIterator =
                QRegularExpression(QStringLiteral(R"(#([^\s#]+))")).globalMatch(additionalText);
            while (addIterator.hasNext()) {
                QRegularExpressionMatch addMatch = addIterator.next();
                QString tag = addMatch.captured(1).trimmed();

                if (!tags.contains(tag)) {
                    tags << tag;
                    additionalText.remove(
                        QRegularExpression("#" + QRegularExpression::escape(tag) + "\\b"));
                }
            }

            description = additionalText.trimmed();
        }

        if (withBasicUrls && !tags.contains(QStringLiteral("current"))) {
            tags << QStringLiteral("current");
        }

        auto bookmark = Bookmark(url, name, tags, description);
        bookmark.mergeInList(bookmarks);
    }

    if (withBasicUrls) {
        // parse named links like [name](http://my.site.com)
        i = QRegularExpression(QStringLiteral(R"(\[(.+?)\]\(([\w-]+://.+?)\))")).globalMatch(text);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString name = match.captured(1);
            QString url = match.captured(2);

            auto bookmark = Bookmark(url, name, QStringList() << QStringLiteral("current"));
            bookmark.mergeInList(bookmarks);
        }

        // parse links like <http://my.site.com>
        i = QRegularExpression(QStringLiteral(R"(<([\w-]+://.+?)>)")).globalMatch(text);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString url = match.captured(1);

            auto bookmark = Bookmark(url, QString(), QStringList{"current"});
            bookmark.mergeInList(bookmarks);
        }
    }

    return bookmarks;
}

/**
 * Returns json text of a bookmark list for the WebSocketServerService
 *
 * @return
 */
QString Bookmark::bookmarksWebServiceJsonText(const QVector<Bookmark> &bookmarks) {
    QJsonArray bookmarkObjectList;
    QJsonArray noteFolderObjectList;

    for (const Bookmark &bookmark : bookmarks) {
        bookmarkObjectList.push_back(bookmark.jsonObject());
    }

    const auto noteFolders = NoteFolder::fetchAll();
    for (const NoteFolder &noteFolder : noteFolders) {
        noteFolderObjectList.push_back(noteFolder.jsonObject());
    }

    QJsonObject bookmarkResultObject;
    bookmarkResultObject.insert(QStringLiteral("type"), QJsonValue::fromVariant("bookmarks"));
    bookmarkResultObject.insert(QStringLiteral("data"), bookmarkObjectList);
    bookmarkResultObject.insert(QStringLiteral("noteFolderName"),
                                NoteFolder::currentNoteFolder().getName());
    bookmarkResultObject.insert(QStringLiteral("noteFolders"), noteFolderObjectList);
    bookmarkResultObject.insert(QStringLiteral("noteFolderId"), NoteFolder::currentNoteFolderId());

    QJsonDocument doc(bookmarkResultObject);

    return doc.toJson(QJsonDocument::Compact);
}

/**
 * Returns json text parsed from links of a text for the WebSocketServerService
 *
 * @return
 */
QString Bookmark::parsedBookmarksWebServiceJsonText(const QString &text, bool withBasicUrls) {
    return bookmarksWebServiceJsonText(parseBookmarks(text, withBasicUrls));
}

/**
 * Merges the current bookmark into a list of bookmarks
 */
void Bookmark::mergeInList(QVector<Bookmark> &bookmarks) {
    int i = bookmarks.indexOf(*this);

    if (i == -1) {
        bookmarks.append(*this);
    } else {
        auto existingBookmark = bookmarks.at(i);

        // merge bookmarks
        existingBookmark.merge(*this);
        bookmarks[i] = existingBookmark;
    }
}

/**
 * Merges a bookmark into a list of bookmarks
 */
void Bookmark::mergeInList(QVector<Bookmark> &bookmarks, Bookmark &bookmark) {
    bookmark.mergeInList(bookmarks);
}

/**
 * Merges the current bookmark with another
 */
void Bookmark::merge(Bookmark &bookmark) {
    tags.append(bookmark.tags);
    tags.removeDuplicates();
    tags.sort();

    if (name.isEmpty()) {
        name = bookmark.name;
    }

    if (!description.contains(bookmark.description)) {
        if (!description.isEmpty()) {
            description += QLatin1String(", ");
        }

        description += bookmark.description;
    }
}

/**
 * Merges a list of bookmarks into another
 */
void Bookmark::mergeListInList(const QVector<Bookmark> &sourceBookmarks,
                               QVector<Bookmark> &destinationBookmarks) {
    for (Bookmark bookmark : sourceBookmarks) {
        bookmark.mergeInList(destinationBookmarks);
    }
}
