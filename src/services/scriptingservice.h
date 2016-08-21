#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QFile>
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
    Q_INVOKABLE void log(QString text);
    Q_INVOKABLE QString downloadUrlToString(QUrl url);
    Q_INVOKABLE void registerCustomAction(QString identifier, QString menuText,
                                          QString buttonText = "");
    QString callInsertingFromMimeDataHookForObject(QObject *object,
                                                   const QMimeData *mimeData);
    QString callInsertingFromMimeDataHook(const QMimeData *mimeData);
    QString callHandleNoteTextFileNameHookForObject(QObject *object,
                                                    Note *note);
    QString callHandleNoteTextFileNameHook(Note *note);
    QString callNoteToMarkdownHtmlHook(Note *note, QString html);

    QString callHandleNewNoteHeadlineHookForObject(QObject *object,
                                                   QString headline);
    QString callHandleNewNoteHeadlineHook(QString headline);

private:
    QQmlEngine *_engine;
    NoteApi *_currentNoteAPi;
    QHash<int, ScriptComponent> _scriptComponents;
    bool methodExistsForObject(QObject *object, QString method);
    QString callInsertMediaHookForObject(QObject *object,
                                         QFile *file,
                                         QString markdownText);
    QString callNoteToMarkdownHtmlHookForObject(QObject *object, Note *note,
                                                QString html);
    void initComponent(Script script);
    void outputMethodsOfObject(QObject *object);
    void reloadScriptComponents();

signals:
    void noteStored(QVariant note);

public slots:
    void onCurrentNoteChanged(Note *note);
    void reloadEngine();
    void onCustomActionInvoked(QString identifier);

    void callCustomActionInvokedForObject(QObject *object, QString identifier);
};
