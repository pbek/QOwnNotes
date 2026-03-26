/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "listutils.h"

#include <QChar>
#include <QList>
#include <QRegularExpression>
#include <QStringList>

#include "misc.h"

namespace {
const QChar kSelectionNewlineCharacter(0x2029);

struct ListLineData {
    QString indentation;
    QString marker;
    QString content;
    bool isList = false;
    bool hasCheckbox = false;
    QChar checkboxState = QLatin1Char(' ');
};

struct CheckboxTreeNode {
    QString text;
    QList<CheckboxTreeNode> children;
};

static ListLineData parseListLine(const QString &line) {
    ListLineData data;
    if (line.trimmed().isEmpty()) {
        data.content = line;
        return data;
    }

    static const QRegularExpression listRegex(QStringLiteral(
        R"(^(\s*)(?:([+\-*])|(\d+[\.)])|([A-Za-z]+[\.)]))(?:\s\[( |x|X|-)\])?\s+(.*)$)"));
    const auto match = listRegex.match(line);
    if (match.hasMatch()) {
        data.indentation = match.captured(1);
        data.marker = !match.captured(2).isEmpty()
                          ? match.captured(2)
                          : (!match.captured(3).isEmpty() ? match.captured(3) : match.captured(4));
        data.content = match.captured(6);
        data.isList = true;
        data.hasCheckbox = !match.captured(5).isEmpty();
        data.checkboxState = data.hasCheckbox ? match.captured(5).at(0) : QLatin1Char(' ');
        return data;
    }

    static const QRegularExpression indentationRegex(QStringLiteral(R"(^(\s*)(.*)$)"));
    const auto indentationMatch = indentationRegex.match(line);
    data.indentation = indentationMatch.captured(1);
    data.content = indentationMatch.captured(2);
    return data;
}

static QString checkboxPrefix(const QString &marker, QChar state) {
    return marker + QStringLiteral(" [") + state + QStringLiteral("] ");
}

static QString alphabeticalListLabel(int index) {
    QString label;
    int value = index;

    do {
        const int remainder = value % 26;
        label.prepend(QChar(QLatin1Char('a').unicode() + remainder));
        value = (value / 26) - 1;
    } while (value >= 0);

    return label;
}

static int checkboxStateRank(QChar state) {
    switch (state.toLower().unicode()) {
        case 'x':
            return 0;
        case '-':
            return 1;
        case ' ':
        default:
            return 2;
    }
}

static bool checkboxStateForLine(const QString &line, QChar &state) {
    static const QRegularExpression checkboxRegex(
        QStringLiteral(R"(^\s*(?:[+\-*]|\d+[\.)]|[A-Za-z]+[\.)])\s\[( |x|X|-)\])"));
    const auto match = checkboxRegex.match(line);
    if (!match.hasMatch()) {
        return false;
    }

    state = match.captured(1).at(0).toLower();
    return true;
}

static void addCheckboxNode(QList<CheckboxTreeNode> &level, QString row,
                            const QString &singleIndentation) {
    if (!singleIndentation.isEmpty() && row.startsWith(singleIndentation) && !level.isEmpty()) {
        row.remove(0, singleIndentation.size());
        addCheckboxNode(level.last().children, row, singleIndentation);
        return;
    }

    level.append({row, {}});
}

static void sortCheckboxLevel(QList<CheckboxTreeNode> &level) {
    for (auto &item : level) {
        sortCheckboxLevel(item.children);
    }

    std::stable_sort(level.begin(), level.end(),
                     [](const CheckboxTreeNode &a, const CheckboxTreeNode &b) {
                         QChar stateA;
                         QChar stateB;
                         const bool hasStateA = checkboxStateForLine(a.text, stateA);
                         const bool hasStateB = checkboxStateForLine(b.text, stateB);

                         if (!hasStateA || !hasStateB) {
                             return false;
                         }

                         return checkboxStateRank(stateA) < checkboxStateRank(stateB);
                     });
}

static void appendCheckboxLevelText(const QList<CheckboxTreeNode> &level,
                                    const QString &topLevelIndentation,
                                    const QString &singleIndentation, const QString &prefix,
                                    QStringList &output) {
    for (const auto &item : level) {
        output.append(topLevelIndentation + prefix + item.text);
        if (!item.children.isEmpty()) {
            appendCheckboxLevelText(item.children, topLevelIndentation, singleIndentation,
                                    prefix + singleIndentation, output);
        }
    }
}

static QStringList selectedTextLines(const QString &text) {
    return Utils::ListUtils::normalizeSelectedText(text).split(QLatin1Char('\n'));
}
}    // namespace

