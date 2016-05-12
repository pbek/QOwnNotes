#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QtCore/QFile>
#include <entities/script.h>
#include <entities/notefolder.h>
#include <QMimeData>


struct ScriptComponent {
    QQmlComponent *component;
    QObject *object;
};

class ScriptingService : public QObject
{
    Q_OBJECT

public:
    explicit ScriptingService(QObject *parent = 0);
    static ScriptingService *instance();
    static ScriptingService *createInstance(QObject *parent);
    QQmlEngine* engine();
    void initComponents();
    QString callInsertMediaHook(QFile *file, QString markdownText);
    static bool validateScript(Script script, QString &errorMessage);
    Q_INVOKABLE bool startDetachedProcess(QString executablePath,
                                          QStringList parameters);
    Q_INVOKABLE QString currentNoteFolderPath();
    QString callInsertingFromMimeDataHookForObject(QObject *object,
                                                   const QMimeData *mimeData);
    QString callInsertingFromMimeDataHook(const QMimeData *mimeData);

private:
    QQmlEngine *_engine;
    QHash<int, ScriptComponent> _scriptComponents;
    bool methodExistsForObject(QObject *object, QString method);
    QString callInsertMediaHookForObject(QObject *object,
                                         QFile *file,
                                         QString markdownText);
    void initComponent(Script script);
    void outputMethodsOfObject(QObject *object);

signals:
    void noteStored(QVariant fileName, QVariant noteText);

public slots:
};
