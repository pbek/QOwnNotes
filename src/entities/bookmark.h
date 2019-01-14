/**
 * Bookmark struct
 */


#pragma once


#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtCore/QVariant>

struct Bookmark {
    QString name;
    QString url;
    Bookmark() = default;
    Bookmark(QString url) : url(url) {}
    Bookmark(QString url, QString name) : name(name), url(url) {}

//    QDebug operator<<(QDebug dbg, const Bookmark &bookmark) {
//        dbg.nospace() << "Bookmark: <name>" << bookmark.name <<
//                      " <url>" << bookmark.url;
//        return dbg.space();
//    }

    QJsonObject jsonObject() {
        QJsonObject bookmarkObject;
        bookmarkObject.insert("name", QJsonValue::fromVariant(name));
        bookmarkObject.insert("url", QJsonValue::fromVariant(url));
        return bookmarkObject;
    };
};
