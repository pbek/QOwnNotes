#pragma once

#include <api/tagapi.h>
#include <entities/script.h>
#include <entities/tag.h>
#include <threads/scriptthread.h>

#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QVariant>

#include "helpers/qownnotesmarkdownhighlighter.h"

class ScriptThread;
class QQmlComponent;
class QQmlEngine;
class NoteApi;
class Note;
class NoteSubFolderApi;
class QMimeData;

#define PERSISTENT_VARIABLE_SETTINGS_PREFIX "PersistentScripting"

struct ScriptComponent {
    QQmlComponent *component;
    QObject *object;
    Script script;
};

class ScriptingService : public QObject {
    Q_OBJECT
    //    Q_PROPERTY(NoteApi currentNote READ currentNote())
    //    Q_PROPERTY(NoteApi currentNoteX MEMBER _currentNoteX READ currentNote)

   public:
    explicit ScriptingService(QObject *parent = 0);
    static ScriptingService *instance();
    static ScriptingService *createInstance(QObject *parent);
    QQmlEngine *engine() const;
    void initComponents();
    QString callInsertMediaHook(QFile *file, QString markdownText);
    QString callInsertAttachmentHook(QFile *file, QString markdownText);
    void callWorkspaceSwitchedHook(const QString &oldUuid, const QString &newUuid);
    QVariant callNoteTaggingHook(const Note &note, const QString &action,
                                 const QString &tagName = QString(),
                                 const QString &newTagName = QString());
    QVariant callNoteTaggingByObjectHook(const Note &note, const QString &action,
                                         const Tag &tag = Tag(),
                                         const QString &newTagName = QString());
    void callWindowStateChangeHook(const QString &windowStateStr);
    bool noteTaggingHookExists() const;
    bool handleNoteNameHookExists() const;
    bool methodExists(const QString &methodName) const;
    static bool validateScript(const Script &script, QString &errorMessage);
    Q_INVOKABLE QString cacheDir(const QString &subFolder = QString()) const;
    Q_INVOKABLE bool clearCacheDir(const QString &subFolder = QString()) const;
    Q_INVOKABLE bool startDetachedProcess(const QString &executablePath,
                                          const QStringList &parameters,
                                          const QString &callbackIdentifier = QString(),
                                          const QVariant &callbackParameter = 0,
                                          const QByteArray &processData = QByteArray(),
                                          const QString &workingDirectory = "");
    Q_INVOKABLE QByteArray startSynchronousProcess(const QString &executablePath,
                                                   const QStringList &parameters,
                                                   const QByteArray &data = QByteArray(),
                                                   const QString &workingDirectory = "") const;
    Q_INVOKABLE QString currentNoteFolderPath();
    Q_INVOKABLE NoteApi *currentNote() const;
    Q_INVOKABLE void log(QString text);
    Q_INVOKABLE QString downloadUrlToString(const QUrl &url);
    Q_INVOKABLE QString downloadUrlToMedia(const QUrl &url, bool returnUrlOnly = false);
    Q_INVOKABLE QString aiComplete(const QString &prompt);
    Q_INVOKABLE QString insertMediaFile(const QString &mediaFilePath, bool returnUrlOnly = false);
    Q_INVOKABLE QString insertAttachmentFile(const QString &attachmentFilePath,
                                             const QString &fileName, bool returnUrlOnly);
    Q_INVOKABLE void registerCustomAction(const QString &identifier, const QString &menuText,
                                          const QString &buttonText = QString(),
                                          const QString &icon = QString(),
                                          bool useInNoteEditContextMenu = false,
                                          bool hideButtonInToolbar = false,
                                          bool useInNoteListContextMenu = false);
    Q_INVOKABLE void createNote(QString text);
    Q_INVOKABLE QString clipboard(bool asHtml = false);
    Q_INVOKABLE void noteTextEditWrite(const QString &text);
    Q_INVOKABLE QString noteTextEditSelectedText();
    Q_INVOKABLE void noteTextEditSelectAll();
    Q_INVOKABLE void noteTextEditSelectCurrentLine();
    Q_INVOKABLE void noteTextEditSelectCurrentWord();
    Q_INVOKABLE void noteTextEditSetSelection(int start, int end);
    Q_INVOKABLE void noteTextEditSetCursorPosition(int position);
    Q_INVOKABLE int noteTextEditCursorPosition();
    Q_INVOKABLE int noteTextEditSelectionStart();
    Q_INVOKABLE int noteTextEditSelectionEnd();
    Q_INVOKABLE QString noteTextEditCurrentWord(bool withPreviousCharacters = false);
    Q_INVOKABLE QString noteTextEditCurrentBlock();
    Q_INVOKABLE void encryptionDisablePassword();
    Q_INVOKABLE bool platformIsLinux();
    Q_INVOKABLE bool platformIsOSX();
    Q_INVOKABLE bool platformIsWindows();
    Q_INVOKABLE void tagCurrentNote(const QString &tagName);
    Q_INVOKABLE void addStyleSheet(const QString &stylesheet);
    Q_INVOKABLE void reloadScriptingEngine();
    Q_INVOKABLE NoteApi *fetchNoteByFileName(const QString &fileName, int noteSubFolderId = -1);
    Q_INVOKABLE NoteApi *fetchNoteById(int id);
    Q_INVOKABLE bool noteExistsByFileName(const QString &fileName, int ignoreNoteId = 0,
                                          int noteSubFolderId = -1);
    Q_INVOKABLE void setClipboardText(const QString &text, bool asHtml = false);
    Q_INVOKABLE void setCurrentNote(NoteApi *note, bool asTab = false);

    Q_INVOKABLE void informationMessageBox(const QString &text, const QString &title = QString());

