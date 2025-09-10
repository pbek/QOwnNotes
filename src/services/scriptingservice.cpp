#include "scriptingservice.h"

#include <api/scriptapi.h>
#include <entities/notefolder.h>
#include <entities/script.h>
#include <services/metricsservice.h>
#include <utils/misc.h>
#include <widgets/logwidget.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>

#ifdef __GNUC__
#if __GNUC__ >= 11
QT_WARNING_DISABLE_GCC("-Wmismatched-new-delete")
#endif
#endif
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QTimer>
#include <QVariant>
#include <utility>

#include "api/noteapi.h"
#include "api/notesubfolderapi.h"
#include "api/tagapi.h"
#include "cryptoservice.h"
#include "entities/notesubfolder.h"
#include "services/settingsservice.h"

#ifndef INTEGRATION_TESTS
#include <mainwindow.h>

#include <QInputDialog>
#include <QMessageBox>

#include "dialogs/textdiffdialog.h"
#include "openaiservice.h"
#include "widgets/qownnotesmarkdowntextedit.h"
#endif

ScriptingService::ScriptingService(QObject *parent) : QObject(parent) {
    _engine = new QQmlEngine(this);
    _engine->rootContext()->setContextProperty(QStringLiteral("script"), this);
#ifndef INTEGRATION_TESTS
    if (!MainWindow::instance()) {
        qWarning() << "Unexpected null MainWindow in ScriptingService()";
    }
    _engine->rootContext()->setContextProperty(QStringLiteral("mainWindow"),
                                               MainWindow::instance());
#endif

    // deprecated
    qmlRegisterType<NoteApi>("com.qownnotes.noteapi", 1, 0, "NoteApi");
    // deprecated
    qmlRegisterType<TagApi>("com.qownnotes.tagapi", 1, 0, "TagApi");

    qmlRegisterType<NoteApi>("QOwnNotesTypes", 1, 0, "Note");
    qmlRegisterType<NoteSubFolderApi>("QOwnNotesTypes", 1, 0, "NoteSubFolder");
    qmlRegisterType<TagApi>("QOwnNotesTypes", 1, 0, "Tag");
    qmlRegisterType<ScriptApi>("QOwnNotesTypes", 1, 0, "Script");

    int scriptCount = Script::countAll();
    if (scriptCount > 0) {
        MetricsService::instance()->sendEventIfEnabled(
            QStringLiteral("script/init"), QStringLiteral("script"), QStringLiteral("script count"),
            QString::number(scriptCount) % QStringLiteral(" scripts"), scriptCount);
    }
}

/**
 * Fetches the global instance of the class
 * The instance will be created if it doesn't exist.
 */
ScriptingService *ScriptingService::instance() {
    ScriptingService *scriptingService =
        qApp->property("scriptingService").value<ScriptingService *>();

    if (scriptingService == nullptr) {
        scriptingService = createInstance(nullptr);
    }

    return scriptingService;
}

/**
 * Creates a global instance of the class
 */
ScriptingService *ScriptingService::createInstance(QObject *parent) {
    ScriptingService *scriptingService = new ScriptingService(parent);

    qApp->setProperty("scriptingService",
                      QVariant::fromValue<ScriptingService *>(scriptingService));

    return scriptingService;
}

/**
 * Returns the engine
 */
QQmlEngine *ScriptingService::engine() const { return _engine; }

/**
 * Initializes a component from a script
 */
void ScriptingService::initComponent(const Script &script) {
    const QString path = script.getScriptPath();
    log(QStringLiteral("loading script file: ") % path);
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
        object->setProperty("scriptDirPath", QDir::toNativeSeparators(script.getScriptDirPath()));

        // register the script settings variables
        QList<QVariant> list = registerSettingsVariables(object, script);
        if (list.count() > 0) {
            _settingsVariables[scriptId] = list;
        }

        // call the init function if it exists
        if (methodExistsForObject(object, QStringLiteral("init()"))) {
            QMetaObject::invokeMethod(object, "init");
        }

        if (methodExistsForObject(object, QStringLiteral("onNoteStored(QVariant)"))) {
            QObject::connect(this, SIGNAL(noteStored(QVariant)), object,
                             SLOT(onNoteStored(QVariant)));
        }
    } else {
        qWarning() << "script errors: " << component->errors();
        bool urlEmpty = component->errors().at(0).url().isEmpty();
        if (urlEmpty) script.remove();
    }
}

/**
 * Registers the script settings variables
 *
 * @param object
 * @param script
 */
