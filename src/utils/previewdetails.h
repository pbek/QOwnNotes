#pragma once

#include <QHash>
#include <QString>
#include <QUrl>
#include <QVector>

namespace Utils {

// Keeps disclosure state separate from the source HTML and note contents.
class PreviewDetails {
   public:
    void setHtml(const QString &html, const QString &noteReference = QString());
    QString renderedHtml(bool expandAll = false) const;
    bool toggle(const QUrl &url);
    // Opens collapsed sections containing an id or name anchor, so links to it can scroll there.
    bool revealAnchor(const QString &name);
    static bool isToggleUrl(const QUrl &url);

   private:
    struct Section {
        int start;
        int bodyStart;
        int bodyEnd;
        int end;
        int summaryStart = -1;
        int summaryEnd = -1;
        QString label;
        QString key;
        bool initiallyOpen;
    };

    QString renderRange(int start, int end, bool expandAll) const;
    QString _html;
    QString _noteReference;
    QVector<Section> _sections;
    // First tag position of each id or name anchor
    QHash<QString, int> _anchors;
    // Only explicit user choices override the source's open attribute on refresh.
    QHash<QString, bool> _open;
};

}    // namespace Utils
