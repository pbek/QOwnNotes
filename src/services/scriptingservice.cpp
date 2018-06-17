#include "scriptingservice.h"
#include <QCoreApplication>
#include <QDebug>
#include <QQmlContext>
#include <QQmlComponent>
#include <QFileInfo>
#include <QMimeData>
#include <entities/script.h>
#include <utils/misc.h>
#include <entities/notefolder.h>
#include <services/metricsservice.h>
#include <widgets/logwidget.h>
#include <QTimer>
#include <QVariant>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QRegularExpression>
#include <QSettings>
#include <api/scriptapi.h>

#ifndef INTEGRATION_TESTS
#include <mainwindow.h>
#include <QMessageBox>
#include <QInputDialog>
#endif

ScriptingService::ScriptingService(QObject *parent) : QObject(parent) {
    _engine = new QQmlEngine(this);
    _engine->rootContext()->setContextProperty("script", this);
#ifndef INTEGRATION_TESTS
    _engine->rootContext()->setContextProperty(
            "mainWindow", qApp->property("mainWindow").value<MainWindow *>());
#endif

    // deprecated
    qmlRegisterType<NoteApi>("com.qownnotes.noteapi", 1, 0, "NoteApi");
    // deprecated
    qmlRegisterType<TagApi>("com.qownnotes.tagapi", 1, 0, "TagApi");

    qmlRegisterType<NoteApi>("QOwnNotesTypes", 1, 0, "Note");
    qmlRegisterType<TagApi>("QOwnNotesTypes", 1, 0, "Tag");
    qmlRegisterType<ScriptApi>("QOwnNotesTypes", 1, 0, "Script");

    int scriptCount = Script::countAll();
    if (scriptCount > 0) {
        MetricsService::instance()->sendEventIfEnabled(
                "script/init",
                "script",
                "script count",
                QString::number(scriptCount) + " scripts",
                scriptCount);
    }
}

/**
 * Fetches the global instance of the class
 * The instance will be created if it doesn't exist.
 */
ScriptingService * ScriptingService::instance() {
    ScriptingService *scriptingService =
            qApp->property("scriptingService").value<ScriptingService *>();

    if (scriptingService == NULL) {
        scriptingService = createInstance(NULL);
    }

    return scriptingService;
}

/**
 * Creates a global instance of the class
 */
ScriptingService * ScriptingService::createInstance(QObject *parent) {
    ScriptingService *scriptingService = new ScriptingService(parent);

    qApp->setProperty(
            "scriptingService",
            QVariant::fromValue<ScriptingService *>(scriptingService));

    return scriptingService;
}

/**
 * Returns the engine
 */
QQmlEngine* ScriptingService::engine() {
    return _engine;
}

/**
 * Initializes a component from a script
 */
void ScriptingService::initComponent(Script script) {
    const QString path = script.getScriptPath();
    log("loading script file: " + path);
    const QUrl fileUrl = QUrl::fromLocalFile(path);

    ScriptComponent scriptComponent;
    QQmlComponent *component = new QQmlComponent(_engine);
    component->loadUrl(fileUrl);

    QObject *object = component->create();
    if (component->isReady() && !component->isError()) {
        scriptComponent.component = component;
        scriptComponent.object = object;
        scriptComponent.script = script;

        int scriptId = script.getId();
        int priority = script.getPriority();
        // generate a key from priority and scriptId
        // this key will be used to check the hooks in the correct order
        int key = (priority << 10) + scriptId;
        _scriptComponents[key] = scriptComponent;

        // set the script directory path of the script
        object->setProperty(
                "scriptDirPath",
                QDir::toNativeSeparators(script.getScriptDirPath()));

        // register the script settings variables
        QList<QVariant> list = registerSettingsVariables(object, script);
        if (list.count() > 0) {
            _settingsVariables[scriptId] = list;
        }

        // call the init function if it exists
        if (methodExistsForObject(object, "init()")) {
            QMetaObject::invokeMethod(object, "init");
        }

        if (methodExistsForObject(object, "onNoteStored(QVariant)")) {
            QObject::connect(this, SIGNAL(noteStored(QVariant)),
                             object, SLOT(onNoteStored(QVariant)));
        }
    } else {
        qWarning() << "script errors: " << component->errors();
    }
}

/**
 * Registers the script settings variables
 *
 * @param object
 * @param script
 */