QList<QVariant> ScriptingService::registerSettingsVariables(QObject *object, const Script &script) {
    // registerSettingsVariables will override the settingsVariables property
    if (methodExistsForObject(object, QStringLiteral("registerSettingsVariables()"))) {
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
            QString type = variableMap[QStringLiteral("type")].toString();
            QString identifier = variableMap[QStringLiteral("identifier")].toString();

            if (type == QStringLiteral("integer")) {
                int value = jsonObject.value(identifier).toInt();

                if (jsonObject.value(identifier).isUndefined()) {
                    value = variableMap[QStringLiteral("default")].toInt();
                }

                object->setProperty(identifier.toUtf8(), value);
            } else if (type == QStringLiteral("boolean")) {
                bool value = jsonObject.value(identifier).toBool();

                if (jsonObject.value(identifier).isUndefined()) {
                    value = variableMap[QStringLiteral("default")].toBool();
                }

                object->setProperty(identifier.toUtf8(), value);
            } else if (type == QStringLiteral("string-secret")) {
                QString value;
                // The secret identifier is the identifier with a "!" in front (so we can mask it in
                // the settings dump)
                const QString secretIdentifier = QStringLiteral("!") + identifier;

                if (!jsonObject.value(secretIdentifier).isUndefined()) {
                    value = jsonObject.value(secretIdentifier).toString();

                    // Decrypt the value if the value is not empty
                    if (!value.isEmpty()) {
                        value = CryptoService::instance()->decryptToString(value);
                    }
                }

                object->setProperty(identifier.toUtf8(), value);
            } else {
                QString value = jsonObject.value(identifier).toString();

                if (jsonObject.value(identifier).isUndefined()) {
                    value = variableMap[QStringLiteral("default")].toString();
                }

                object->setProperty(identifier.toUtf8(), value);
            }
        }
    } else {
        Q_UNUSED(script)
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
    if (mainWindow != nullptr) {
        mainWindow->preReloadScriptingEngine();
    }
#endif

    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    // delete all objects and components
    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        delete (scriptComponent.object);
        delete (scriptComponent.component);
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
bool ScriptingService::validateScript(const Script &script, QString &errorMessage) {
    const QString path = script.getScriptPath();
    QFile file(path);

    if (!file.exists()) {
        errorMessage = tr("file doesn't exist");
        return false;
    }

    const QUrl fileUrl = QUrl::fromLocalFile(path);

    auto *engine = new QQmlEngine();
    auto *component = new QQmlComponent(engine);
    component->loadUrl(fileUrl);

    // we need the object to get all errors
    QObject *object = component->create();

    bool result = component->isReady() && !component->isError();

    if (!result) {
        errorMessage = component->errorString();
    }

    delete (object);
    delete (component);
    delete (engine);
    return result;
}

/**
 * Initializes all components
 */
void ScriptingService::initComponents() {
    clearCustomStyleSheets();
    _scriptComponents.clear();
    _settingsVariables.clear();
    _highlightingRules.clear();

    // fetch enabled only
    const QList<Script> scripts = Script::fetchAll(true);

    for (const Script &script : scripts) {
        initComponent(script);
    }
}

/**
 * Reloads the engine
 */
void ScriptingService::reloadEngine() {
    reloadScriptComponents();

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        // Reload the OpenAI controls, so that the scripts can add custom backends
        mainWindow->reloadOpenAiControls();
    }
#endif
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
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    QTimer::singleShot(500, this, SLOT(reloadEngine()));
}

/**
 * Checks if a method exists for an object
 */
bool ScriptingService::methodExistsForObject(QObject *object, const QString &method) const {
    return object->metaObject()->indexOfMethod(method.toStdString().c_str()) > -1;
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
 * Calls the insertMediaHook function for all script components
 * This function is called when media file is inserted into the note
 */
QString ScriptingService::callInsertMediaHook(QFile *file, QString markdownText) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        if (methodExistsForObject(scriptComponent.object,
                                  QStringLiteral("insertMediaHook(QVariant,QVariant)"))) {
            QVariant newMarkdownText;
            QMetaObject::invokeMethod(
                scriptComponent.object, "insertMediaHook", Q_RETURN_ARG(QVariant, newMarkdownText),
                Q_ARG(QVariant, file->fileName()), Q_ARG(QVariant, markdownText));
            QString result = newMarkdownText.toString();

            if (!result.isEmpty()) {
                return result;
            }
        }
    }

    return markdownText;
}

/**
 * Calls the insertMediaHook function for all script components
 * This function is called when media file is inserted into the note
 */
QString ScriptingService::callInsertAttachmentHook(QFile *file, QString markdownText) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        if (methodExistsForObject(scriptComponent.object,
                                  QStringLiteral("insertAttachmentHook(QVariant,QVariant)"))) {
            QVariant newMarkdownText;
            QMetaObject::invokeMethod(scriptComponent.object, "insertAttachmentHook",
                                      Q_RETURN_ARG(QVariant, newMarkdownText),
                                      Q_ARG(QVariant, file->fileName()),
                                      Q_ARG(QVariant, markdownText));
            QString result = newMarkdownText.toString();

            if (!result.isEmpty()) {
                return result;
            }
        }
    }

    return markdownText;
}

/**
 * Calls the workspaceSwitchedHook function for all script components
 * This function is called when workspaces are switched
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
void ScriptingService::callWorkspaceSwitchedHook(const QString &oldUuid, const QString &newUuid) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        if (methodExistsForObject(scriptComponent.object,
                                  QStringLiteral("workspaceSwitchedHook(QVariant,QVariant)"))) {
            QMetaObject::invokeMethod(scriptComponent.object, "workspaceSwitchedHook",
                                      Q_ARG(QVariant, oldUuid), Q_ARG(QVariant, newUuid));
        }
    }
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
 * @return note text QString or QStringList of tag names (if action = "list") inside a QVariant
 */
QVariant ScriptingService::callNoteTaggingHook(const Note &note, const QString &action,
                                               const QString &tagName, const QString &newTagName) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    NoteApi *noteApi = NoteApi::fromNote(note);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QVariant result;

        if (methodExistsForObject(
                scriptComponent.object,
                QStringLiteral("noteTaggingHook(QVariant,QVariant,QVariant,QVariant)"))) {
            QMetaObject::invokeMethod(
                scriptComponent.object, "noteTaggingHook", Q_RETURN_ARG(QVariant, result),
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))),
                Q_ARG(QVariant, action), Q_ARG(QVariant, tagName), Q_ARG(QVariant, newTagName));

            if (!result.isNull()) {
                return result;
            }
        }
    }

    return QVariant();
}

/**
 * Calls the noteTaggingByObjectHook function for all script components
 * This function is called when tags are added to, removed from or renamed in
 * notes or the tags of a note should be listed
 *
 * @param note
 * @param action can be "add", "remove", "rename" or "list"
 * @param tag to be added, removed or renamed
 * @param newTagName tag name to be renamed to if action = "rename"
 * @return note text QString or QStringList of tag ids (if action = "list") inside a QVariant
 */
QVariant ScriptingService::callNoteTaggingByObjectHook(const Note &note, const QString &action,
                                                       const Tag &tag, const QString &newTagName) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    auto *noteApi = NoteApi::fromNote(note);
    auto *tagApi = TagApi::fromTag(tag);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QVariant result;

        if (methodExistsForObject(
                scriptComponent.object,
                QStringLiteral("noteTaggingByObjectHook(QVariant,QVariant,QVariant,QVariant)"))) {
            QMetaObject::invokeMethod(
                scriptComponent.object, "noteTaggingByObjectHook", Q_RETURN_ARG(QVariant, result),
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))),
                Q_ARG(QVariant, action),
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(tagApi))),
                Q_ARG(QVariant, newTagName));

            if (!result.isNull()) {
                return result;
            }
        }
    }

    return QVariant();
}

