#ifndef LANGUAGECACHE_H
#define LANGUAGECACHE_H

#include <QTextBlockUserData>
// LanguageCache class
// * Copyright (C)  2004  Zack Rusin <zack@kde.org>
// * Copyright (C)  2006  Laurent Montel <montel@kde.org>
// * Copyright (C)  2013  Martin Sandsmark <martin.sandsmark@org>
class LanguageCache : public QTextBlockUserData {
   public:
    // Key: QPair<start, length>
    // Value: language name
    QMap<QPair<int, int>, QString> languages;

    // Remove all cached language information after @p pos
    void invalidate(int pos) {
        QMutableMapIterator<QPair<int, int>, QString> it(languages);
        it.toBack();
        while (it.hasPrevious()) {
            it.previous();
            if (it.key().first + it.key().second >= pos) {
                it.remove();
            } else {
                break;
            }
        }
    }

    QString languageAtPos(int pos) const {
        // The data structure isn't really great for such lookups...
        QMapIterator<QPair<int, int>, QString> it(languages);
        while (it.hasNext()) {
            it.next();
            if (it.key().first <= pos && it.key().first + it.key().second >= pos) {
                return it.value();
            }
        }
        return QString();
    }
};

#endif    // LANGUAGECACHE_H