QList<QVariant> ScriptingService::registerSettingsVariables(
        QObject *object, Script script) {
    // registerSettingsVariables will override the settingsVariables property
    if (methodExistsForObject(object, "registerSettingsVariables()")) {
        QVariant variables;
        QMetaObject::invokeMethod(object, "registerSettingsVariables");
    }

    // unfortunately there is no QVariantHash in Qt, we only can use
    // QVariantMap (that has no arbitrary ordering) or QVariantList (which at
    // least can be ordered arbitrarily)
    QList<QVariant> list = object->property("settingsVariables").toList();

    if (list.count() > 0) {
        QJsonObject jsonObject = script.getSettingsVariablesJsonObject();

        // set the properties in the script
        foreach (QVariant variable, list) {
                QMap<QString, QVariant> variableMap = variable.toMap();
                QString type = variableMap["type"].toString();
                QString identifier = variableMap["identifier"].toString();

                if (type == "integer") {
                    int value = jsonObject.value(identifier).toInt();

                    if (jsonObject.value(identifier).isUndefined()) {
                        value = variableMap["default"].toInt();
                    }

                    object->setProperty(identifier.toUtf8(), value);
                } else if (type == "boolean") {
                    bool value = jsonObject.value(identifier).toBool();

                    if (jsonObject.value(identifier).isUndefined()) {
                        value = variableMap["default"].toBool();
                    }

                    object->setProperty(identifier.toUtf8(), value);
                } else {
                    QString value = jsonObject.value(identifier).toString();

                    if (jsonObject.value(identifier).isUndefined()) {
                        value = variableMap["default"].toString();
                    }

                    object->setProperty(identifier.toUtf8(), value);
                }
            }
    } else {
        Q_UNUSED(script);
    }

    return list;
}

/**
 * Reloads all script components
 */
void ScriptingService::reloadScriptComponents() {
#ifndef INTEGRATION_TESTS
    // do some things in the main window (like clearing the custom actions)
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        mainWindow->preReloadScriptingEngine();
    }
#endif

    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    // delete all objects and components
    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        delete(scriptComponent.object);
        delete(scriptComponent.component);
    }

    // clear the component cache
    _engine->clearComponentCache();

    // enable the password dialog
    qApp->setProperty("encryptionPasswordDisabled", false);

    // init the components again
    initComponents();
}

/**
 * Checks if the script can be used in a component
 */
bool ScriptingService::validateScript(Script script,
                                      QString &errorMessage) {
    const QString path = script.getScriptPath();
    QFile file(path);

    if (!file.exists()) {
        errorMessage = tr("file doesn't exist");
        return false;
    }

    const QUrl fileUrl = QUrl::fromLocalFile(path);

    QQmlEngine *engine = new QQmlEngine();
    QQmlComponent *component = new QQmlComponent(engine);
    component->loadUrl(fileUrl);

    // we need the object to get all errors
    QObject *object = component->create();

    bool result = component->isReady() && !component->isError();

    if (!result) {
        errorMessage = component->errorString();
    }

    delete(object);
    delete(component);
    return result;
}

/**
 * Initializes all components
 */
void ScriptingService::initComponents() {
    clearCustomStyleSheets();
    _scriptComponents.clear();
    _settingsVariables.clear();
    QList<Script> scripts = Script::fetchAll();

    Q_FOREACH(Script script, scripts) {
            if (script.isEnabled()) {
                initComponent(script);
            }
        }
}

/**
 * Reloads the engine
 */
void ScriptingService::reloadEngine() {
    reloadScriptComponents();
}

/**
 * Returns the registered script variables
 *
 * @return
 */
QList<QVariant> ScriptingService::getSettingsVariables(int scriptId) {
    return _settingsVariables[scriptId];
}

/**
 * Reloads the scripting engine
 *
 * This is done in the background so the application doesn't crash when
 * called from a script
 */
void ScriptingService::reloadScriptingEngine() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    QTimer::singleShot(500, this, SLOT(reloadEngine()));
}

/**
 * Checks if a method exists for an object
 */
bool ScriptingService::methodExistsForObject(QObject *object,
                                             QString method) {
    return object->metaObject()->indexOfMethod(method.toStdString().c_str())
           > -1;
}

/**
 * Outputs the method signatures of an object for debugging
 */
void ScriptingService::outputMethodsOfObject(QObject *object) {
    const QMetaObject *metaObject = object->metaObject();

    for (int i = 0; i <= metaObject->methodCount(); i++) {
        qDebug() << metaObject->method(i).methodSignature();
    }
}