void ScriptingService::callWindowStateChangeHook(const QString &windowStateStr) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QObject *object = scriptComponent.object;

        if (methodExistsForObject(object, QStringLiteral("windowStateChangedHook(QVariant)"))) {
            QMetaObject::invokeMethod(object, "windowStateChangedHook",
                                      Q_ARG(QVariant, QVariant::fromValue(windowStateStr)));
        }
    }
}

/**
 * Checks if a noteTaggingHook or noteTaggingByObjectHook function exists in a script
 * @return true if a function was found
 */
bool ScriptingService::noteTaggingHookExists() const {
    return methodExists(QStringLiteral("noteTaggingHook(QVariant,QVariant,QVariant,QVariant)")) ||
           methodExists(
               QStringLiteral("noteTaggingByObjectHook(QVariant,QVariant,QVariant,QVariant)"));
}

/**
 * Checks if a handleNoteNameHook function exists in a script
 * @return true if a function was found
 */
bool ScriptingService::handleNoteNameHookExists() const {
    return methodExists(QStringLiteral("handleNoteNameHook(QVariant)"));
}

/**
 * Checks if a method exists in a script
 * @return true if method was found
 */
bool ScriptingService::methodExists(const QString &methodName) const {
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
QStringList ScriptingService::callAutocompletionHook() const {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    QStringList results;

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QVariant result;

        if (methodExistsForObject(scriptComponent.object, QStringLiteral("autocompletionHook()"))) {
            QMetaObject::invokeMethod(scriptComponent.object, "autocompletionHook",
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
 * Calls the openAiBackendsHook function for all script components
 * This function is called when OpenAI service is initialized
 *
 * @return QList of QJsonObject with the backend information
 */
QList<QVariantMap> ScriptingService::callOpenAiBackendsHook() const {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    QList<QVariantMap> results;

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QVariant result;

        if (methodExistsForObject(scriptComponent.object, QStringLiteral("openAiBackendsHook()"))) {
            QMetaObject::invokeMethod(scriptComponent.object, "openAiBackendsHook",
                                      Q_RETURN_ARG(QVariant, result));
            // Convert the returned value to QVariantList
            QVariantList resultList = result.toList();

            for (const QVariant &item : resultList) {
                QVariantMap itemMap = item.toMap();

                results.append(itemMap);
            }
        }
    }

    return results;
}

/**
 * Calls the insertingFromMimeDataHook function for an object
 */
QString ScriptingService::callInsertingFromMimeDataHookForObject(QObject *object,
                                                                 const QMimeData *mimeData) {
    if (methodExistsForObject(object,
                              QStringLiteral("insertingFromMimeDataHook(QVariant,QVariant)"))) {
        QVariant text;
        QMetaObject::invokeMethod(object, "insertingFromMimeDataHook", Q_RETURN_ARG(QVariant, text),
                                  Q_ARG(QVariant, mimeData->text()),
                                  Q_ARG(QVariant, mimeData->html()));
        return text.toString();
    }

    return {};
}

/**
 * Calls the insertingFromMimeDataHook function for all script components
 * This function is called when html or a media file is pasted to a note with
 * `Ctrl + Shift + V`
 */
QString ScriptingService::callInsertingFromMimeDataHook(const QMimeData *mimeData) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callInsertingFromMimeDataHookForObject(scriptComponent.object, mimeData);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return {};
}

/**
 * Calls the handleNoteTextFileNameHook function for an object
 * This function is called when a note gets stored to disk if
 * "Allow note file name to be different from headline" is enabled
 * in the settings
 */
QString ScriptingService::callHandleNoteTextFileNameHookForObject(QObject *object, Note *note) {
    if (methodExistsForObject(object, QStringLiteral("handleNoteTextFileNameHook(QVariant)"))) {
        auto *noteApi = new NoteApi();
        noteApi->fetch(note->getId());

        QVariant text;
        QMetaObject::invokeMethod(
            object, "handleNoteTextFileNameHook", Q_RETURN_ARG(QVariant, text),
            Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))));
        return text.toString();
    }

    return {};
}

/**
 * Calls the handleNoteTextFileNameHook function for all script components
 */
QString ScriptingService::callHandleNoteTextFileNameHook(Note *note) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callHandleNoteTextFileNameHookForObject(scriptComponent.object, note);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return {};
}

/**
 * Calls the handleNewNoteHeadlineHook function for an object
 * This function is called when new note gets created
 */
QString ScriptingService::callHandleNewNoteHeadlineHookForObject(QObject *object,
                                                                 const QString &headline) {
    if (methodExistsForObject(object, QStringLiteral("handleNewNoteHeadlineHook(QVariant)"))) {
        QVariant text;
        QMetaObject::invokeMethod(object, "handleNewNoteHeadlineHook", Q_RETURN_ARG(QVariant, text),
                                  Q_ARG(QVariant, headline));
        return text.toString();
    }

    return {};
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

        if (methodExistsForObject(object, QStringLiteral("noteOpenedHook(QVariant)"))) {
            auto *noteApi = new NoteApi();
            noteApi->fetch(note->getId());

            QMetaObject::invokeMethod(
                object, "noteOpenedHook",
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))));
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

        if (methodExistsForObject(object, QStringLiteral("handleNoteNameHook(QVariant)"))) {
            auto *noteApi = new NoteApi();
            noteApi->fetch(note->getId());

            QVariant text;
            QMetaObject::invokeMethod(
                object, "handleNoteNameHook", Q_RETURN_ARG(QVariant, text),
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))));
            return text.toString();
        }
    }

    return {};
}

/**
 * Calls the handleNewNoteHeadlineHook function for all script components
 */
QString ScriptingService::callHandleNewNoteHeadlineHook(const QString &headline) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callHandleNewNoteHeadlineHookForObject(scriptComponent.object, headline);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return {};
}

/**
 * Calls the noteToMarkdownHtmlHook function for an object
 */
