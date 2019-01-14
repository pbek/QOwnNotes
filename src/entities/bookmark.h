/**
 * Bookmark header
 */


#pragma once


#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtCore/QVariant>
#include <QtCore/QDebug>

class Bookmark {
public:
    explicit Bookmark();
    explicit Bookmark(QString url, QString name = "");
    friend QDebug operator<<(QDebug dbg, const Bookmark &bookmark);
    QJsonObject jsonObject();
    static QList<Bookmark> parseBookmarks(const QString &text);
    static QString bookmarksWebServiceJsonText(QList<Bookmark> bookmarks);
    static QString parsedBookmarksWebServiceJsonText(QString text);

protected:
    QString name;
    QString url;
};
