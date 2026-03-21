#pragma once

#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QVector>

struct LanguageToolMatch {
    int offset = 0;
    int length = 0;
    QString message;
    QString shortMessage;
    QString ruleId;
    QString ruleCategory;
    QStringList replacements;
    QString contextText;
    QString ruleUrl;

    bool containsPosition(int position) const {
        return (position >= offset) && (position < (offset + length));
    }
};

Q_DECLARE_METATYPE(LanguageToolMatch)
Q_DECLARE_METATYPE(QVector<LanguageToolMatch>)