QString ScriptingService::callNoteToMarkdownHtmlHookForObject(ScriptComponent *scriptComponent,
                                                              Note *note, const QString &html,
                                                              const bool forExport) {
    if (methodExistsForObject(
            scriptComponent->object,
            QStringLiteral("noteToMarkdownHtmlHook(QVariant,QVariant,QVariant)"))) {
        auto *noteApi = new NoteApi();
        noteApi->fetch(note->getId());

        QVariant text;
        QMetaObject::invokeMethod(
            scriptComponent->object, "noteToMarkdownHtmlHook", Q_RETURN_ARG(QVariant, text),
            Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))),
            Q_ARG(QVariant, html), Q_ARG(QVariant, forExport));
        return text.toString();
    } else if (methodExistsForObject(scriptComponent->object,
                                     QStringLiteral("noteToMarkdownHtmlHook(QVariant,QVariant)"))) {
        auto *noteApi = new NoteApi();
        noteApi->fetch(note->getId());
        qWarning() << "Warning: noteToMarkdownHtmlHook(note, html) "
                      "is deprecated, please use "
                      "noteToMarkdownHtmlHook(note, html, forExport) "
                      "in " +
                          scriptComponent->script.getName();
        QVariant text;
        QMetaObject::invokeMethod(
            scriptComponent->object, "noteToMarkdownHtmlHook", Q_RETURN_ARG(QVariant, text),
            Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))),
            Q_ARG(QVariant, html));
        return text.toString();
    }

    return {};
}

/**
 * Calls the noteToMarkdownHtmlHook function for all script components
 * This function is called when the Markdown html of a note is generated
 */
QString ScriptingService::callNoteToMarkdownHtmlHook(Note *note, const QString &html,
                                                     const bool forExport) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    QString resultHtml = html;

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text =
            callNoteToMarkdownHtmlHookForObject(&scriptComponent, note, resultHtml, forExport);

        if (!text.isEmpty()) {
            resultHtml = text;
        }
    }

    return html == resultHtml ? QString() : resultHtml;
}

/**
 * Calls the preNoteToMarkdownHtmlHook function for all script components
 * This function is called before the Markdown html of a note is generated
 */
QString ScriptingService::callPreNoteToMarkdownHtmlHook(Note *note, const QString &markdown,
                                                        const bool forExport) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    QString resultMarkdown = markdown;

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        //        QString text = callNoteToMarkdownHtmlHookForObject(
        //            scriptComponent.object, note, resultMarkdown);

        if (methodExistsForObject(
                scriptComponent.object,
                QStringLiteral("preNoteToMarkdownHtmlHook(QVariant,QVariant,QVariant)"))) {
            auto *noteApi = new NoteApi();
            noteApi->fetch(note->getId());

            QVariant resultText;
            QMetaObject::invokeMethod(
                scriptComponent.object, "preNoteToMarkdownHtmlHook",
                Q_RETURN_ARG(QVariant, resultText),
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))),
                Q_ARG(QVariant, resultMarkdown), Q_ARG(QVariant, forExport));
            QString text = resultText.toString();

            if (!text.isEmpty()) {
                resultMarkdown = text;
            }
        } else if (methodExistsForObject(
                       scriptComponent.object,
                       QStringLiteral("preNoteToMarkdownHtmlHook(QVariant,QVariant)"))) {
            qWarning() << "Warning: preNoteToMarkdownHtmlHook(note, markdown) "
                          "is deprecated, please use "
                          "preNoteToMarkdownHtmlHook(note, markdown, forExport) "
                          "in " +
                              scriptComponent.script.getName();
            auto *noteApi = new NoteApi();
            noteApi->fetch(note->getId());

            QVariant resultText;
            QMetaObject::invokeMethod(
                scriptComponent.object, "preNoteToMarkdownHtmlHook",
                Q_RETURN_ARG(QVariant, resultText),
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))),
                Q_ARG(QVariant, resultMarkdown));
            QString text = resultText.toString();

            if (!text.isEmpty()) {
                resultMarkdown = text;
            }
        }
    }

    return markdown == resultMarkdown ? QString() : resultMarkdown;
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
QString ScriptingService::callEncryptionHookForObject(QObject *object, const QString &text,
                                                      const QString &password, bool decrypt) {
    if (methodExistsForObject(object,
                              QStringLiteral("encryptionHook(QVariant,QVariant,QVariant)"))) {
        QVariant result;
        QMetaObject::invokeMethod(object, "encryptionHook", Q_RETURN_ARG(QVariant, result),
                                  Q_ARG(QVariant, text), Q_ARG(QVariant, password),
                                  Q_ARG(QVariant, decrypt));
        return result.toString();
    }

    return {};
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
QString ScriptingService::callEncryptionHook(const QString &text, const QString &password,
                                             bool decrypt) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString result =
            callEncryptionHookForObject(scriptComponent.object, text, password, decrypt);
        if (!result.isEmpty()) {
            return result;
        }
    }

    return {};
}

/**
 * Calls the noteDoubleClickedHook function for all script components
 *
 * Returns true if hook was found
 */
bool ScriptingService::callHandleNoteDoubleClickedHook(Note *note) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    bool hookFound = false;

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QObject *object = scriptComponent.object;

        if (methodExistsForObject(object, QStringLiteral("noteDoubleClickedHook(QVariant)"))) {
            auto *noteApi = new NoteApi();
            noteApi->fetch(note->getId());
            hookFound = true;

            QMetaObject::invokeMethod(
                object, "noteDoubleClickedHook",
                Q_ARG(QVariant, QVariant::fromValue(static_cast<QObject *>(noteApi))));
        }
    }

    return hookFound;
}

/**
 * Calls the websocketRawDataHook function for all script components
 *
 * This hook is called when data is sent from the QOwnNotes Web Companion
 * browser extension via the web browser's context menu
 *
 * @param requestType can be "page" or "selection"
 * @param pageUrl the url of the webpage where the request was made
 * @param pageTitle the page title of the webpage where the request was made
 * @param rawData the data that was transmitted, html for requestType "page" or plain text for
 * requestType "selection"
 * @param screenshotDataUrl the data url of the screenshot of the webpage where the request was made
 * @return true if data was handled by a hook
 */
