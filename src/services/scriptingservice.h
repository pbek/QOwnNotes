#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QtCore/QFile>
#include <entities/script.h>
#include <entities/notefolder.h>
#include <entities/note.h>
#include <QMimeData>
#include <api/noteapi.h>


struct ScriptComponent {
    QQmlComponent *component;
    QObject *object;
};

class ScriptingService : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(NoteApi currentNote READ currentNote())
//    Q_PROPERTY(NoteApi currentNoteX MEMBER _currentNoteX READ currentNote)

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
    Q_INVOKABLE NoteApi *currentNote();
    QString callInsertingFromMimeDataHookForObject(QObject *object,
                                                   const QMimeData *mimeData);
    QString callInsertingFromMimeDataHook(const QMimeData *mimeData);

private:
    QQmlEngine *_engine;
    NoteApi *_currentNoteAPi;
    QHash<int, ScriptComponent> _scriptComponents;
    bool methodExistsForObject(QObject *object, QString method);
    QString callInsertMediaHookForObject(QObject *object,
                                         QFile *file,
                                         QString markdownText);
    void initComponent(Script script);
    void outputMethodsOfObject(QObject *object);

signals:
    void noteStored(QVariant note);

public slots:
    void onCurrentNoteChanged(Note *note);
};
