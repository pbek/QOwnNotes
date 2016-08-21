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
#include <dialogs/logdialog.h>
#include <QTimer>
#include <QVariant>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#ifndef INTEGRATION_TESTS
#include <mainwindow.h>
#endif

ScriptingService::ScriptingService(QObject *parent) : QObject(parent) {
    _engine = new QQmlEngine(this);
    _engine->rootContext()->setContextProperty("script", this);
//    _engine->rootContext()->setContextProperty(
//            "mainWindow", qApp->property("mainWindow").value<MainWindow *>());

    qmlRegisterType<NoteApi>("com.qownnotes.noteapi", 1, 0, "NoteApi");
    qmlRegisterType<TagApi>("com.qownnotes.tagapi", 1, 0, "TagApi");

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
        _scriptComponents[script.getId()] = scriptComponent;

        // call the init function if it exists
        if (methodExistsForObject(object, "init()")) {
            QMetaObject::invokeMethod(object, "init");
        }

//        outputMethodsOfObject(object);

        if (methodExistsForObject(object, "onNoteStored(QVariant)")) {
            QObject::connect(this, SIGNAL(noteStored(QVariant)),
                             object, SLOT(onNoteStored(QVariant)));
        }
    } else {
        qWarning() << "script errors: " << component->errors();
    }
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

    QHashIterator<int, ScriptComponent> i(_scriptComponents);

    // delete all objects and components
    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();
        delete(scriptComponent.object);
        delete(scriptComponent.component);
    }

    // clear the component cache
    _engine->clearComponentCache();

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
    _scriptComponents.clear();
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
    QHashIterator<int, ScriptComponent> i(_scriptComponents);

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
    QHashIterator<int, ScriptComponent> i(_scriptComponents);

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
    QHashIterator<int, ScriptComponent> i(_scriptComponents);

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
 * Calls the handleNewNoteHeadlineHook function for all script components
 */
QString ScriptingService::callHandleNewNoteHeadlineHook(QString headline) {
    QHashIterator<int, ScriptComponent> i(_scriptComponents);

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
    QHashIterator<int, ScriptComponent> i(_scriptComponents);

    while (i.hasNext()) {
        i.next();
        ScriptComponent scriptComponent = i.value();

        QString text = callNoteToMarkdownHtmlHookForObject(
                scriptComponent.object, note, html);
        if (!text.isEmpty()) {
            return text;
        }
    }

    return "";
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
    return Utils::Misc::startDetachedProcess(executablePath, parameters);
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
 */
void ScriptingService::onCurrentNoteChanged(Note *note) {
    _currentNoteAPi = new NoteApi();
    _currentNoteAPi->fetch(note->getId());
}

/**
 * Calls the customActionInvoked function in all scripts
 */
void ScriptingService::onCustomActionInvoked(QString identifier) {
    QHashIterator<int, ScriptComponent> i(_scriptComponents);

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
    return _currentNoteAPi;
}

/**
 * QML wrapper to log to the log dialog
 *
 * @param text
 */
void ScriptingService::log(QString text) {
    LogDialog::instance()->log(LogDialog::ScriptingLogType, text);
}


/**
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString ScriptingService::downloadUrlToString(QUrl url) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QNetworkRequest networkRequest = QNetworkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute,
                                true);
#endif

    QNetworkReply *reply = manager->get(networkRequest);
    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        // get the text from the network reply
        QString data = reply->readAll();
        if (data.size() > 0) {
            return data;
        }
    }

    // timer elapsed, no reply from network request or empty data
    return "";
}

/**
 * Registers a custom action
 *
 * @param identifier the identifier of the action
 * @param menuText the text shown in the menu
 * @param buttonText the text shown in the button
 *                   (no button will be viewed if empty)
 */
void ScriptingService::registerCustomAction(QString identifier,
                                            QString menuText,
                                            QString buttonText) {
#ifndef INTEGRATION_TESTS
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow != Q_NULLPTR) {
        mainWindow->addCustomAction(identifier, menuText, buttonText);
    }
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
        // create a temporary name for the note
        QDateTime currentDate = QDateTime::currentDateTime();
        QString name = "Note " +
                currentDate.toString(Qt::ISODate).replace(":", ".");

        // create the new note and move the cursor to the end
        mainWindow->createNewNote(name, text, true);
    }
#endif
}

/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml) {
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);
    return asHtml ? mimeData->html() : mimeData->text();
}