bool ScriptingService::callHandleWebsocketRawDataHook(const QString &requestType,
                                                      const QString &pageUrl,
                                                      const QString &pageTitle,
                                                      const QString &rawData,
                                                      const QString &screenshotDataUrl) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QObject *object = scriptComponent.object;

        if (methodExistsForObject(
                object, QStringLiteral("websocketRawDataHook(QVariant,QVariant,QVariant,QVariant,"
                                       "QVariant)"))) {
            QVariant result;

            QMetaObject::invokeMethod(object, "websocketRawDataHook",
                                      Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, requestType),
                                      Q_ARG(QVariant, pageUrl), Q_ARG(QVariant, pageTitle),
                                      Q_ARG(QVariant, rawData), Q_ARG(QVariant, screenshotDataUrl));

            // if data was handled by hook return true
            if (result.toBool()) {
                return true;
            }
        }
    }

    return false;
}

/**
 * QML wrapper to start a detached process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function
 * (optional)
 * @param callbackParameter an additional parameter for loops or the like (optional)
 * @param processData data written to the process if the callback is used (optional)
 * @param workingDirectory the working directory to execute the process in (optional, only works
 * without callback)
 * @return true on success, false otherwise
 */
bool ScriptingService::startDetachedProcess(const QString &executablePath,
                                            const QStringList &parameters,
                                            const QString &callbackIdentifier,
                                            const QVariant &callbackParameter,
                                            const QByteArray &processData,
                                            const QString &workingDirectory) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    // callback provided: create new script thread
    if (!callbackIdentifier.isEmpty()) {
        TerminalCmd cmd;
        cmd.executablePath = executablePath;
        cmd.parameters = parameters;
        cmd.data = processData;

        auto *st = new ScriptThread(this, cmd, callbackIdentifier, callbackParameter);
        st->start();

        return true;
    }

    return Utils::Misc::startDetachedProcess(executablePath, parameters, workingDirectory);
}

/**
 * QML wrapper to start a synchronous process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @param workingDirectory the working directory to execute the process in (optional)
 * @return the text that was returned by the process
 */
QByteArray ScriptingService::startSynchronousProcess(const QString &executablePath,
                                                     const QStringList &parameters,
                                                     const QByteArray &data,
                                                     const QString &workingDirectory) const {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    return Utils::Misc::startSynchronousProcess(executablePath, parameters, data, workingDirectory);
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
    _currentNote = note;
    _currentNoteApi = new NoteApi();
    _currentNoteApi->fetch(note->getId());
}

/**
 * Calls the customActionInvoked function in all scripts
 */
void ScriptingService::onCustomActionInvoked(const QString &identifier) {
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
                                                        const QString &identifier) {
    if (methodExistsForObject(object, QStringLiteral("customActionInvoked(QVariant)"))) {
        QMetaObject::invokeMethod(object, "customActionInvoked", Q_ARG(QVariant, identifier));
    }
}

/**
 * QML wrapper to get the current note
 *
 * @return {NoteApi} the current note
 */
NoteApi *ScriptingService::currentNote() const {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    return _currentNoteApi;
}

/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(const QString &text) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr && mainWindow->doNoteEditingCheck()) {
        mainWindow->writeToNoteTextEdit(text);
    }
#else
    Q_UNUSED(text)
#endif
}

/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    return mainWindow != nullptr ? mainWindow->selectedNoteTextEditText() : QString();
#else
    return QString();
#endif
}

/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        mainWindow->activeNoteTextEdit()->selectAll();
    }
#endif
}

/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();
        c.movePosition(QTextCursor::StartOfBlock);
        c.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(c);
    }
#endif
}

/**
 * Select the current word in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();
        c.select(QTextCursor::WordUnderCursor);
        textEdit->setTextCursor(c);
    }
#endif
}

/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();

        start = std::max<int>(start, 0);
        end = std::min<int>(end, textEdit->toPlainText().count());

        c.setPosition(start);
        c.setPosition(end, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(c);
    }
#else
    Q_UNUSED(start)
    Q_UNUSED(end)
#endif
}

/**
 * Sets the text cursor in the note text edit to a certain position
 * 0 would be the beginning of the note
 * special case: -1 would be the end of the note
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
        position = std::min<int>(position, textEdit->toPlainText().count());
        QTextCursor c = textEdit->textCursor();

        if (position < 0) {
            c.movePosition(QTextCursor::End);
        } else {
            c.setPosition(position);
        }

        textEdit->setTextCursor(c);
    }
#else
    Q_UNUSED(position)
#endif
}

/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();

        return c.position();
    }
#endif

    return 0;
}

/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
        return textEdit->textCursor().selectionStart();
    }
#endif

    return 0;
}

/**
 * Returns the end position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionEnd() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QOwnNotesMarkdownTextEdit *textEdit = mainWindow->activeNoteTextEdit();
        return textEdit->textCursor().selectionEnd();
    }
#endif

    return 0;
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
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    return mainWindow != nullptr
               ? mainWindow->activeNoteTextEdit()->currentWord(withPreviousCharacters)
               : QString();
#else
    Q_UNUSED(withPreviousCharacters)
    return QString();
#endif
}

/**
 * Reads the current block in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    return mainWindow != nullptr ? mainWindow->activeNoteTextEdit()->currentBlock() : QString();
#else
    return {};
#endif
}

/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(const QString &tagName) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        mainWindow->linkTagNameToCurrentNote(tagName);
    }
#else
    Q_UNUSED(tagName)
#endif
}

/**
 * QML wrapper to log to the log widget
 *
 * @param text
 */
void ScriptingService::log(QString text) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        emit mainWindow->log(LogWidget::ScriptingLogType, std::move(text));
    }
#else
    Q_UNUSED(text)
#endif
}

