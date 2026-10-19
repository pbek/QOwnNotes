#include "kdeglobalshortcutmanager.h"

#include <KGlobalAccel>
#include <QAction>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDebug>
#include <QScopedValueRollback>

namespace {
const QString serviceName = QStringLiteral("org.kde.kglobalaccel");
}

KdeGlobalShortcutManager::KdeGlobalShortcutManager(QObject *parent) : QObject(parent) {
    auto *watcher = new QDBusServiceWatcher(serviceName, QDBusConnection::sessionBus(),
                                            QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged, this,
            [this](const QString &, const QString &, const QString &) { emit serviceChanged(); });
}

KdeGlobalShortcutManager::~KdeGlobalShortcutManager() { releaseActions(); }

bool KdeGlobalShortcutManager::isAvailable() {
    auto *bus = QDBusConnection::sessionBus().interface();
    if (bus == nullptr) {
        return false;
    }
    const QDBusReply<bool> reply = bus->isServiceRegistered(serviceName);
    return reply.isValid() && reply.value();
}

void KdeGlobalShortcutManager::releaseActions() {
    // Destroying actions marks them inactive without deleting Plasma's saved bindings.
    qDeleteAll(_actions);
    _actions.clear();
}

void KdeGlobalShortcutManager::synchronizeShortcut(QAction *action) {
    const QList<QKeySequence> sequences = KGlobalAccel::self()->shortcut(action);
    const QKeySequence sequence = sequences.isEmpty() ? QKeySequence() : sequences.first();
    const QString name = action->objectName();
    const bool changed = _configuredShortcuts.value(name) != sequence;
    _configuredShortcuts.insert(name, sequence);

    // Only report real changes, so unassigned actions don't touch the settings
    if (changed) {
        emit shortcutChanged(name, sequence);
    }
}

bool KdeGlobalShortcutManager::configure(const QMap<QString, ShortcutInfo> &shortcuts) {
    if (!isAvailable()) {
        releaseActions();
        return false;
    }

    auto *accel = KGlobalAccel::self();
    if (!_connected) {
        connect(accel, &KGlobalAccel::globalShortcutChanged, this,
                [this](QAction *action, const QKeySequence &) {
                    if (!_configuring && _actions.value(action->objectName()) == action) {
                        synchronizeShortcut(action);
                    }
                });
        _connected = true;
    }

    const QScopedValueRollback<bool> configuring(_configuring, true);
    for (const QString &name : _actions.keys()) {
        if (!shortcuts.contains(name)) {
            // Actions can disappear temporarily, e.g. while scripts reload. Deleting the proxy
            // only marks it inactive in Plasma, so its saved binding isn't lost
            delete _actions.take(name);
            _configuredShortcuts.remove(name);
        }
    }

    for (auto it = shortcuts.cbegin(); it != shortcuts.cend(); ++it) {
        QAction *action = _actions.value(it.key());
        const bool isNew = action == nullptr;
        if (isNew) {
            action = new QAction(this);
            action->setObjectName(it.key());
            action->setProperty("componentName", QCoreApplication::applicationName());
            action->setAutoRepeat(false);
            _actions.insert(it.key(), action);
            connect(action, &QAction::triggered, this,
                    [this, name = it.key()]() { emit shortcutActivated(name); });
        }
        action->setText(it.value().description);

        // Plasma's saved bindings win when an action is registered. Only an explicit
        // application-side edit of an already registered action overrides them; unchanged
        // reconfiguration preserves additional bindings that Plasma can store beyond the
        // single key shown by QOwnNotes.
        const bool changed = _hasConfigured && !isNew &&
                             _configuredShortcuts.value(it.key()) != it.value().keySequence;
        if (isNew) {
            // Start from the application's value, so an unchanged binding isn't reported
            _configuredShortcuts.insert(it.key(), it.value().keySequence);
        }
        const QList<QKeySequence> sequences = it.value().keySequence.isEmpty()
                                                  ? QList<QKeySequence>()
                                                  : QList<QKeySequence>{it.value().keySequence};
        if (!accel->setShortcut(
                action, sequences,
                changed ? KGlobalAccel::NoAutoloading : KGlobalAccel::Autoloading)) {
            qWarning() << "Failed to register KDE global shortcut:" << it.key();
            releaseActions();
            return false;
        }
        if (changed && !sequences.isEmpty() && accel->shortcut(action).isEmpty()) {
            qWarning() << "KDE global shortcut was rejected (possibly a conflict):" << it.key();
        }
        synchronizeShortcut(action);
    }
    _hasConfigured = true;
    return true;
}
