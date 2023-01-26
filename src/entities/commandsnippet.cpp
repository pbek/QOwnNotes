/**
 * CommandSnippet class
 */

#include "commandsnippet.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <utility>

CommandSnippet::CommandSnippet() {
    command = QString();
    tags = QStringList();
    description = QString();
}

CommandSnippet::CommandSnippet(QString command, QString description, QStringList tagList) {
    this->command = std::move(command);
    this->tags = std::move(tagList);
    this->description = std::move(description);
}

QJsonObject CommandSnippet::jsonObject() const {
    QJsonObject commandSnippetObject;
    commandSnippetObject.insert(QStringLiteral("command"), QJsonValue::fromVariant(command));
    commandSnippetObject.insert(QStringLiteral("tags"), QJsonValue::fromVariant(tags));
    commandSnippetObject.insert(QStringLiteral("description"),
                                QJsonValue::fromVariant(description));
    return commandSnippetObject;
}

QDebug operator<<(QDebug dbg, const CommandSnippet &commandSnippet) {
    dbg.nospace() << " <command>" << commandSnippet.command << " <tags>" << commandSnippet.tags
                  << " <description>" << commandSnippet.description;
    return dbg.space();
}

bool CommandSnippet::operator==(const CommandSnippet &commandSnippet) const {
    return command == commandSnippet.command;
}

/**
 * Parses commandSnippets from a text
 *
 * @param text
 * @return
 */
QVector<CommandSnippet> CommandSnippet::parseCommandSnippets(const QString &text,
                                                             bool withPrefixOnly) {
    QRegularExpressionMatchIterator i;
    QVector<CommandSnippet> commandSnippets;
    auto regex = QRegularExpression(withPrefixOnly ? QStringLiteral(R"([-*]\s+cmd:\s+`(.+?)`(.*)$)")
                                                   : QStringLiteral(R"([-*]\s+`(.+?)`(.*)$)"),
                                    QRegularExpression::MultilineOption);

    // parse command snippets like "- `my-command` #tag1 #tag2 description text"
    // with optional tags and description
    i = regex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString command = match.captured(1);
        QString additionalText = match.captured(2).trimmed();
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

        if (withPrefixOnly && !tags.contains(QStringLiteral("current"))) {
            tags << QStringLiteral("current");
        }

        auto commandSnippet = CommandSnippet(command, description, tags);
        commandSnippet.mergeInList(commandSnippets);
    }

    regex = QRegularExpression(
        QStringLiteral(R"(## ([^\n]+)\n(.*?)^```(bash|sh)\n(.+?)^```)"),
        QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);

    // parse snippets that are in a "bash" or "sh" code block,
    // preceded by a heading 2 or higher as a description
    i = regex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString description = match.captured(1);
        QString additionalText = match.captured(2).trimmed();
        QString command = match.captured(4);
        QStringList tags;

        if (!additionalText.isEmpty()) {
            QRegularExpressionMatchIterator addIterator =
                QRegularExpression(QStringLiteral(R"(#([^\s#]+))")).globalMatch(additionalText);
            while (addIterator.hasNext()) {
                QRegularExpressionMatch addMatch = addIterator.next();
                QString tag = addMatch.captured(1).trimmed();

                if (!tags.contains(tag)) {
                    tags << tag;
                }
            }
        }

        auto commandSnippet = CommandSnippet(command, description, tags);
        commandSnippet.mergeInList(commandSnippets);
    }

    return commandSnippets;
}

/**
 * Returns json text of a commandSnippet list for the WebSocketServerService
 *
 * @return
 */
QString CommandSnippet::commandSnippetsWebServiceJsonText(
    const QVector<CommandSnippet> &commandSnippets) {
    QJsonArray commandSnippetObjectList;

    for (const CommandSnippet &commandSnippet : commandSnippets) {
        commandSnippetObjectList.push_back(commandSnippet.jsonObject());
    }

    QJsonObject commandSnippetResultObject;
    commandSnippetResultObject.insert(QStringLiteral("type"),
                                      QJsonValue::fromVariant("commandSnippets"));
    commandSnippetResultObject.insert(QStringLiteral("data"), commandSnippetObjectList);

    QJsonDocument doc(commandSnippetResultObject);

    return doc.toJson(QJsonDocument::Compact);
}

/**
 * Returns json text parsed from links of a text for the WebSocketServerService
 *
 * @return
 */
QString CommandSnippet::parsedCommandSnippetsWebServiceJsonText(const QString &text,
                                                                bool withPrefixOnly) {
    return commandSnippetsWebServiceJsonText(parseCommandSnippets(text, withPrefixOnly));
}

/**
 * Merges the current commandSnippet into a list of commandSnippets
 */
void CommandSnippet::mergeInList(QVector<CommandSnippet> &commandSnippets) {
    int i = commandSnippets.indexOf(*this);

    if (i == -1) {
        commandSnippets.append(*this);
    } else {
        auto existingCommandSnippet = commandSnippets.at(i);

        // merge commandSnippets
        existingCommandSnippet.merge(*this);
        commandSnippets[i] = existingCommandSnippet;
    }
}

/**
 * Merges a commandSnippet into a list of commandSnippets
 */
void CommandSnippet::mergeInList(QVector<CommandSnippet> &commandSnippets,
                                 CommandSnippet &commandSnippet) {
    commandSnippet.mergeInList(commandSnippets);
}

/**
 * Merges the current commandSnippet with another
 */
void CommandSnippet::merge(CommandSnippet &commandSnippet) {
    tags.append(commandSnippet.tags);
    tags.removeDuplicates();
    tags.sort();

    if (!description.contains(commandSnippet.description)) {
        if (!description.isEmpty()) {
            description += QLatin1String(", ");
        }

        description += commandSnippet.description;
    }
}

/**
 * Merges a list of commandSnippets into another
 */
void CommandSnippet::mergeListInList(const QVector<CommandSnippet> &sourceCommandSnippets,
                                     QVector<CommandSnippet> &destinationCommandSnippets) {
    for (CommandSnippet commandSnippet : sourceCommandSnippets) {
        commandSnippet.mergeInList(destinationCommandSnippets);
    }
}