/**
 * Calls the insertMediaHook function for an object
 */
QString ScriptingService::callInsertMediaHookForObject(
        QObject *object,
        QFile *file,
        QString markdownText) {
    if (methodExistsForObject(object,
                              "insertMediaHook(QVariant,QVariant)")) {
        QVariant newMarkdownText;
        QMetaObject::invokeMethod(object, "insertMediaHook",
                                  Q_RETURN_ARG(QVariant, newMarkdownText),
                                  Q_ARG(QVariant, file->fileName()),
                                  Q_ARG(QVariant, markdownText));
        return newMarkdownText.toString();
    }

    return "";
}

/**
 * Calls the insertMediaHook function for all script components
 * This function is called when media file is inserted into the note
 */
QString ScriptingService::callInsertMediaHook(QFile *file,
                                              QString markdownText) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callInsertMediaHookForObject(scriptComponent.object,
                                                    file, markdownText);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return markdownText;
}

/**
 * Calls the noteTaggingHook function for all script components
 * This function is called when tags are added to, removed from or renamed in
 * notes or the tags of a note should be listed
 *
 * @param note
 * @param action can be "add", "remove", "rename" or "list"
 * @param tagName tag name to be added, removed or renamed
 * @param newTagName tag name to be renamed to if action = "rename"
 * @return QString or QStringList (if action = "list") inside a QVariant
 */
QVariant ScriptingService::callNoteTaggingHook(Note note, QString action,
                                               QString tagName,
                                               QString newTagName) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    NoteApi *noteApi = NoteApi::fromNote(note);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QVariant result;

        if (methodExistsForObject(
                scriptComponent.object,
                "noteTaggingHook(QVariant,QVariant,QVariant,QVariant)")) {
            QMetaObject::invokeMethod(scriptComponent.object,
                                      "noteTaggingHook",
                                      Q_RETURN_ARG(QVariant, result),
                                      Q_ARG(QVariant, QVariant::fromValue(
                                              static_cast<QObject*>(noteApi))),
                                      Q_ARG(QVariant, action),
                                      Q_ARG(QVariant, tagName),
                                      Q_ARG(QVariant, newTagName));

            if (!result.isNull()) {
                return result;
            }
        }
    }

    return QVariant();
}

/**
 * Checks if a noteTaggingHook function exists in a script
 * @return true if a function was found
 */
bool ScriptingService::noteTaggingHookExists() {
    return methodExists("noteTaggingHook(QVariant,QVariant,QVariant,QVariant)");
}

/**
 * Checks if a handleNoteNameHook function exists in a script
 * @return true if a function was found
 */
bool ScriptingService::handleNoteNameHookExists() {
    return methodExists("handleNoteNameHook(QVariant)");
}

/**
 * Checks if a method exists in a script
 * @return true if method was found
 */
bool ScriptingService::methodExists(QString methodName) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        if (methodExistsForObject(scriptComponent.object, methodName)) {
            return true;
        }
    }

    return false;
}

/**
 * Calls the autocompletionHook function for all script components
 * This function is called when autocompletion is invoked in a note
 *
 * @return QStringList of text for the autocomplete list
 */
QStringList ScriptingService::callAutocompletionHook() {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    QStringList results;

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QVariant result;

        if (methodExistsForObject(
                scriptComponent.object,
                "autocompletionHook()")) {
            QMetaObject::invokeMethod(scriptComponent.object,
                                      "autocompletionHook",
                                      Q_RETURN_ARG(QVariant, result));

            if (!result.isNull()) {
                results.append(result.toStringList());
            }
        }
    }

    results.sort();
    return results;
}

/**
 * Calls the insertingFromMimeDataHook function for an object
 */
QString ScriptingService::callInsertingFromMimeDataHookForObject(
        QObject *object,
        const QMimeData *mimeData) {
    if (methodExistsForObject(
            object,
            "insertingFromMimeDataHook(QVariant,QVariant)")) {
        QVariant text;
        QMetaObject::invokeMethod(object, "insertingFromMimeDataHook",
                                  Q_RETURN_ARG(QVariant, text),
                                  Q_ARG(QVariant, mimeData->text()),
                                  Q_ARG(QVariant, mimeData->html()));
        return text.toString();
    }

    return "";
}

/**
 * Calls the insertingFromMimeDataHook function for all script components
 * This function is called when html or a media file is pasted to a note with
 * `Ctrl + Shift + V`
 */
