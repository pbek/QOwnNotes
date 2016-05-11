#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QtCore/QFile>


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
    QString callModifyMediaMarkdown(QFile *file, QString markdownText);

private:
    QQmlEngine *_engine;
    QHash<int, ScriptComponent> _scriptComponents;
    bool methodExistsForObject(QObject *object, QString method);
    QString callModifyMediaMarkdownForObject(QObject *object,
                                             QFile *file,
                                             QString markdownText);

signals:
    void noteStored(QVariant fileName, QVariant noteText);

public slots:

    void outputMethodsOfObject(QObject *object);
};