/**
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString ScriptingService::downloadUrlToString(const QUrl &url) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    return Utils::Misc::downloadUrl(url);
}

/**
 * QML wrapper to use the AI Completer
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString &prompt) {
#ifndef INTEGRATION_TESTS
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        mainWindow->enableOpenAiActivitySpinner();
    }

    const auto result = OpenAiService::instance()->complete(prompt);

    if (mainWindow != nullptr) {
        mainWindow->enableOpenAiActivitySpinner(false);
    }

    return result;
#else
    Q_UNUSED(prompt)
    return {};
#endif
}

/**
 * QML wrapper to download an url to the media folder and returning the media
 * url or the Markdown image text of the media relative to the current note
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly if true only the media url will be returned
 * (default false)
 * @return {QString} the media Markdown or url
 */
QString ScriptingService::downloadUrlToMedia(const QUrl &url, bool returnUrlOnly) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    return _currentNote->downloadUrlToMedia(url, returnUrlOnly);
}

/**
 * QML wrapper to insert a media file into the media folder and returning
 * the media url or the Markdown image text of the media relative to the current
 * note
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly if true only the media url will be returned
 * (default false)
 * @return {QString} the media Markdown or url
 */
QString ScriptingService::insertMediaFile(const QString &mediaFilePath, bool returnUrlOnly) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    auto *mediaFile = new QFile(mediaFilePath);

    if (!mediaFile->exists()) {
        return {};
    }

    return _currentNote->getInsertMediaMarkdown(mediaFile, true, returnUrlOnly);
}

/**
 * QML wrapper to insert an attachment file into the "attachments" folder and
 * returning the attachment url or the Markdown text of the attachment
 * relative to the current note
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName to use in the Markdown
 * @param {bool} returnUrlOnly if true only the attachment url will be returned
 * (default false)
 * @return {QString} the attachment markdown or url
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName, bool returnUrlOnly) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    auto *attachmentFile = new QFile(attachmentFilePath);

    if (!attachmentFile->exists()) {
        return {};
    }

    return _currentNote->getInsertAttachmentMarkdown(attachmentFile, fileName, returnUrlOnly);
}

/**
 * Regenerates the note preview
 */
void ScriptingService::regenerateNotePreview() const {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                       QString(__func__));

        // Also regenerate the note preview if the note wasn't changed to e.g. reload images
        mainWindow->forceRegenerateNotePreview();
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
void ScriptingService::registerCustomAction(const QString &identifier, const QString &menuText,
                                            const QString &buttonText, const QString &icon,
                                            bool useInNoteEditContextMenu, bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                       QString(__func__));

        mainWindow->addCustomAction(identifier, menuText, buttonText, icon,
                                    useInNoteEditContextMenu, hideButtonInToolbar,
                                    useInNoteListContextMenu);
    }
#else
    Q_UNUSED(identifier)
    Q_UNUSED(menuText)
    Q_UNUSED(buttonText)
    Q_UNUSED(icon)
    Q_UNUSED(useInNoteEditContextMenu)
    Q_UNUSED(hideButtonInToolbar)
    Q_UNUSED(useInNoteListContextMenu)
#endif
}

/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(const QString &identifier, const QString &text) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                       QString(__func__));

        mainWindow->addScriptingLabel(identifier, text);
    }
#else
    Q_UNUSED(identifier)
    Q_UNUSED(text)
#endif
}

/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(const QString &identifier, const QString &text) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                       QString(__func__));

        mainWindow->setScriptingLabelText(identifier, text);
    }
#else
    Q_UNUSED(identifier)
    Q_UNUSED(text)
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

    if (mainWindow != nullptr) {
        MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                       QString(__func__));

        // create a temporary name for the note
        QString name = QStringLiteral("Note ") % Utils::Misc::createUuidString();

        // create the new note and move the cursor to the end
        mainWindow->createNewNote(
            name, std::move(text),
            MainWindow::CreateNewNoteOptions(MainWindow::CreateNewNoteOption::CursorAtEnd));
    }
#else
    Q_UNUSED(text)
#endif
}

/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);
    return asHtml ? mimeData->html() : mimeData->text();
}

/**
 * Disables the note encryption password dialog
 */
void ScriptingService::encryptionDisablePassword() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    qApp->setProperty("encryptionPasswordDisabled", true);
}

/**
 * Returns true if the current platform is Linux
 */
bool ScriptingService::platformIsLinux() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

/**
 * Returns true if the current platform is macOS
 */
bool ScriptingService::platformIsOSX() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

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
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

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
void ScriptingService::addStyleSheet(const QString &stylesheet) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    qApp->setStyleSheet(qApp->styleSheet() % QStringLiteral("\n/* BEGIN CUSTOM STYLESHEET */\n") %
                        stylesheet % QStringLiteral("\n/* END CUSTOM STYLESHEET */"));
}

/**
 * Clears all custom stylesheets
 */
void ScriptingService::clearCustomStyleSheets() {
    QRegularExpression regExp(
        QRegularExpression::escape(QStringLiteral("/* BEGIN CUSTOM STYLESHEET */")) % ".+" +
            QRegularExpression::escape(QStringLiteral("/* END CUSTOM STYLESHEET */")),
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
    qApp->setStyleSheet(qApp->styleSheet().remove(regExp));
}

/**
 * Fetches a note by its file name
 *
 * @param fileName string the file name of the note (mandatory)
 * @param noteSubFolderId integer id of the note subfolder
 * @return NoteApi*
 */
NoteApi *ScriptingService::fetchNoteByFileName(const QString &fileName, int noteSubFolderId) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    return NoteApi::fromNote(Note::fetchByFileName(fileName, noteSubFolderId));
}

/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi *ScriptingService::fetchNoteById(int id) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    auto *note = new NoteApi();
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
bool ScriptingService::noteExistsByFileName(const QString &fileName, int ignoreNoteId,
                                            int noteSubFolderId) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    Note note = Note::fetchByFileName(fileName, noteSubFolderId);

    // check if the note id should be ignored
    if ((ignoreNoteId > 0) && (note.getId() == ignoreNoteId)) {
        return false;
    }

    return note.isFetched();
}

