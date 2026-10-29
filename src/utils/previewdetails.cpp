#include "previewdetails.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QStringList>

namespace {
struct Tag {
    int start;
    int end;
    QString name;
    bool closing;
    bool open;
    int depth;
    int partner;
    QStringList anchors;
};

bool nameCharacter(QChar c) {
    return c.isLetterOrNumber() || c == QLatin1Char('-') || c == QLatin1Char(':') ||
           c == QLatin1Char('_');
}

const QStringList &voidTags() {
    static const QStringList tags = {
        QStringLiteral("area"),  QStringLiteral("base"),  QStringLiteral("br"),
        QStringLiteral("col"),   QStringLiteral("embed"), QStringLiteral("hr"),
        QStringLiteral("img"),   QStringLiteral("input"), QStringLiteral("link"),
        QStringLiteral("meta"),  QStringLiteral("param"), QStringLiteral("source"),
        QStringLiteral("track"), QStringLiteral("wbr")};
    return tags;
}

const QStringList &literalTags() {
    static const QStringList tags = {QStringLiteral("pre"),      QStringLiteral("code"),
                                     QStringLiteral("script"),   QStringLiteral("style"),
                                     QStringLiteral("textarea"), QStringLiteral("title"),
                                     QStringLiteral("xmp")};
    return tags;
}

// Finds "</name" followed by optional whitespace and ">" without compiling a regex per tag
int findEndTag(const QString &html, const QString &name, int from) {
    const QString needle = QStringLiteral("</") + name;
    int pos = from;
    while ((pos = html.indexOf(needle, pos, Qt::CaseInsensitive)) != -1) {
        int after = pos + needle.size();
        while (after < html.size() && html.at(after).isSpace()) ++after;
        if (after < html.size() && html.at(after) == QLatin1Char('>')) return pos;
        pos += needle.size();
    }
    return -1;
}

// Tokenize tags rather than matching whole sections with a regex. Quoted attributes,
// comments and literal/raw-text elements must not introduce disclosure sections.
QVector<Tag> tokenize(const QString &html) {
    QVector<Tag> tags;
    QVector<int> stack;
    int pos = 0;
    while ((pos = html.indexOf(QLatin1Char('<'), pos)) != -1) {
        const int start = pos++;
        if (start + 3 < html.size() && html.at(start + 1) == QLatin1Char('!') &&
            html.at(start + 2) == QLatin1Char('-') && html.at(start + 3) == QLatin1Char('-')) {
            const int end = html.indexOf(QStringLiteral("-->"), pos);
            if (end == -1) break;
            pos = end + 3;
            continue;
        }
        bool closing = false;
        if (pos < html.size() && html.at(pos) == QLatin1Char('/')) {
            closing = true;
            ++pos;
        }
        const int nameStart = pos;
        while (pos < html.size() && nameCharacter(html.at(pos))) ++pos;
        const QString name = html.mid(nameStart, pos - nameStart).toLower();
        QChar quote;
        const int attributesStart = pos;
        while (pos < html.size()) {
            const QChar c = html.at(pos);
            if (!quote.isNull()) {
                if (c == quote) quote = QChar();
            } else if (c == QLatin1Char('\'') || c == QLatin1Char('"')) {
                quote = c;
            } else if (c == QLatin1Char('>')) {
                break;
            }
            ++pos;
        }
        if (pos == html.size()) break;
        const int attributesEnd = pos;
        ++pos;
        if (name.isEmpty()) continue;

        bool open = false;
        QStringList anchors;
        int attr = attributesStart;
        while (attr < attributesEnd) {
            while (attr < attributesEnd && !nameCharacter(html.at(attr))) ++attr;
            const int attrStart = attr;
            while (attr < attributesEnd && nameCharacter(html.at(attr))) ++attr;
            const QString attrName = html.mid(attrStart, attr - attrStart).toLower();
            if (attrName == QLatin1String("open")) {
                open = true;
            }
            while (attr < attributesEnd && html.at(attr).isSpace()) ++attr;
            if (attr < attributesEnd && html.at(attr) == QLatin1Char('=')) {
                ++attr;
                while (attr < attributesEnd && html.at(attr).isSpace()) ++attr;
                int valueStart = attr;
                int valueEnd;
                if (attr < attributesEnd &&
                    (html.at(attr) == QLatin1Char('\'') || html.at(attr) == QLatin1Char('"'))) {
                    const QChar delimiter = html.at(attr++);
                    valueStart = attr;
                    while (attr < attributesEnd && html.at(attr) != delimiter) ++attr;
                    valueEnd = attr;
                    if (attr < attributesEnd) ++attr;
                } else {
                    while (attr < attributesEnd && !html.at(attr).isSpace()) ++attr;
                    valueEnd = attr;
                }
                if (!closing &&
                    (attrName == QLatin1String("id") || attrName == QLatin1String("name"))) {
                    anchors.append(html.mid(valueStart, valueEnd - valueStart));
                }
            }
        }

        const int index = tags.size();
        tags.append({start, pos, name, closing, open, int(stack.size()), -1, anchors});
        if (closing) {
            for (int i = stack.size() - 1; i >= 0; --i) {
                if (tags.at(stack.at(i)).name == name) {
                    tags[index].depth = i;
                    tags[index].partner = stack.at(i);
                    tags[stack.at(i)].partner = index;
                    stack.resize(i);
                    break;
                }
            }
        } else if (!voidTags().contains(name)) {
            stack.append(index);
            if (literalTags().contains(name)) {
                const int endTag = findEndTag(html, name, pos);
                if (endTag == -1) break;
                pos = endTag;
            }
        }
    }
    return tags;
}

QString summaryLabel(const QString &html) {
    // Avoid nested links when making the summary itself clickable.
    QString result = html;
    const auto tags = tokenize(html);
    for (int i = tags.size() - 1; i >= 0; --i) {
        if (tags.at(i).name == QLatin1String("a")) {
            result.remove(tags.at(i).start, tags.at(i).end - tags.at(i).start);
        }
    }
    return result;
}
}    // namespace

