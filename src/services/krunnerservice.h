#pragma once

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusContext>
#include <QDBusMessage>
#include <QList>
#include <QObject>
#include <QVariantMap>
#include <functional>

/**
 * A search result for KRunner
 */
struct KRunnerMatch {
    // Stable identifier, used to open the result
    QString id;
    QString text;
    QString iconName;
    // KRunner::QueryMatch::CategoryRelevance
    int categoryRelevance = 0;
    qreal relevance = 0;
    QVariantMap properties;
};

struct KRunnerAction {
    QString id;
    QString text;
    QString iconName;
};

using KRunnerMatches = QList<KRunnerMatch>;
using KRunnerActions = QList<KRunnerAction>;

Q_DECLARE_METATYPE(KRunnerMatch)
Q_DECLARE_METATYPE(KRunnerAction)

QDBusArgument &operator<<(QDBusArgument &argument, const KRunnerMatch &match);
const QDBusArgument &operator>>(const QDBusArgument &argument, KRunnerMatch &match);
QDBusArgument &operator<<(QDBusArgument &argument, const KRunnerAction &action);
const QDBusArgument &operator>>(const QDBusArgument &argument, KRunnerAction &action);

/**
 * Provides search results to KDE Plasma's KRunner (org.kde.krunner1)
 *
 * This only needs Qt D-Bus. KRunner uses the service if the plugin file
 * krunner/dbusplugins/org.qownnotes.QOwnNotes.desktop is installed. Each running instance
 * registers its own service name below the name in the plugin file, so KRunner shows the
 * results of all instances and none if QOwnNotes isn't running. QOwnNotes isn't started
 * by KRunner.
 *
 * Match calls are answered asynchronously: they are processed in the event loop, a newer
 * query of the same caller supersedes a pending one, and the number of results is capped.
 */
class KRunnerService : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.krunner1")

   public:
    // Searches results for the query, with at most the given number of results
    using SearchFunction = std::function<KRunnerMatches(const QString &query, int limit)>;

    static const QString serviceNamePrefix;
    static const QString objectPath;
    static const int maxResults = 10;
    static const int minQueryLength = 3;

    explicit KRunnerService(SearchFunction search, QObject *parent = nullptr);
    ~KRunnerService() override;

    bool registerOnBus(const QDBusConnection &connection);
    void unregisterFromBus();
    QString serviceName() const;

   signals:
    // Emitted if a result should be opened, with a token to activate the window
    void runRequested(const QString &matchId, const QString &actionId,
                      const QString &xdgActivationToken);

   public slots:
    // D-Bus API of org.kde.krunner1
    KRunnerActions Actions();
    KRunnerMatches Match(const QString &query);
    void Run(const QString &matchId, const QString &actionId);
    void SetActivationToken(const QString &token);
    void Teardown();

   private:
    struct PendingMatch {
        QDBusMessage message;
        QString query;
    };

    SearchFunction _search;
    QString _serviceName;
    QString _activationToken;
    QList<PendingMatch> _pendingMatches;
    bool _processingScheduled = false;
    bool _registered = false;
    QDBusConnection _connection;

    void processPendingMatches();
};