/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(const QString &text, bool asHtml) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    QClipboard *clipboard = QApplication::clipboard();

    if (asHtml) {
        auto *mimeData = new QMimeData();
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
 * @param asTab bool if true the note will be opened in a new tab if not already open
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        if (asTab) {
            mainWindow->openNoteInTab(Note::fetch(note->getId()));
        } else {
            mainWindow->setCurrentNoteFromNoteId(note->getId());
        }
    }
#else
    Q_UNUSED(note)
    Q_UNUSED(asTab)
#endif
}

/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(const QString &text, const QString &title) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        QMessageBox::information(mainWindow, title, text);
    }
#else
    Q_UNUSED(text)
    Q_UNUSED(title)
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
int ScriptingService::questionMessageBox(const QString &text, const QString &title, int buttons,
                                         int defaultButton) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        return QMessageBox::question(mainWindow, title, text, QMessageBox::StandardButtons(buttons),
                                     QMessageBox::StandardButton(defaultButton));
    }
#else
    Q_UNUSED(text)
    Q_UNUSED(title)
    Q_UNUSED(buttons)
    Q_UNUSED(defaultButton)
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
QString ScriptingService::getOpenFileName(const QString &caption, const QString &dir,
                                          const QString &filter) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        return QFileDialog::getOpenFileName(mainWindow, caption, dir, filter);
    }
#else
    Q_UNUSED(caption)
    Q_UNUSED(dir)
    Q_UNUSED(filter)
#endif

    return {};
}

/**
 * Shows an save file dialog
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(const QString &caption, const QString &dir,
                                          const QString &filter) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        return QFileDialog::getSaveFileName(mainWindow, caption, dir, filter);
    }
#else
    Q_UNUSED(caption)
    Q_UNUSED(dir)
    Q_UNUSED(filter)
#endif

    return {};
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
QString ScriptingService::toNativeDirSeparators(const QString &path) {
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
QString ScriptingService::fromNativeDirSeparators(const QString &path) {
    return QDir::fromNativeSeparators(path);
}

/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator() { return QDir::separator(); }

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
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        Q_FOREACH (Note note, mainWindow->selectedNotes()) {
            selectedNotePaths << QDir::toNativeSeparators(note.fullNoteFilePath());
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
QList<int> ScriptingService::selectedNotesIds() const {
    QList<int> selectedNotesIds;
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != nullptr) {
        Q_FOREACH (Note note, mainWindow->selectedNotes()) {
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
QVector<int> ScriptingService::fetchNoteIdsByNoteTextPart(const QString &text) const {
    QVector<int> noteIds = Note::fetchAllIdsByNoteTextPart(text);
    return noteIds;
}

/**
 * Opens an input dialog with a select box
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param items {QStringList} list of items to select
 * @param current {int} index of the item that should be selected (default: 0)
 * @param editable {bool} if true the text in the dialog can be edited (default:
 * false)
 * @return {QString} text of the selected item
 */
QString ScriptingService::inputDialogGetItem(const QString &title, const QString &label,
                                             const QStringList &items, int current, bool editable) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    bool ok;
    QString result = QInputDialog::getItem(nullptr, title, label, items, current, editable, &ok);

    return ok ? result : QLatin1String("");
#else
    Q_UNUSED(title)
    Q_UNUSED(label)
    Q_UNUSED(items)
    Q_UNUSED(current)
    Q_UNUSED(editable)
    return QString();
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
QString ScriptingService::inputDialogGetText(const QString &title, const QString &label,
                                             const QString &text) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    bool ok;
    QString result = QInputDialog::getText(nullptr, title, label, QLineEdit::Normal, text, &ok);

    return ok ? result : QLatin1String("");
#else
    Q_UNUSED(title)
    Q_UNUSED(label)
    Q_UNUSED(text)
    return QString();
#endif
}

/**
 * Opens an input dialog with a multi-line text edit
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetMultiLineText(const QString &title, const QString &label,
                                                      const QString &text) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    bool ok;
    QString result = QInputDialog::getMultiLineText(nullptr, title, label, text, &ok);

    return ok ? result : QLatin1String("");
#else
    Q_UNUSED(title)
    Q_UNUSED(label)
    Q_UNUSED(text)
    return QString();
#endif
}

/**
 * Opens a dialog to show the differences between two texts and lets the user edit the result
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text1 {QString} first text
 * @param text2 {QString} second text
 * @return
 */
QString ScriptingService::textDiffDialog(const QString &title, const QString &label, QString text1,
                                         QString text2) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    if (text1.isNull()) {
        text1 = QLatin1String("");
    }

    if (text2.isNull()) {
        text2 = QLatin1String("");
    }

#ifndef INTEGRATION_TESTS
    auto dialog = new TextDiffDialog(nullptr, title, label, text1, text2);
    dialog->exec();
    auto accepted = dialog->resultAccepted();
    auto text = dialog->resultText();

    return accepted ? text : QLatin1String("");
#else
    Q_UNUSED(title)
    Q_UNUSED(label)
    return QString();
#endif
}

/**
 * Stores a persistent variable
 * These variables are accessible globally over all scripts
 * Please use a meaningful prefix in your key like
 * "PersistentVariablesTest/myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key, const QVariant &value) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    SettingsService settings;
    settings.setValue(
        QStringLiteral(PERSISTENT_VARIABLE_SETTINGS_PREFIX) % QStringLiteral("/") % key, value);
}

/**
 * Loads a persistent variable
 * These variables are accessible globally over all scripts
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist
 * (optional)
 * @return
 */
QVariant ScriptingService::getPersistentVariable(const QString &key, const QVariant &defaultValue) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    SettingsService settings;
    return settings.value(
        QStringLiteral(PERSISTENT_VARIABLE_SETTINGS_PREFIX) % QStringLiteral("/") % key,
        defaultValue);
}

/**
 * Loads an application settings variable
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist
 * (optional)
 * @return
 */
QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                          const QVariant &defaultValue) {
    SettingsService settings;
    return settings.value(key, defaultValue);
}

