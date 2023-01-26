/**
 * Bookmark header
 */

#pragma once

#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

class Bookmark {
   public:
    Bookmark();
    explicit Bookmark(QString url, QString name = QString(), QStringList tagList = QStringList(),
                      QString description = QString());
    friend QDebug operator<<(QDebug dbg, const Bookmark &bookmark);
    QJsonObject jsonObject() const;
    static QVector<Bookmark> parseBookmarks(const QString &text, bool withBasicUrls = false);
    static QString bookmarksWebServiceJsonText(const QVector<Bookmark> &bookmarks);
    static QString parsedBookmarksWebServiceJsonText(const QString &text,
                                                     bool withBasicUrls = false);
    bool operator==(const Bookmark &bookmark) const;
    static void mergeInList(QVector<Bookmark> &bookmarks, Bookmark &bookmark);
    static void mergeListInList(const QVector<Bookmark> &sourceBookmarks,
                                QVector<Bookmark> &destinationBookmarks);
    void mergeInList(QVector<Bookmark> &bookmarks);
    void merge(Bookmark &bookmark);

   protected:
    QString name;
    QString url;
    QStringList tags;
    QString description;
};