QString ScriptingService::callInsertingFromMimeDataHook(
        const QMimeData *mimeData) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callInsertingFromMimeDataHookForObject(
                scriptComponent.object, mimeData);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return "";
}

/**
 * Calls the handleNoteTextFileNameHook function for an object
 * This function is called when a note gets stored to disk if
 * "Allow note file name to be different from headline" is enabled
 * in the settings
 */
QString ScriptingService::callHandleNoteTextFileNameHookForObject(
        QObject *object,
        Note *note) {
    if (methodExistsForObject(
            object,
            "handleNoteTextFileNameHook(QVariant)")) {
        NoteApi *noteApi = new NoteApi();
        noteApi->fetch(note->getId());

        QVariant text;
        QMetaObject::invokeMethod(object, "handleNoteTextFileNameHook",
                                  Q_RETURN_ARG(QVariant, text),
                                  Q_ARG(QVariant, QVariant::fromValue(
                                          static_cast<QObject*>(noteApi))));
        return text.toString();
    }

    return "";
}

/**
 * Calls the handleNoteTextFileNameHook function for all script components
 */
QString ScriptingService::callHandleNoteTextFileNameHook(
        Note *note) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callHandleNoteTextFileNameHookForObject(
                scriptComponent.object, note);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return "";
}


/**
 * Calls the handleNewNoteHeadlineHook function for an object
 * This function is called when new note gets created
 */
QString ScriptingService::callHandleNewNoteHeadlineHookForObject(
        QObject *object, QString headline) {
    if (methodExistsForObject(
            object,
            "handleNewNoteHeadlineHook(QVariant)")) {

        QVariant text;
        QMetaObject::invokeMethod(object, "handleNewNoteHeadlineHook",
                                  Q_RETURN_ARG(QVariant, text),
                                  Q_ARG(QVariant, headline));
        return text.toString();
    }

    return "";
}

/**
 * Calls the noteOpenedHook function for all script components
 */
void ScriptingService::callHandleNoteOpenedHook(Note *note) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QObject *object = scriptComponent.object;

        if (methodExistsForObject(object, "noteOpenedHook(QVariant)")) {
            NoteApi *noteApi = new NoteApi();
            noteApi->fetch(note->getId());

            QMetaObject::invokeMethod(object, "noteOpenedHook",
                                      Q_ARG(QVariant, QVariant::fromValue(
                                              static_cast<QObject*>(noteApi))));
        }
    }
}

/**
 * Calls the handleNoteNameHook function for all script components
 */
QString ScriptingService::callHandleNoteNameHook(Note *note) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QObject *object = scriptComponent.object;

        if (methodExistsForObject(object, "handleNoteNameHook(QVariant)")) {
            NoteApi *noteApi = new NoteApi();
            noteApi->fetch(note->getId());

            QVariant text;
            QMetaObject::invokeMethod(object, "handleNoteNameHook",
                                      Q_RETURN_ARG(QVariant, text),
                                      Q_ARG(QVariant, QVariant::fromValue(
                                              static_cast<QObject*>(noteApi))));
            return text.toString();
        }
    }

    return "";
}

/**
 * Calls the handleNewNoteHeadlineHook function for all script components
 */
QString ScriptingService::callHandleNewNoteHeadlineHook(QString headline) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callHandleNewNoteHeadlineHookForObject(
                scriptComponent.object, headline);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return "";
}

/**
 * Calls the noteToMarkdownHtmlHook function for an object
 */
QString ScriptingService::callNoteToMarkdownHtmlHookForObject(
        QObject *object, Note *note, QString html) {
    if (methodExistsForObject(
            object,
            "noteToMarkdownHtmlHook(QVariant,QVariant)")) {
        NoteApi *noteApi = new NoteApi();
        noteApi->fetch(note->getId());

        QVariant text;
        QMetaObject::invokeMethod(object, "noteToMarkdownHtmlHook",
                                  Q_RETURN_ARG(QVariant, text),
                                  Q_ARG(QVariant, QVariant::fromValue(
                                          static_cast<QObject*>(noteApi))),
                                  Q_ARG(QVariant, html));
        return text.toString();
    }

    return "";
}

/**
 * Calls the noteToMarkdownHtmlHook function for all script components
 * This function is called when the markdown html of a note is generated
 */