/**
 * Jumps to a note subfolder
 *
 * @param noteSubFolderPath {QString} path of the subfolder, relative to the
 * note folder
 * @param separator {QString} separator between parts of the path, default "/"
 * @return true if jump was successful
 */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                           const QString &separator) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return false;
    }

    NoteSubFolder folder = NoteSubFolder::fetchByPathData(noteSubFolderPath, separator);

    if (!folder.isFetched()) {
        return false;
    }

    // jump to the note subfolder
    return mainWindow->jumpToNoteSubFolder(folder.getId());
#else
    Q_UNUSED(noteSubFolderPath)
    Q_UNUSED(separator)
    return false;
#endif
}

/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(const QString &name) const {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    QStringList tags = Tag::searchAllNamesByName(name);
    return tags;
}

/**
 * Fetches or creates a tag by its "breadcrumb list" of tag names
 * Element nameList[0] would be highest in the tree (with parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} if true (default) all missing tags will be created
 * @return TagApi object of deepest tag of the name breadcrumb list
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(const QStringList &nameList,
                                                     bool createMissing) const {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

    Tag tag = Tag::getTagByNameBreadcrumbList(nameList, createMissing);
    auto *tagApi = TagApi::fromTag(tag);

    return tagApi;
}

/**
 * Writes a text to a file
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} optional (default: false)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data,
                                   const bool createParentDirs) const {
    if (filePath.isEmpty()) return false;

    QFile file(filePath);

    if (createParentDirs) {
        QFileInfo fileInfo(file);
        QDir dir = fileInfo.dir();
        if (!dir.mkpath(dir.path())) return false;
    }

    if (!file.open(QFile::WriteOnly | QFile::Truncate)) return false;

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << data;
    file.close();
    return true;
}

/**
 * Read text from a file
 *
 * @param filePath
 * @return the file data or null if the file does not exist
 */
QString ScriptingService::readFromFile(const QString &filePath, const QString &codec) const {
    if (filePath.isEmpty()) {
        return {};
    }

    QFile file(filePath);

    if (!file.open(QFile::ReadOnly)) {
        return {};
    }

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (codec == "latin1") {
        in.setEncoding(QStringConverter::Latin1);
    }
#else
    in.setCodec(codec.toLatin1());
#endif
    QString data = in.readAll();
    file.close();

    return data;
}

/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(const QString &filePath) const {
    if (filePath.isEmpty()) {
        return false;
    }
    QFile file(filePath);
    return file.exists();
}

/**
 * Triggers a menu action
 *
 * @param objectName {QString} object name of the action to trigger
 * @param checked {QString} only trigger the action if checked-state is
 *                          different than this parameter (can be 0 or 1)
 */
void ScriptingService::triggerMenuAction(const QString &objectName, const QString &checked) const {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("scripting/") %
                                                   QString(__func__));

#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    QAction *action = mainWindow->findAction(objectName);

    // return if action wasn't found
    if (action == nullptr) {
        return;
    }

    // script wants to set a checked state
    if (!checked.isEmpty()) {
        // return if action is not checkable
        if (!action->isCheckable()) {
            return;
        }

        bool isChecked = checked == QStringLiteral("1");

        // return if action already had the desired checked state
        if (isChecked == action->isChecked()) {
            return;
        }
    }

    // trigger the action
    action->trigger();
#else
    Q_UNUSED(objectName)
    Q_UNUSED(checked)
#endif
}

/**
 * Called after a script thread is done
 *
 * @brief ScriptingService::onScriptThreadDone
 * @param thread
 */
void ScriptingService::onScriptThreadDone(ScriptThread *thread) {
    QMapIterator<int, ScriptComponent> i(_scriptComponents);
    TerminalCmd *cmd = thread->getTerminalCmd();
    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        QVariantList cmdList, threadList;
        cmdList << cmd->executablePath << cmd->parameters << cmd->exitCode;
        threadList << thread->getIndex() << thread->getThreadCounter();
        if (methodExistsForObject(
                scriptComponent.object,
                QStringLiteral("onDetachedProcessCallback(QVariant,QVariant,QVariant,QVariant)"))) {
            QMetaObject::invokeMethod(scriptComponent.object, "onDetachedProcessCallback",
                                      Q_ARG(QVariant, thread->getIdentifier()),
                                      Q_ARG(QVariant, cmd->resultSet),
                                      Q_ARG(QVariant, QVariant::fromValue(cmdList)),
                                      Q_ARG(QVariant, QVariant::fromValue(threadList)));
        }
    }
}

/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const {
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                       QStringLiteral("/scripts/");

    if (!subDir.isEmpty()) {
        cacheDir = QDir::toNativeSeparators(cacheDir + subDir);
    }

    QDir dir = QDir(cacheDir);
    if (!dir.exists()) {
        dir.mkpath(dir.path());
    }

    return dir.path();
}

/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear or nothing for the entire script cache folder
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const {
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                       QStringLiteral("/scripts/");

    if (!subDir.isEmpty()) {
        cacheDir = QDir::toNativeSeparators(cacheDir + subDir);
    }

    QDir dir = QDir(cacheDir);
    bool result = false;

    if (dir.exists()) {
        result = dir.removeRecursively();
        dir.mkpath(dir.path());    // recreate the folder
    }

    return result;
}

/**
 * Adds a highlighting rule to the syntax highlighter of the editor
 *
 * @param pattern {QString} the regular expression pattern to highlight
 * @param shouldContain {QString} a string that must be contained in the highlighted text for the
 * pattern to be parsed
 * @param state {int} the state of the syntax highlighter to use
 * @param capturingGroup {int} the capturing group for the pattern to use for highlighting (default:
 * 0)
 * @param maskedGroup {int} the capturing group for the pattern to use for masking (default: 0)
 */
void ScriptingService::addHighlightingRule(const QString &pattern, const QString &shouldContain,
                                           int state, int capturingGroup, int maskedGroup) {
    QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule rule(
        static_cast<MarkdownHighlighter::HighlighterState>(state));
    rule.pattern = QRegularExpression(pattern);
    rule.shouldContain = shouldContain;
    rule.capturingGroup = capturingGroup;
    rule.maskedGroup = maskedGroup;
    _highlightingRules.append(rule);
}

QVector<QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule>
ScriptingService::getHighlightingRules() {
    return _highlightingRules;
}