void Utils::PreviewDetails::setHtml(const QString &html, const QString &noteReference) {
    if (_noteReference != noteReference || html.isEmpty()) _open.clear();
    _noteReference = noteReference;
    _html = html;
    _sections.clear();
    _anchors.clear();
    const auto tags = tokenize(html);
    QHash<QString, int> occurrences;
    QHash<QString, bool> retained;
    for (int i = 0; i < tags.size(); ++i) {
        const auto &tag = tags.at(i);
        for (const QString &anchor : tag.anchors) {
            if (!_anchors.contains(anchor)) _anchors.insert(anchor, tag.start);
        }
        if (tag.name != QLatin1String("details") || tag.closing || tag.partner < 0) continue;
        const auto &close = tags.at(tag.partner);
        Section section;
        section.start = tag.start;
        section.bodyStart = tag.end;
        section.bodyEnd = close.start;
        section.end = close.end;
        section.initiallyOpen = tag.open;
        for (int j = i + 1; j < tag.partner; ++j) {
            const auto &summary = tags.at(j);
            if (summary.name == QLatin1String("summary") && !summary.closing &&
                summary.depth == tag.depth + 1 && summary.partner > j &&
                summary.partner < tag.partner) {
                const auto &summaryClose = tags.at(summary.partner);
                section.summaryStart = summary.start;
                section.summaryEnd = summaryClose.end;
                section.label = html.mid(summary.end, summaryClose.start - summary.end);
                break;
            }
        }
        if (section.summaryStart == -1) {
            section.label =
                QCoreApplication::translate("PreviewDetails", "Details").toHtmlEscaped();
        }
        QString parentKey;
        for (int j = _sections.size() - 1; j >= 0; --j) {
            if (_sections.at(j).end > section.end) {
                parentKey = _sections.at(j).key;
                break;
            }
        }
        // Summary-based identities preserve state when body text or preceding sections change.
        const QString identity = parentKey + QLatin1Char('\n') + section.label;
        const int occurrence = occurrences[identity]++;
        section.key = QString::fromLatin1(
            QCryptographicHash::hash(
                (identity + QLatin1Char('\n') + QString::number(occurrence)).toUtf8(),
                QCryptographicHash::Sha256)
                .toHex());
        if (_open.contains(section.key)) retained.insert(section.key, _open.value(section.key));
        _sections.append(section);
    }
    _open = retained;
}

QString Utils::PreviewDetails::renderRange(int start, int end, bool expandAll) const {
    QString result;
    int pos = start;
    for (const auto &section : _sections) {
        if (section.start < pos || section.end > end) continue;
        result += _html.mid(pos, section.start - pos);
        const bool open = expandAll || _open.value(section.key, section.initiallyOpen);
        result += QStringLiteral("<div><p>");
        if (expandAll) {
            result += section.label;
        } else {
            result +=
                QStringLiteral("<a href=\"qownnotes-details:%1\">%2 %3</a>")
                    .arg(section.key, open ? QStringLiteral("&#9660;") : QStringLiteral("&#9654;"),
                         summaryLabel(section.label));
        }
        result += QStringLiteral("</p>");
        if (open) {
            if (section.summaryStart >= 0) {
                result += renderRange(section.bodyStart, section.summaryStart, expandAll);
                result += renderRange(section.summaryEnd, section.bodyEnd, expandAll);
            } else {
                result += renderRange(section.bodyStart, section.bodyEnd, expandAll);
            }
        }
        result += QStringLiteral("</div>");
        pos = section.end;
    }
    result += _html.mid(pos, end - pos);
    return result;
}

QString Utils::PreviewDetails::renderedHtml(bool expandAll) const {
    return renderRange(0, _html.size(), expandAll);
}

bool Utils::PreviewDetails::isToggleUrl(const QUrl &url) {
    return url.scheme() == QLatin1String("qownnotes-details");
}

bool Utils::PreviewDetails::revealAnchor(const QString &name) {
    if (!_anchors.contains(name)) return false;
    const int position = _anchors.value(name);
    bool changed = false;
    for (const auto &section : _sections) {
        if (position >= section.bodyStart && position < section.bodyEnd &&
            !_open.value(section.key, section.initiallyOpen)) {
            _open.insert(section.key, true);
            changed = true;
        }
    }
    return changed;
}

bool Utils::PreviewDetails::toggle(const QUrl &url) {
    if (!isToggleUrl(url)) return false;
    for (const auto &section : _sections) {
        if (section.key == url.path()) {
            _open.insert(section.key, !_open.value(section.key, section.initiallyOpen));
            return true;
        }
    }
    return false;
}
