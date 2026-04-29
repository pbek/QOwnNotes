#include "services/markdownlspignoredrules.h"

#include "services/settingsservice.h"

namespace {
const auto kMarkdownLspIgnoredRulesSettingsKey = QStringLiteral("Editor/markdownLspIgnoredRules");
}

MarkdownLspIgnoredRules *MarkdownLspIgnoredRules::instance() {
    static MarkdownLspIgnoredRules ignoredRules;
    return &ignoredRules;
}

MarkdownLspIgnoredRules::MarkdownLspIgnoredRules() { readSettings(); }

void MarkdownLspIgnoredRules::ignoreRule(const QString &ruleId) {
    const QString trimmedRuleId = ruleId.trimmed();
    if (trimmedRuleId.isEmpty() || _ignoredRules.contains(trimmedRuleId)) {
        return;
    }

    _ignoredRules.insert(trimmedRuleId);
    storeIgnoredRules();
    Q_EMIT ignoredRulesChanged();
}

bool MarkdownLspIgnoredRules::isRuleIgnored(const QString &ruleId) const {
    return _ignoredRules.contains(ruleId.trimmed());
}

void MarkdownLspIgnoredRules::clearIgnoredRules() {
    if (_ignoredRules.isEmpty()) {
        return;
    }

    _ignoredRules.clear();
    storeIgnoredRules();
    Q_EMIT ignoredRulesChanged();
}

QSet<QString> MarkdownLspIgnoredRules::ignoredRules() const { return _ignoredRules; }

void MarkdownLspIgnoredRules::readSettings() {
    SettingsService settings;
    const QStringList ignoredRulesList =
        settings.value(kMarkdownLspIgnoredRulesSettingsKey).toStringList();

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    _ignoredRules = QSet<QString>(ignoredRulesList.begin(), ignoredRulesList.end());
#else
    _ignoredRules.clear();
    for (const QString &ignoredRule : ignoredRulesList) {
        _ignoredRules.insert(ignoredRule);
    }
#endif

    _ignoredRules.remove(QString());
}

void MarkdownLspIgnoredRules::storeIgnoredRules() const {
    SettingsService settings;
    settings.setValue(kMarkdownLspIgnoredRulesSettingsKey, QStringList(_ignoredRules.values()));
}