QString ScriptingService::callNoteToMarkdownHtmlHook(
        Note *note, QString html) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    QString resultHtml = html;

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callNoteToMarkdownHtmlHookForObject(
                scriptComponent.object, note, resultHtml);

        if (!text.isEmpty()) {
            resultHtml = text;
        }
    }

    return html == resultHtml ? "" : resultHtml;
}

/**
 * Calls the encryptionHook function for an object
 *
 * @param object
 * @param text the text to encrypt or decrypt
 * @param password the password
 * @param decrypt if false encryption is demanded, if true decryption is
 * demanded
 * @return
 */
QString ScriptingService::callEncryptionHookForObject(
        QObject *object, QString text, QString password, bool decrypt) {
    if (methodExistsForObject(
            object, "encryptionHook(QVariant,QVariant,QVariant)")) {
        QVariant result;
        QMetaObject::invokeMethod(object, "encryptionHook",
                                  Q_RETURN_ARG(QVariant, result),
                                  Q_ARG(QVariant, text),
                                  Q_ARG(QVariant, password),
                                  Q_ARG(QVariant, decrypt));
        return result.toString();
    }

    return "";
}

/**
 * Calls the encryptionHook function for all script components
 * This function is called when notes are encrypted or decrypted
 *
 * @param text
 * @param password the password
 * @param decrypt if false encryption is demanded, if true decryption is
 * demanded
 * @return
 */
QString ScriptingService::callEncryptionHook(QString text, QString password,
                                             bool decrypt) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString result = callEncryptionHookForObject(scriptComponent.object,
                                                     text, password, decrypt);
        if (!result.isEmpty()) {
            return result;
        }
    }

    return "";
}

/**
 * Calls the noteDoubleClickedHook function for all script components
 */
void ScriptingService::callHandleNoteDoubleClickedHook(Note *note) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QObject *object = scriptComponent.object;

        if (methodExistsForObject(object, "noteDoubleClickedHook(QVariant)")) {
            NoteApi *noteApi = new NoteApi();
            noteApi->fetch(note->getId());

            QMetaObject::invokeMethod(object, "noteDoubleClickedHook",
                                      Q_ARG(QVariant, QVariant::fromValue(
                                              static_cast<QObject*>(noteApi))));
        }
    }
}
/**
 * QML wrapper to start a detached process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @return true on success, false otherwise
 */
bool ScriptingService::startDetachedProcess(QString executablePath,
                                            QStringList parameters) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    return Utils::Misc::startDetachedProcess(executablePath, parameters);
}

/**
 * QML wrapper to start a synchronous process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @return the text that was returned by the process
 */
QByteArray ScriptingService::startSynchronousProcess(
        QString executablePath, QStringList parameters, QByteArray data) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    return Utils::Misc::startSynchronousProcess(
            executablePath, parameters, data);
}

/**
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
 */
QString ScriptingService::currentNoteFolderPath() {
    return NoteFolder::currentNoteFolder().getLocalPath();
}

/**
 * Updates the current note for the scripts
 */
void ScriptingService::onCurrentNoteChanged(Note *note) {
    _currentNoteApi = new NoteApi();
    _currentNoteApi->fetch(note->getId());
}

/**
 * Calls the customActionInvoked function in all scripts
 */
void ScriptingService::onCustomActionInvoked(QString identifier) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        callCustomActionInvokedForObject(scriptComponent.object, identifier);
    }
}

/**
 * Calls the customActionInvoked function for an object
 */
void ScriptingService::callCustomActionInvokedForObject(QObject *object,
                                                        QString identifier) {
    if (methodExistsForObject(object, "customActionInvoked(QVariant)")) {
        QMetaObject::invokeMethod(object, "customActionInvoked",
                                  Q_ARG(QVariant, identifier));
    }
}

/**
 * QML wrapper to get the current note
 *
 * @return {NoteApi} the the current note
 */
NoteApi* ScriptingService::currentNote() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    return _currentNoteApi;
}

/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        mainWindow->writeToNoteTextEdit(text);
    }
#else
    Q_UNUSED(text);
#endif
}

/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    return mainWindow != Q_NULLPTR ?
           mainWindow->selectedNoteTextEditText() : "";
#else
    return "";
#endif
}

/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        mainWindow->activeNoteTextEdit()->selectAll();
    }
#endif
}

/**
 * Reads the current word in the note text edit
 *
 * @param withPreviousCharacters also get more characters at the beginning
 *                               to get characters like "@" that are not
 *                               word-characters
 * @return
 */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    return mainWindow != Q_NULLPTR ?
           mainWindow->noteTextEditCurrentWord(withPreviousCharacters) : "";
