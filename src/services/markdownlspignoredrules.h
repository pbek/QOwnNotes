#pragma once

#include <QObject>
#include <QSet>
#include <QString>

class MarkdownLspIgnoredRules : public QObject {
    Q_OBJECT

   public:
    static MarkdownLspIgnoredRules *instance();

    void ignoreRule(const QString &ruleId);
    bool isRuleIgnored(const QString &ruleId) const;
    void clearIgnoredRules();
    QSet<QString> ignoredRules() const;

   Q_SIGNALS:
    void ignoredRulesChanged();

   private:
    MarkdownLspIgnoredRules();

    void readSettings();
    void storeIgnoredRules() const;

    QSet<QString> _ignoredRules;
};
