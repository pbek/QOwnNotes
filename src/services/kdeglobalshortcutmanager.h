#pragma once

#include <QKeySequence>
#include <QMap>
#include <QObject>

class QAction;

class KdeGlobalShortcutManager : public QObject {
    Q_OBJECT

   public:
    struct ShortcutInfo {
        QKeySequence keySequence;
        QString description;
    };

    explicit KdeGlobalShortcutManager(QObject *parent = nullptr);
    ~KdeGlobalShortcutManager() override;

    static bool isAvailable();
    bool configure(const QMap<QString, ShortcutInfo> &shortcuts);

   signals:
    void shortcutActivated(const QString &actionName);
    void shortcutChanged(const QString &actionName, const QKeySequence &sequence);
    void serviceChanged();

   private:
    void releaseActions();
    void synchronizeShortcut(QAction *action);

    QMap<QString, QAction *> _actions;
    QMap<QString, QKeySequence> _configuredShortcuts;
    bool _connected = false;
    bool _configuring = false;
    bool _hasConfigured = false;
};