#else
    Q_UNUSED(withPreviousCharacters);
    return "";
#endif
}

/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        mainWindow->linkTagNameToCurrentNote(tagName);
    }
#else
    Q_UNUSED(tagName);
#endif
}

/**
 * QML wrapper to log to the log widget
 *
 * @param text
 */
void ScriptingService::log(QString text) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        emit(mainWindow->log(LogWidget::ScriptingLogType, text));
    }
#else
    Q_UNUSED(text);
#endif
}

/**
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString ScriptingService::downloadUrlToString(QUrl url) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    return Utils::Misc::downloadUrl(url);
}

/**
 * QML wrapper to download an url to the media folder and returning the media
 * url or the markdown image text of the media
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString ScriptingService::downloadUrlToMedia(QUrl url, bool returnUrlOnly) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    return Note::downloadUrlToMedia(url, returnUrlOnly);
}

/**
 * QML wrapper to insert a media file into the media folder and returning
 * the media url or the markdown image text of the media
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString ScriptingService::insertMediaFile(QString mediaFilePath,
                                          bool returnUrlOnly) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    QFile *mediaFile = new QFile(mediaFilePath);

    if (!mediaFile->exists()) {
        return "";
    }

    return Note::getInsertMediaMarkdown(mediaFile, true, returnUrlOnly);
}

/**
 * Regenerates the note preview
 */
void ScriptingService::regenerateNotePreview() {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        MetricsService::instance()->sendVisitIfEnabled(
                "scripting/" + QString(__func__));

        mainWindow->regenerateNotePreview();
    }
#endif
}

/**
 * Registers a custom action
 *
 * @param identifier the identifier of the action
 * @param menuText the text shown in the menu
 * @param buttonText the text shown in the button
 *                   (no button will be viewed if empty)
 * @param icon the icon file path or the name of a freedesktop theme icon
 *             you will find a list of icons here:
 *             https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu if true use the action in the note edit
 *                                 context menu (default: false)
 * @param hideButtonInToolbar if true the button will not be shown in the
 *                            custom action toolbar (default: false)
 * @param useInNoteListContextMenu if true use the action in the note list
 *                                 context menu (default: false)
 */
void ScriptingService::registerCustomAction(QString identifier,
                                            QString menuText,
                                            QString buttonText,
                                            QString icon,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        MetricsService::instance()->sendVisitIfEnabled(
                "scripting/" + QString(__func__));

        mainWindow->addCustomAction(identifier, menuText, buttonText, icon,
                                    useInNoteEditContextMenu,
                                    hideButtonInToolbar,
                                    useInNoteListContextMenu);
    }
#else
    Q_UNUSED(identifier);
    Q_UNUSED(menuText);
    Q_UNUSED(buttonText);
    Q_UNUSED(icon);
    Q_UNUSED(useInNoteEditContextMenu);
    Q_UNUSED(hideButtonInToolbar);
    Q_UNUSED(useInNoteListContextMenu);
#endif
}

/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        MetricsService::instance()->sendVisitIfEnabled(
                "scripting/" + QString(__func__));

        mainWindow->addScriptingLabel(identifier, text);
    }
#else
    Q_UNUSED(identifier);
    Q_UNUSED(text);
#endif
}

/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        MetricsService::instance()->sendVisitIfEnabled(
                "scripting/" + QString(__func__));

        mainWindow->setScriptingLabelText(identifier, text);
    }
#else
    Q_UNUSED(identifier);
    Q_UNUSED(text);
#endif
}

/**
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        MetricsService::instance()->sendVisitIfEnabled(
                "scripting/" + QString(__func__));

        // create a temporary name for the note
        QDateTime currentDate = QDateTime::currentDateTime();
        QString name = "Note " +
                currentDate.toString(Qt::ISODate).replace(":", ".");

        // create the new note and move the cursor to the end
        mainWindow->createNewNote(
                name, text, MainWindow::CreateNewNoteOptions(
                        MainWindow::CreateNewNoteOption::CursorAtEnd));
    }
#else
    Q_UNUSED(text);
#endif
}

/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);
    return asHtml ? mimeData->html() : mimeData->text();
}

/**
 * Disables the note encryption password dialog
 */
void ScriptingService::encryptionDisablePassword() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    qApp->setProperty("encryptionPasswordDisabled", true);
}

