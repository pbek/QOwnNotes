/**
 * CommandSnippet header
 */

#pragma once

#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

class CommandSnippet {
   public:
    CommandSnippet();
    explicit CommandSnippet(QString command, QString description = QString(),
                            QStringList tagList = QStringList());
    friend QDebug operator<<(QDebug dbg, const CommandSnippet &commandSnippet);
    QJsonObject jsonObject() const;
    static QVector<CommandSnippet> parseCommandSnippets(const QString &text,
                                                        bool withPrefixOnly = false);
    static QString commandSnippetsWebServiceJsonText(
        const QVector<CommandSnippet> &commandSnippets);
    static QString parsedCommandSnippetsWebServiceJsonText(const QString &text,
                                                           bool withPrefixOnly = false);
    bool operator==(const CommandSnippet &commandSnippet) const;
    static void mergeInList(QVector<CommandSnippet> &commandSnippets,
                            CommandSnippet &commandSnippet);
    static void mergeListInList(const QVector<CommandSnippet> &sourceCommandSnippets,
                                QVector<CommandSnippet> &destinationCommandSnippets);
    void mergeInList(QVector<CommandSnippet> &commandSnippets);
    void merge(CommandSnippet &commandSnippet);

   protected:
    QString command;
    QStringList tags;
    QString description;
};
