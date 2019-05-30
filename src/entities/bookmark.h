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
    explicit Bookmark(QString url, QString name = "",
            QStringList tagList = QStringList(), QString description = "");
    friend QDebug operator<<(QDebug dbg, const Bookmark &bookmark);
    QJsonObject jsonObject();
    static QList<Bookmark> parseBookmarks(const QString& text, bool withBasicUrls = false);
    static QString bookmarksWebServiceJsonText(QList<Bookmark> bookmarks);
    static QString parsedBookmarksWebServiceJsonText(const QString& text, bool withBasicUrls = false);
    bool operator==(const Bookmark &bookmark) const;
    static void mergeInList(QList<Bookmark> &bookmarks, Bookmark &bookmark);
    static void mergeListInList(QList<Bookmark> &sourceBookmarks,
            QList<Bookmark> &destinationBookmarks);
    void mergeInList(QList<Bookmark> &bookmarks);
    void merge(Bookmark &bookmark);

protected:
    QString name;
    QString url;
    QStringList tags;
    QString description;
};