/**
 * Returns true if the current platform is Linux
 */
bool ScriptingService::platformIsLinux() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

/**
 * Returns true if the current platform is Mac OS X
 */
bool ScriptingService::platformIsOSX() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifdef Q_OS_MAC
    return true;
#else
    return false;
#endif
}

/**
 * Returns true if the current platform is Windows
 */
bool ScriptingService::platformIsWindows() {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    qApp->setStyleSheet(qApp->styleSheet() + "\n/* BEGIN CUSTOM STYLESHEET */\n"
                        + stylesheet + "\n/* END CUSTOM STYLESHEET */");
}

/**
 * Clears all custom stylesheets
 */
void ScriptingService::clearCustomStyleSheets() {
    QRegularExpression regExp(
            QRegularExpression::escape("/* BEGIN CUSTOM STYLESHEET */") + ".+" +
                    QRegularExpression::escape("/* END CUSTOM STYLESHEET */"),
            QRegularExpression::DotMatchesEverythingOption |
    QRegularExpression::MultilineOption);
    qApp->setStyleSheet(qApp->styleSheet().remove(regExp));
}

/**
 * Fetches a note by its file name
 *
 * @param fileName string the file name of the note (mandatory)
 * @param noteSubFolderId integer id of the note subfolder
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                               int noteSubFolderId) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    return NoteApi::fromNote(Note::fetchByFileName(fileName, noteSubFolderId));
}

/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    NoteApi *note = new NoteApi();
    note->fetch(id);
    return note;
}

/**
 * Checks if a note file exists by its file name
 *
 * @param fileName string the file name of the note (mandatory)
 * @param ignoreNoteId integer id of a note to ignore in the check
 * @param noteSubFolderId integer id of the note subfolder
 * @return bool
 */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    Note note = Note::fetchByFileName(fileName, noteSubFolderId);

    // check if the note id should be ignored
    if ((ignoreNoteId > 0) && (note.getId() == ignoreNoteId)) {
        return false;
    }

    return note.exists();
}

/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    QClipboard *clipboard = QApplication::clipboard();

    if (asHtml) {
        QMimeData *mimeData = new QMimeData();
        mimeData->setHtml(text);
        clipboard->setMimeData(mimeData);
    } else {
        clipboard->setText(text);
    }
}

/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 */
void ScriptingService::setCurrentNote(NoteApi *note) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        mainWindow->setCurrentNoteFromNoteId(note->getId());
    }
#else
    Q_UNUSED(note);
#endif
}

/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        QMessageBox::information(mainWindow, title, text);
    }
#else
    Q_UNUSED(text);
    Q_UNUSED(title);
#endif
}

/**
 * Shows a question message box
 *
 * For information about buttons see:
 * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
 *
 * @param text
 * @param title (optional)
 * @param buttons buttons that should be shown (optional)
 * @param defaultButton default button that will be selected (optional)
 * @return id of pressed button
 */
int ScriptingService::questionMessageBox(
        QString text, QString title, int buttons, int defaultButton) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        return QMessageBox::question(mainWindow, title, text,
                                     QMessageBox::StandardButtons(buttons),
                                     QMessageBox::StandardButton(
                                             defaultButton));
    }
#else
    Q_UNUSED(text);
    Q_UNUSED(title);
    Q_UNUSED(buttons);
    Q_UNUSED(defaultButton);
#endif

    return QMessageBox::NoButton;
}