    Q_INVOKABLE int questionMessageBox(const QString &text, const QString &title = QString(),
                                       int buttons = QMessageBox::StandardButtons(QMessageBox::Yes |
                                                                                  QMessageBox::No),
                                       int defaultButton = QMessageBox::NoButton);

    Q_INVOKABLE QString getOpenFileName(const QString &caption = QString(),
                                        const QString &dir = QString(),
                                        const QString &filter = QString());

    Q_INVOKABLE QString getSaveFileName(const QString &caption = QString(),
                                        const QString &dir = QString(),
                                        const QString &filter = QString());

    Q_INVOKABLE void registerLabel(const QString &identifier, const QString &text = QString());

    Q_INVOKABLE void setLabelText(const QString &identifier, const QString &text);

    QString callInsertingFromMimeDataHookForObject(QObject *object, const QMimeData *mimeData);
    QString callInsertingFromMimeDataHook(const QMimeData *mimeData);
    QString callHandleNoteTextFileNameHookForObject(QObject *object, Note *note);
    QString callHandleNoteTextFileNameHook(Note *note);
    QString callNoteToMarkdownHtmlHook(Note *note, const QString &html, const bool forExport);
    QString callPreNoteToMarkdownHtmlHook(Note *note, const QString &markdown,
                                          const bool forExport);

    QString callHandleNewNoteHeadlineHookForObject(QObject *object, const QString &headline);
    QString callHandleNewNoteHeadlineHook(const QString &headline);
    QString callEncryptionHookForObject(QObject *object, const QString &text,
                                        const QString &password, bool decrypt = false);
    QString callEncryptionHook(const QString &text, const QString &password, bool decrypt = false);
    void callHandleNoteOpenedHook(Note *note);
    QString callHandleNoteNameHook(Note *note);
    bool callHandleNoteDoubleClickedHook(Note *note);
    bool callHandleWebsocketRawDataHook(const QString &requestType, const QString &pageUrl,
                                        const QString &pageTitle, const QString &rawData,
                                        const QString &screenshotDataUrl);
    QList<QVariant> getSettingsVariables(int scriptId);
    Q_INVOKABLE QString toNativeDirSeparators(const QString &path);
    Q_INVOKABLE QString fromNativeDirSeparators(const QString &path);
    Q_INVOKABLE QString dirSeparator();
    Q_INVOKABLE QStringList selectedNotesPaths();

    Q_INVOKABLE QString inputDialogGetItem(const QString &title, const QString &label,
                                           const QStringList &items, int current = 0,
                                           bool editable = false);

    Q_INVOKABLE QString inputDialogGetText(const QString &title, const QString &label,
                                           const QString &text = QString());

    Q_INVOKABLE QString inputDialogGetMultiLineText(const QString &title, const QString &label,
                                                    const QString &text = QString());

    Q_INVOKABLE QString textDiffDialog(const QString &title, const QString &label, QString text1,
                                       QString text2);

    Q_INVOKABLE void setPersistentVariable(const QString &key, const QVariant &value);

    Q_INVOKABLE void addHighlightingRule(const QString &pattern, const QString &shouldContain,
                                         int state, int capturingGroup = 0, int maskedGroup = 0);

    QVector<QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule> getHighlightingRules();

    Q_INVOKABLE QVariant getPersistentVariable(const QString &key,
                                               const QVariant &defaultValue = QVariant());

    Q_INVOKABLE QVariant getApplicationSettingsVariable(const QString &key,
                                                        const QVariant &defaultValue = QVariant());

    Q_INVOKABLE bool jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                         const QString &separator = QStringLiteral("/"));
    QStringList callAutocompletionHook() const;

    QList<QVariantMap> callOpenAiBackendsHook() const;

    Q_INVOKABLE QStringList searchTagsByName(const QString &name) const;

    Q_INVOKABLE TagApi *getTagByNameBreadcrumbList(const QStringList &nameList,
                                                   bool createMissing = true) const;

    Q_INVOKABLE void regenerateNotePreview() const;

    Q_INVOKABLE QList<int> selectedNotesIds() const;

    Q_INVOKABLE bool writeToFile(const QString &filePath, const QString &data,
                                 const bool createParentDirs = false) const;
    Q_INVOKABLE QString readFromFile(const QString &filePath, const QString &codec = "UTF-8") const;
    Q_INVOKABLE bool fileExists(const QString &filePath) const;

    Q_INVOKABLE QVector<int> fetchNoteIdsByNoteTextPart(const QString &text) const;

    Q_INVOKABLE void triggerMenuAction(const QString &objectName,
                                       const QString &checked = QString()) const;

   private:
    QQmlEngine *_engine;
    NoteApi *_currentNoteApi;
    Note *_currentNote;
    QMap<int, ScriptComponent> _scriptComponents;
    QHash<int, QList<QVariant>> _settingsVariables;
    QVector<QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule> _highlightingRules;
    bool methodExistsForObject(QObject *object, const QString &method) const;
    QString callNoteToMarkdownHtmlHookForObject(ScriptComponent *scriptComponent, Note *note,
                                                const QString &html, const bool forExport);
    void initComponent(const Script &script);
    void outputMethodsOfObject(QObject *object);
    void reloadScriptComponents();
    void clearCustomStyleSheets();
    QList<QVariant> registerSettingsVariables(QObject *object, const Script &script);

   signals:
    void noteStored(QVariant note);

   public slots:
    void onCurrentNoteChanged(Note *note);
    void reloadEngine();
    void onCustomActionInvoked(const QString &identifier);
    void callCustomActionInvokedForObject(QObject *object, const QString &identifier);
    void onScriptThreadDone(ScriptThread *thread);
};
