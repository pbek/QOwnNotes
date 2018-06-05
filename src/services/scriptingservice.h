#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QFile>
#include <entities/script.h>
#include <entities/notefolder.h>
#include <entities/note.h>
#include <QMimeData>
#include <QVariant>
#include <QHash>
#include <QMap>
#include <api/noteapi.h>
#include <QMessageBox>

#define PERSISTENT_VARIABLE_SETTINGS_PREFIX "PersistentScripting"

struct ScriptComponent {
    QQmlComponent *component;
    QObject *object;
    Script script;
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
    QVariant callNoteTaggingHook(Note note, QString action,
                                 QString tagName = "", QString newTagName = "");
    bool noteTaggingHookExists();
    bool handleNoteNameHookExists();
    bool methodExists(QString methodName);
    static bool validateScript(Script script, QString &errorMessage);
    Q_INVOKABLE bool startDetachedProcess(QString executablePath,
                                          QStringList parameters);
    Q_INVOKABLE QByteArray startSynchronousProcess(
            QString executablePath, QStringList parameters,
            QByteArray data = QByteArray());
    Q_INVOKABLE QString currentNoteFolderPath();
    Q_INVOKABLE NoteApi *currentNote();
    Q_INVOKABLE void log(QString text);
    Q_INVOKABLE QString downloadUrlToString(QUrl url);
    Q_INVOKABLE QString downloadUrlToMedia(QUrl url,
                                           bool returnUrlOnly = false);
    Q_INVOKABLE QString insertMediaFile(QString mediaFilePath,
                                        bool returnUrlOnly = false);
    Q_INVOKABLE void registerCustomAction(
            QString identifier, QString menuText, QString buttonText = "",
            QString icon = "", bool useInNoteEditContextMenu = false,
            bool hideButtonInToolbar = false,
            bool useInNoteListContextMenu = false);
    Q_INVOKABLE void createNote(QString text);
    Q_INVOKABLE QString clipboard(bool asHtml = false);
    Q_INVOKABLE void noteTextEditWrite(QString text);
    Q_INVOKABLE QString noteTextEditSelectedText();
    Q_INVOKABLE void noteTextEditSelectAll();
    Q_INVOKABLE QString noteTextEditCurrentWord(
            bool withPreviousCharacters = false);
    Q_INVOKABLE void encryptionDisablePassword();
    Q_INVOKABLE bool platformIsLinux();
    Q_INVOKABLE bool platformIsOSX();
    Q_INVOKABLE bool platformIsWindows();
    Q_INVOKABLE void tagCurrentNote(QString tagName);
    Q_INVOKABLE void addStyleSheet(QString stylesheet);
    Q_INVOKABLE void reloadScriptingEngine();
    Q_INVOKABLE NoteApi* fetchNoteByFileName(QString fileName,
                                             int noteSubFolderId = -1);
    Q_INVOKABLE NoteApi* fetchNoteById(int id);
    Q_INVOKABLE bool noteExistsByFileName(QString fileName,
                                          int ignoreNoteId = 0,
                                          int noteSubFolderId = -1);
    Q_INVOKABLE void setClipboardText(QString text, bool asHtml = false);
    Q_INVOKABLE void setCurrentNote(NoteApi *note);

    Q_INVOKABLE void informationMessageBox(QString text, QString title = "");

    Q_INVOKABLE int questionMessageBox(
            QString text, QString title = "",
            int buttons =
            QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
            int defaultButton = QMessageBox::NoButton);

    Q_INVOKABLE QString getOpenFileName(QString caption = "", QString dir = "",
                                        QString filter = "");

    Q_INVOKABLE QString getSaveFileName(QString caption = "", QString dir = "",
                                        QString filter = "");

    Q_INVOKABLE void registerLabel(QString identifier, QString text = "");

    Q_INVOKABLE void setLabelText(QString identifier, QString text);

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
    QString callEncryptionHookForObject(QObject *object, QString text,
                                        QString password, bool decrypt = false);
    QString callEncryptionHook(QString text, QString password,
                               bool decrypt = false);
    void callHandleNoteOpenedHook(Note *note);
    QString callHandleNoteNameHook(Note *note);
    void callHandleNoteDoubleClickedHook(Note *note);
    QList<QVariant> getSettingsVariables(int scriptId);
    Q_INVOKABLE QString toNativeDirSeparators(QString path);
    Q_INVOKABLE QString fromNativeDirSeparators(QString path);
    Q_INVOKABLE QString dirSeparator();
    Q_INVOKABLE QStringList selectedNotesPaths();

    Q_INVOKABLE QString inputDialogGetItem(
            const QString &title, const QString &label,
            const QStringList &items, int current = 0, bool editable = false);

    Q_INVOKABLE QString inputDialogGetText(
            const QString &title, const QString &label, const QString &text = "");

    Q_INVOKABLE void setPersistentVariable(const QString &key,
                                           const QVariant &value);

    Q_INVOKABLE QVariant getPersistentVariable(
            const QString &key, const QVariant &defaultValue = QVariant());

    Q_INVOKABLE QVariant getApplicationSettingsVariable(
            const QString &key, const QVariant &defaultValue = QVariant());

    Q_INVOKABLE bool jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                         QString separator = "/");
    QStringList callAutocompletionHook();

    Q_INVOKABLE QStringList searchTagsByName(QString name);

    Q_INVOKABLE void regenerateNotePreview();

    Q_INVOKABLE QList<int> selectedNotesIds();

    Q_INVOKABLE bool writeToFile(const QString &filePath, const QString &data);

    Q_INVOKABLE QList<int> fetchNoteIdsByNoteTextPart(QString text);

private:
    QQmlEngine *_engine;
    NoteApi *_currentNoteApi;
    QMap<int, ScriptComponent> _scriptComponents;
    QHash<int, QList<QVariant>> _settingsVariables;
    bool methodExistsForObject(QObject *object, QString method);
    QString callInsertMediaHookForObject(QObject *object,
                                         QFile *file,
                                         QString markdownText);
    QString callNoteToMarkdownHtmlHookForObject(QObject *object, Note *note,
                                                QString html);
    void initComponent(Script script);
    void outputMethodsOfObject(QObject *object);
    void reloadScriptComponents();
    void clearCustomStyleSheets();
    QList<QVariant> registerSettingsVariables(QObject *object, Script script);

signals:
    void noteStored(QVariant note);

public slots:
    void onCurrentNoteChanged(Note *note);
    void reloadEngine();
    void onCustomActionInvoked(QString identifier);
    void callCustomActionInvokedForObject(QObject *object, QString identifier);
};