QString Utils::ListUtils::normalizeSelectedText(QString text) {
    text.replace(kSelectionNewlineCharacter, QLatin1Char('\n'));
    return text;
}

QString Utils::ListUtils::toggleCheckboxes(const QString &text) {
    QStringList lines = selectedTextLines(text);
    for (QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const auto data = parseListLine(line);
        if (data.hasCheckbox) {
            QChar nextState = QLatin1Char(' ');
            switch (data.checkboxState.toLower().unicode()) {
                case 'x':
                    nextState = QLatin1Char('-');
                    break;
                case '-':
                    nextState = QLatin1Char(' ');
                    break;
                case ' ':
                default:
                    nextState = QLatin1Char('x');
                    break;
            }
            line = data.indentation + checkboxPrefix(data.marker, nextState) + data.content;
        } else if (data.isList) {
            line = data.indentation + checkboxPrefix(data.marker, QLatin1Char(' ')) + data.content;
        } else {
            line = data.indentation + checkboxPrefix(QStringLiteral("-"), QLatin1Char(' ')) +
                   data.content;
        }
    }

    return lines.join(QLatin1Char('\n'));
}

QString Utils::ListUtils::createUnorderedList(const QString &text) {
    QStringList lines = selectedTextLines(text);
    for (QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const auto data = parseListLine(line);
        line = data.indentation + QStringLiteral("- ") + data.content;
    }

    return lines.join(QLatin1Char('\n'));
}

QString Utils::ListUtils::createOrderedList(const QString &text) {
    QStringList lines = selectedTextLines(text);
    int index = 1;
    for (QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const auto data = parseListLine(line);
        line = data.indentation + QString::number(index++) + QStringLiteral(". ") + data.content;
    }

    return lines.join(QLatin1Char('\n'));
}

QString Utils::ListUtils::createAlphabeticalList(const QString &text) {
    QStringList lines = selectedTextLines(text);
    int index = 0;
    for (QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const auto data = parseListLine(line);
        line =
            data.indentation + alphabeticalListLabel(index++) + QStringLiteral(". ") + data.content;
    }

    return lines.join(QLatin1Char('\n'));
}

QString Utils::ListUtils::createCheckboxList(const QString &text) {
    QStringList lines = selectedTextLines(text);
    for (QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const auto data = parseListLine(line);
        line =
            data.indentation + checkboxPrefix(QStringLiteral("-"), QLatin1Char(' ')) + data.content;
    }

    return lines.join(QLatin1Char('\n'));
}

QString Utils::ListUtils::clearListFormatting(const QString &text) {
    QStringList lines = selectedTextLines(text);
    for (QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const auto data = parseListLine(line);
        line = data.indentation + data.content;
    }

    return lines.join(QLatin1Char('\n'));
}

QString Utils::ListUtils::orderCheckboxes(const QString &text) {
    const QString singleIndentation = Utils::Misc::indentCharacters();
    const QStringList rows = selectedTextLines(text);
    QString topLevelIndentation;
    bool foundFirstNonEmptyRow = false;
    QList<CheckboxTreeNode> structuredRows;

    for (const QString &row : rows) {
        if (row.trimmed().isEmpty()) {
            continue;
        }

        if (!foundFirstNonEmptyRow) {
            topLevelIndentation = parseListLine(row).indentation;
            foundFirstNonEmptyRow = true;
        }

        QString normalizedRow = row;
        if (!topLevelIndentation.isEmpty() && normalizedRow.startsWith(topLevelIndentation)) {
            normalizedRow.remove(0, topLevelIndentation.size());
        }

        addCheckboxNode(structuredRows, normalizedRow, singleIndentation);
    }

    if (structuredRows.isEmpty()) {
        return normalizeSelectedText(text);
    }

    sortCheckboxLevel(structuredRows);

    QStringList orderedRows;
    appendCheckboxLevelText(structuredRows, topLevelIndentation, singleIndentation, QString(),
                            orderedRows);
    return orderedRows.join(QLatin1Char('\n'));
}
