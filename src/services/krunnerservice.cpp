#include "krunnerservice.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMetaType>
#include <QDebug>
#include <QRegularExpression>
#include <QTimer>
#include <QUuid>

const QString KRunnerService::serviceNamePrefix = QStringLiteral("org.qownnotes.QOwnNotes.KRunner");
const QString KRunnerService::objectPath = QStringLiteral("/runner");
const int KRunnerService::maxResults;
const int KRunnerService::minQueryLength;

QDBusArgument &operator<<(QDBusArgument &argument, const KRunnerMatch &match) {
    argument.beginStructure();
    argument << match.id << match.text << match.iconName << match.categoryRelevance
             << match.relevance << match.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, KRunnerMatch &match) {
    argument.beginStructure();
    argument >> match.id >> match.text >> match.iconName >> match.categoryRelevance >>
        match.relevance >> match.properties;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const KRunnerAction &action) {
    argument.beginStructure();
    argument << action.id << action.text << action.iconName;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, KRunnerAction &action) {
    argument.beginStructure();
    argument >> action.id >> action.text >> action.iconName;
    argument.endStructure();
    return argument;
}

KRunnerService::KRunnerService(SearchFunction search, QObject *parent)
    : QObject(parent),
      _search(std::move(search)),
      _connection(QStringLiteral("qownnotes-krunner-unconnected")) {
    qDBusRegisterMetaType<KRunnerMatch>();
    qDBusRegisterMetaType<KRunnerMatches>();
    qDBusRegisterMetaType<KRunnerAction>();
    qDBusRegisterMetaType<KRunnerActions>();
}

KRunnerService::~KRunnerService() { unregisterFromBus(); }

bool KRunnerService::registerOnBus(const QDBusConnection &connection) {
    if (_registered || !connection.isConnected()) {
        return _registered;
    }

    _connection = connection;

    // A unique name per instance, because several instances (sessions) can run at the same
    // time, and PIDs aren't unique between sandboxes. Name elements must not start with a
    // digit.
    QString uuid = QUuid::createUuid().toString();
    uuid.remove(QRegularExpression(QStringLiteral("[{}-]")));
    _serviceName = serviceNamePrefix + QStringLiteral(".i") + uuid;

    if (!_connection.registerObject(objectPath, this, QDBusConnection::ExportAllSlots)) {
        qWarning() << "KRunner: could not register the D-Bus object";
        return false;
    }

    if (!_connection.registerService(_serviceName)) {
        qWarning() << "KRunner: could not register the D-Bus service" << _serviceName;
        _connection.unregisterObject(objectPath);
        return false;
    }

    _registered = true;
    return true;
}

void KRunnerService::unregisterFromBus() {
    if (!_registered) {
        return;
    }

    _connection.unregisterService(_serviceName);
    _connection.unregisterObject(objectPath);
    _registered = false;

    // Answer pending calls, so callers don't wait for a timeout
    const QList<PendingMatch> pendingMatches = _pendingMatches;
    _pendingMatches.clear();
    for (const PendingMatch &pending : pendingMatches) {
        _connection.send(pending.message.createReply(QVariant::fromValue(KRunnerMatches())));
    }
}

QString KRunnerService::serviceName() const { return _serviceName; }

KRunnerActions KRunnerService::Actions() { return {}; }

KRunnerMatches KRunnerService::Match(const QString &query) {
    if (!calledFromDBus()) {
        return _search ? _search(query.trimmed(), maxResults) : KRunnerMatches();
    }

    if (query.trimmed().size() < minQueryLength || !_search) {
        return {};
    }

    // Answer later, so the D-Bus call doesn't block the event loop
    setDelayedReply(true);
    _pendingMatches.append({message(), query.trimmed()});

    if (!_processingScheduled) {
        _processingScheduled = true;
        QTimer::singleShot(0, this, &KRunnerService::processPendingMatches);
    }

    return {};
}

void KRunnerService::processPendingMatches() {
    _processingScheduled = false;
    const QList<PendingMatch> pendingMatches = _pendingMatches;
    _pendingMatches.clear();

    for (int i = 0; i < pendingMatches.size(); ++i) {
        const PendingMatch &pending = pendingMatches.at(i);
        bool superseded = false;

        // KRunner queries while the user types, only the newest query of a caller matters
        for (int j = i + 1; j < pendingMatches.size(); ++j) {
            if (pendingMatches.at(j).message.service() == pending.message.service()) {
                superseded = true;
                break;
            }
        }

        KRunnerMatches matches;
        if (!superseded) {
            matches = _search(pending.query, maxResults).mid(0, maxResults);
        }

        _connection.send(pending.message.createReply(QVariant::fromValue(matches)));
    }
}

void KRunnerService::Run(const QString &matchId, const QString &actionId) {
    const QString token = _activationToken;
    _activationToken.clear();
    emit runRequested(matchId, actionId, token);
}

void KRunnerService::SetActivationToken(const QString &token) { _activationToken = token; }

void KRunnerService::Teardown() { _activationToken.clear(); }