/**
 * Shows an open file dialog
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getOpenFileName(QString caption, QString dir,
                                          QString filter) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        return QFileDialog::getOpenFileName(mainWindow, caption, dir, filter);
    }
#else
    Q_UNUSED(caption);
    Q_UNUSED(dir);
    Q_UNUSED(filter);
#endif

    return "";
}

/**
 * Shows an save file dialog
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                          QString filter) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != Q_NULLPTR) {
        return QFileDialog::getSaveFileName(mainWindow, caption, dir, filter);
    }
#else
    Q_UNUSED(caption);
    Q_UNUSED(dir);
    Q_UNUSED(filter);
#endif

    return "";
}

/**
 * Returns path with the '/' separators converted to separators that are
 * appropriate for the underlying operating system.
 *
 * On Windows, toNativeDirSeparators("c:/winnt/system32") returns
 * "c:\winnt\system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path) {
    return QDir::toNativeSeparators(path);
}

/**
 * Returns path using '/' as file separator.
 * On Windows, for instance, fromNativeDirSeparators("c:\\winnt\\system32")
 * returns "c:/winnt/system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::fromNativeDirSeparators(QString path) {
    return QDir::fromNativeSeparators(path);
}

/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator() {
    return QDir::separator();
}

/**
 * Returns a list of the paths of all selected notes
 *
 * Unfortunately there is no easy way to use a QList<NoteApi*> in QML, so we
 * only will transfer the note filenames
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths() {
    QStringList selectedNotePaths;
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        Q_FOREACH(Note note, mainWindow->selectedNotes()) {
                selectedNotePaths << QDir::toNativeSeparators(
                        note.fullNoteFilePath() );
            }
    }
#endif

    return selectedNotePaths;
}

/**
 * Returns a list of the ids of all selected notes
 *
 * Unfortunately there is no easy way to use a QList<NoteApi*> in QML, so we
 * only will transfer the note ids
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds() {
    QList<int> selectedNotesIds;
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        Q_FOREACH(Note note, mainWindow->selectedNotes()) {
                selectedNotesIds << note.getId();
            }
    }
#endif

    return selectedNotesIds;
}

/**
 * Returns a list of note ids of all notes with a certain text in the note text
 *
 * Unfortunately there is no easy way to use a QList<NoteApi*> in QML, so we
 * can only transfer the note ids
 *
 * @return {QList<int>} list of note ids
 */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text) {
    QList<int> noteIds = Note::fetchAllIdsByNoteTextPart(text);
    return noteIds;
}

/**
 * Opens an input dialog with a select box
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param items {QStringList} list of items to select
 * @param current {int} index of the item that should be selected (default: 0)
 * @param editable {bool} if true the text in the dialog can be edited (default: false)
 * @return {QString} text of the selected item
 */
QString ScriptingService::inputDialogGetItem(
        const QString &title, const QString &label, const QStringList &items,
        int current, bool editable) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    return QInputDialog::getItem(
            Q_NULLPTR, title, label, items, current, editable);
#else
    Q_UNUSED(title);
    Q_UNUSED(label);
    Q_UNUSED(items);
    Q_UNUSED(current);
    Q_UNUSED(editable);
    return "";
#endif
}

/**
 * Opens an input dialog with a line edit
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    return QInputDialog::getText(Q_NULLPTR, title, label,
                                 QLineEdit::Normal, text);
#else
    Q_UNUSED(title);
    Q_UNUSED(label);
    Q_UNUSED(text);
    return "";
#endif
}

/**
 * Stores a persistent variable
 * These variables are accessible globally over all scripts
 * Please use a meaningful prefix in your key like "PersistentVariablesTest/myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                             const QVariant &value) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    QSettings settings;
    settings.setValue(QString(PERSISTENT_VARIABLE_SETTINGS_PREFIX) + "/" + key,
                      value);
}

/**
 * Loads a persistent variable
 * These variables are accessible globally over all scripts
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                 const QVariant &defaultValue) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    QSettings settings;
    return settings.value(
            QString(PERSISTENT_VARIABLE_SETTINGS_PREFIX) + "/" + key,
            defaultValue);
}

/**
 * Loads an application settings variable
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getApplicationSettingsVariable(
        const QString &key, const QVariant &defaultValue) {
    QSettings settings;
    return settings.value(key, defaultValue);
}

/**
 * Jumps to a note subfolder
 *
 * @param noteSubFolderPath {QString} path of the subfolder, relative to the note folder
 * @param separator {QString} separator between parts of the path, default "/"
 * @return true if jump was successful
 */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                           QString separator) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == Q_NULLPTR) {
        return false;
    }

    NoteSubFolder folder = NoteSubFolder::fetchByPathData(noteSubFolderPath,
                                                          separator);

    if (!folder.isFetched()) {
        return false;
    }

    // jump to the note subfolder
    return mainWindow->jumpToNoteSubFolder(folder.getId());
#else
    Q_UNUSED(noteSubFolderPath);
    Q_UNUSED(separator);
    return false;
#endif
}

/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name) {
    MetricsService::instance()->sendVisitIfEnabled(
            "scripting/" + QString(__func__));

    QStringList tags = Tag::searchAllNamesByName(name);
    return tags;
}

/**
 * Writes a text to a file
 *
 * @param filePath
 * @param data
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data)
{
    if (filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
        return false;

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << data;
    file.close();
    return true;
}
