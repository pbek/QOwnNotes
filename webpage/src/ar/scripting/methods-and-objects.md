# الدوال والكائنات التي يوفرها QOwnNotes

## بدء برنامج خارجي في الخلفية

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to start a detached process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function (optional)
 * @param callbackParameter an additional parameter for loops or the like (optional)
 * @param processData data written to the process if the callback is used (optional)
 * @param workingDirectory the working directory to execute the process in (optional, only works without callback)
 * @return true on success, false otherwise
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
                            QByteArray processData, QString workingDirectory);
```

### مثال

مثال بسيط:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

تنفيذ عمليات كثيرة:

```js
for (var i = 0; i < 100; i++) {
  var dur = Math.floor(Math.random() * 10) + 1;
  script.startDetachedProcess("sleep", [`${dur}s`], "my-callback", i);
}

function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread) {
  if (callbackIdentifier == "my-callback") {
    script.log(`#${thread[1]} i[${thread[0]}] t${cmd[1]}`);
  }
}
```

ربما تحب أن تلقي نظرة على الأمثلة [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) أو [callback.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) أو [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml).

وربما تحب أيضا أن تلقي نظرة على خطاف [onDetachedProcessCallback (عند رد عملية منفصلة)](hooks.html#ondetachedprocesscallback).

## بدء برنامج خارجي وانتظار ناتجه

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to start a synchronous process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @param workingDirectory the working directory to execute the process in (optional)
 * @return the text that was returned by the process
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### مثال

```js
var result = script.startSynchronousProcess(
  "/path/to/my/program",
  ["my parameter"],
  "data",
  "/path/to/execute/in",
);
```

ربما تحب أن تلقي نظرة على المثال [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml).

## الحصول على مسار مجلد الملاحظة الحالية

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
 */
QString currentNoteFolderPath();
```

### مثال

```js
var path = script.currentNoteFolderPath();
```

ربما تحب أن تلقي نظرة على المثال [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml).

## الحصول على الملاحظة الحالية

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to get the current note
 *
 * @returns {NoteApi} the current note object
 */
NoteApi currentNote();
```

### مثال

```js
var note = script.currentNote();
```

ربما تحب أن تلقي نظرة على المثال [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## الكتابة في نافذة السجل

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to log to the log widget
 *
 * @param text
 */
void log(QString text);
```

### مثال

```js
script.log("رسالتي النصية");
```

## تنزيل رابط إلى سلسلة نصية

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString downloadUrlToString(QUrl url);
```

### مثال

```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

ربما تحب أن تلقي نظرة على المثال [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

## تنزيل رابط إلى مجلد الوسائط

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to download an url to the media folder and returning the media
 * url or the markdown image text of the media relative to the current note
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### مثال

```js
var markdown = script.downloadUrlToMedia(
  "http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}",
);
```

ربما تحب أن تلقي نظرة على المثال [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml).

## إضافة ملف وسائط إلى مجلد الوسائط

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to insert a media file into the media folder and returning
 * the media url or the markdown image text of the media  relative to the current note
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString ScriptingService::insertMediaFile(QString mediaFilePath,
                                        bool returnUrlOnly);
```

### مثال

```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

ربما تحب أن تلقي نظرة على المثال [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## إضافة ملف مرفق إلى مجلد المرفقات

### نداء الدالة ومُعامِلاتها

```cpp
 * QML wrapper to insert an attachment file into the `attachments` folder and
 * returning the attachment url or the markdown text of the attachment
 * relative to the current note
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName to use in the markdown
 * @param {bool} returnUrlOnly if true only the attachment url will be returned
 * (default false)
 * @return {QString} the attachment markdown or url
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### مثال

```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

## إعادة توليد معاينة الملاحظة

تنعش معاينة الملاحظة.

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * يعيد توليد معاينة الملاحظة
 */
QString ScriptingService::regenerateNotePreview();
```

### مثال

```js
script.regenerateNotePreview();
```

ربما تحب أن تلقي نظرة على المثال [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## تسجيل إجراء مخصص

### نداء الدالة ومُعامِلاتها

```cpp
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
                                            bool useInNoteListContextMenu);
```

::: tip
You can also assign local and global shortcuts to your custom actions in the _Shortcuts settings_.
:::

::: warning
تذكر أن [سمة أيقونات فري دسكتوب](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) متاحة غالبا فقط على لينكس. لذلك إذا أردت استخدام أيقونة في ماك أو ويندوز، عليك تزويد بُريمجك بواحدة. للحصول على مسار بُريمجك لضبط المسار المناسب لأيقونتك، يمكنك استخدام [خاصية scriptDirPath](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
:::

### مثال

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Initializes the custom actions
     */
    function init() {
        // add a custom action without a button
        script.registerCustomAction("mycustomaction1", "Menu text");

        // add a custom action with a button
        script.registerCustomAction("mycustomaction2", "Menu text", "Button text");

        // add a custom action with a button and freedesktop theme icon
        script.registerCustomAction("mycustomaction3", "Menu text", "Button text", "task-new");

        // add a custom action with a button and an icon from a file
        script.registerCustomAction("mycustomaction4", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     *
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "mycustomaction1":
                script.log("Action 1");
            break;
            case "mycustomaction2":
                script.log("Action 2");
            break;
            case "mycustomaction3":
                script.log("Action 3");
            break;
            case "mycustomaction4":
                script.log("Action 4");
            break;
        }
    }
}
```

للمزيد من بعض الأمثلة يمكنك أن تلقي نظرة على [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
يمكنك تنفيذ إجراء مخصص بعد تشغيل التطبيق، باستخدام المُعامِل <code dir="ltr">--action customAction_&lt;المعرِّف&gt;</code>. للمزيد من المعلومات يمكنك أن تلقي نظرة على [تنفيذ إجراءات قائمة بعد التشغيل](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

## Registering a label

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### مثال

```js
script.registerLabel(
  "html-label",
  "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.",
);

script.registerLabel(
  "long-label",
  "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap",
);

script.registerLabel("counter-label");
```

The labels will be visible in the _Scripting panel_, which you need to enable in the _Window / Panels_ menu.

You can use both plain text or html in the labels. The text will be selectable and links can be clicked.

ربما تحب أن تلقي نظرة على مثال البُريمج [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

## Setting the text of a registered label

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### مثال

```js
script.setLabelText("counter-label", "counter text");
```

You can use both plain text or html in the labels. The text will be selectable and links can be clicked.

ربما تحب أن تلقي نظرة على مثال البُريمج [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

::: tip
The scripting label also supports **external links**, as well as **internal links** to notes, tasks, deck cards and more. You might get a grasp of what's supported by taking a look at [URL handling](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

## إنشاء ملاحظة جديدة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### مثال

```js
script.createNote("My note headline\n===\n\nMy text");
```

ربما تحب أن تلقي نظرة على المثال [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterward, like this:

```js
var note = script.currentNote();
note.renameNoteFile("your-filename");
```

:::

## الوصول إلى الحافظة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * يرجع محتوى الحافظة كنص أو هتمل
 *
 * @param asHtml قيمة منطقية لإرجاع محتوى الحافظة كهتمل وليس كنص
 */
QString ScriptingService::clipboard(bool asHtml);
```

### مثال

```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

ربما تحب أن تلقي نظرة على المثال [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## كتابة نص إلى محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * تكتب النص المعطى عند موضع المؤشر الحالي في محرر نص الملاحظات
 *
 * @param text النص الذي سيُكتب
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### مثال

```js
// اكتب نصا في محرر نص الملاحظات
script.noteTextEditWrite("نصي المخصص");
```

ربما تحب أن تلقي نظرة على الإجراء المخصص `transformTextRot13` في المثال [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

ويمكنك استخدام هذه الدالة مع `noteTextEditSelectAll` لاستبدال كل نص الملاحظة الحالية.

## قراءة النص المحدد في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * ترجع النص المحدد حاليا في محرر نص الملاحظات
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### مثال

```js
// اقرأ النص المحدد حاليا في محرر نص الملاحظات
var text = script.noteTextEditSelectedText();
```

ربما تحب أن تلقي نظرة على الإجراء المخصص `transformTextRot13` في المثال [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## تحديد كل النص الذي في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * تحدد كل النص الذي في محرر نص الملاحظات
 */
void ScriptingService::noteTextEditSelectAll();
```

### مثال

```js
script.noteTextEditSelectAll();
```

يمكنك استخدمها مع `noteTextEditWrite` لتغيير كامل نص الملاحظة الحالية.

## تحديد نص السطر الحالي في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * تحدد السطر الحالي في محرر نص الملاحظات
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### مثال

```js
script.noteTextEditSelectCurrentLine();
```

## تحديد الكلمة الحالية في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * تحدد الكلمة الحالية في محرر نص الملاحظات
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### مثال

```js
script.noteTextEditSelectCurrentWord();
```

## تعيين التحديد الحالي في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### مثال

```js
// expands the current selection by one character
script.noteTextEditSetSelection(
  script.noteTextEditSelectionStart() - 1,
  script.noteTextEditSelectionEnd() + 1,
);
```

## الحصول على موضع بداية التحديد الحالي في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * ترجع موضع بداية النص المحدد حاليا في محرر نص الملاحظات
 */
int ScriptingService::noteTextEditSelectionStart();
```

### مثال

```js
script.log(script.noteTextEditSelectionStart());
```

## الحصول على موضع نهاية التحديد الحالي في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * ترجع موضع نهاية النص المحدد حاليا في محرر نص الملاحظات
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### مثال

```js
script.log(script.noteTextEditSelectionEnd());
```

## تعيين موضع مؤشر النص في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Sets the text cursor in the note text edit to a certain position
 * 0 would be the beginning of the note
 * special case: -1 would be the end of the note
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### مثال

```js
// انتقل إلى المحرف الحادي عشر في الملاحظة (الترقيم يبدأ من الصفر)
script.noteTextEditSetCursorPosition(10);

// انتقل إلى نهاية الملاحظة
script.noteTextEditSetCursorPosition(-1);
```

## الحصول على الموضع الحالي لمؤشر النص في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * ترجع الموضع الحالي لمؤشر النص في محرر نص الملاحظات
 * والصفر يدل على بداية الملاحظة
 */
int ScriptingService::noteTextEditCursorPosition();
```

### مثال

```js
script.log(script.noteTextEditCursorPosition());
```

## قراءة الكلمة الحالية في محرر نص الملاحظات

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Reads the current word in the note text edit
 *
 * @param withPreviousCharacters also get more characters at the beginning
 *                               to get characters like "@" that are not
 *                               word-characters
 * @return
 */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### مثال

```js
// اقرأ الكلمة الحالية في محرر نص الملاحظات
var text = script.noteTextEditCurrentWord();
```

ربما تحب أن تلقي نظرة على المثال [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Read the current block from the note text edit

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Reads the current block in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### مثال

```js
// Read the current block in the note text edit
var text = script.noteTextEditCurrentBlock();
```

ربما تحب أن تلقي نظرة على المثال [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Use a completion prompt on the currently selected AI model

The AI completion prompt is a feature that allows you to use a completion prompt on the currently selected AI model, like ChatGPT, Claude or systems like Ollama.

The AI system needs to be enabled in the AI toolbar or main menu for this to work.

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * QML wrapper to use the AI Completer
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### مثال

```js
// Ask the currently selected AI model to complete the prompt
var text = script.aiComplete("Tell me how do you feel today?");
```

ربما تحب أن تلقي نظرة على المثال [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## معرفة ما إذا كانت المنصة لينكس أم ماك أم ويندوز

### نداء الدالة ومُعامِلاتها

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### مثال

```js
if (script.platformIsLinux()) {
  // Will be executed only if under Linux
}
```

## وسم الملاحظة الحالية

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### مثال

```js
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

ربما تحب إلقاء نظرة على الإجراء المخصص `favoriteNote` في المثال [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

## Create or fetch a tag by its name breadcrumb list

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Fetches or creates a tag by its "breadcrumb list" of tag names
 * Element nameList[0] would be highest in the tree (with parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} if true (default) all missing tags will be created
 * @return TagApi object of deepest tag of the name breadcrumb list
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### مثال

```js
// creates all tags until the 3rd level and returns the tag object for
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

## ابحث عن وسوم بالاسم

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### مثال

```js
// searches for all tags with the word game in it
var tags = script.searchTagsByName("game");
```

ربما تحب أن تلقي نظرة على المثال [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## ابحث عن ملاحظات بنص الملاحظة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Returns a list of note ids of all notes with a certain text in the note text
 *
 * Unfortunately there is no easy way to use a QList<NoteApi*> in QML, so we
 * can only transfer the note ids
 *
 * @return {QList<int>} list of note ids
 */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);
```

### مثال

```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // do something with the note
});
```

ربما تحب أن تلقي نظرة على المثال [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml).

## Add a custom stylesheet

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### مثال

```js
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

ربما تحب أن تلقي نظرة على المثال [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml).

You can get the widget names from the `*.ui` files, for example the main window is [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui).

The Qt documentation (for example [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) can help you to see how the widgets are related to each other (search for `Inherits` on the pages).

The base widget for almost everything is [QWidget](https://doc.qt.io/qt-5/qwidget.html). So just styling `QWidget` with for example `QWidget {background-color: black; color: white;}` would mean everything has a black background color and a white foreground color.

::: tip
The [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) of [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) might also be a good reference for styles you can change.
:::

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
If you actually want to see how the dialogs look and what their names are you could download [Qt Creator](https://www.qt.io/product/development-tools) and open the `*.ui` files in it.
:::

## إعادة تحميل محرك البرمجة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * يعيد تحميل محرك البرمجة
 */
void ScriptingService::reloadScriptingEngine();
```

### مثال

```js
// أعد تحميل محرك البرمجة
script.reloadScriptingEngine();
```

## جلب ملاحظة باسم ملفها

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Fetches a note by its file name
 *
 * @param fileName string the file name of the note (mandatory)
 * @param noteSubFolderId integer id of the note subfolder
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                int noteSubFolderId);
```

### مثال

```js
// اجلب ملاحظة باسم ملفها
script.fetchNoteByFileName("ملاحظتي.md");
```

## جلب ملاحظة بمُعرِّفها

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### مثال

```js
// اجلب ملاحظة بمعرفها
script.fetchNoteById(243);
```

ربما تحب أن تلقي نظرة على المثال [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## التحقق من وجود ملاحظة باسم ملفها

### نداء الدالة ومُعامِلاتها

```cpp
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
                                            int noteSubFolderId);
```

### مثال

```js
// check if note exists, but ignore the id of "note"
script.noteExistsByFileName("my note.md", note.id);
```

ربما تحب أن تلقي نظرة على المثال [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## نسخ نص إلى الحافظة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### مثال

```js
// انسخ نصا إلى الحافظة
script.setClipboardText("نص النسخ");
```

ربما تحب أن تلقي نظرة على المثال [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml).

## الانتقال إلى ملاحظة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 * @param asTab bool if true the note will be opened in a new tab (if not already open)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### مثال

```js
// jump to the note
script.setCurrentNote(note);

// open note in new tab (if not already open)
script.setCurrentNote(note, true);
```

ربما تحب أن تلقي نظرة على المثال [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml).

## الانتقال إلى مجلد فرعي لملاحظة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Jumps to a note subfolder
 *
 * @param noteSubFolderPath {QString} path of the subfolder, relative to the note folder
 * @param separator {QString} separator between parts of the path, default "/"
 * @return true if jump was successful
 */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### مثال

```js
// jump to the note subfolder "a sub folder"
script.jumpToNoteSubFolder("a sub folder");

// jump to the note subfolder "sub" inside of "a sub folder"
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: tip
يمكنك إنشاء مجلد ملاحظات فرعي في المجلد الفرعي الحالي بنداء [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

## إظهار صندوق رسالة معلومة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### مثال

```js
// show a information message box
script.informationMessageBox("The text I want to show", "Some optional title");
```

## إظهار صندوق رسالة سؤال

### نداء الدالة ومُعامِلاتها

```cpp
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
        QString text, QString title, int buttons, int defaultButton);
```

### مثال

```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
  "The text I want to show",
  "Some optional title",
  0x01000000 | 0x02000000,
  0x02000000,
);
script.log(result);
```

للمزيد من المعلومات عن الأزرار، انظر [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

ربما تحب أيضا أن تلقي نظرة على المثال [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## إظهار حوار فتح ملف

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Shows an open file dialog
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getOpenFileName(QString caption, QString dir,
                                            QString filter);
```

### مثال

```js
// show an open file dialog
var fileName = script.getOpenFileName(
  "Please select an image",
  "/home/user/images",
  "Images (*.png *.xpm *.jpg)",
);
```

## إظهار حوار حفظ ملف

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Shows a save file dialog
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                            QString filter);
```

### مثال

```js
// show a save file dialog
var fileName = script.getSaveFileName(
  "Please select HTML file to save",
  "output.html",
  "HTML (*.html)",
);
```

ربما تحب أن تلقي نظرة على المثال [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## تسجيل متغيرات إعدادات البُريمج

عليك تعريف متغيرات إعداداتك على أنها خصائص في بُريمِجك وتسجيلها في خاصية تسمى `settingsVariables`.

عندئذٍ يستطيع المستخدم ضبط هذه الخصائص في إعدادات البُريمج.

### مثال

```js
// you have to define your registered variables so you can access them later
property string myString;
property string myStringSecret;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string myDirectory;
property string mySelection;

// register your settings variables so the user can set them in the script settings
//
// unfortunately there is no QVariantHash in Qt, we only can use
// QVariantMap (that has no arbitrary ordering) or QVariantList (which at
// least can be ordered arbitrarily)
property variant settingsVariables: [
    {
        "identifier": "myString",
        "name": "I am a line edit",
        "description": "Please enter a valid string:",
        "type": "string",
        "default": "My default value",
    },
    {
        "identifier": "myStringSecret",
        "name": "I am a password field",
        "description": "Please enter a valid string:",
        "type": "string-secret",
    },
    {
        "identifier": "myBoolean",
        "name": "I am a checkbox",
        "description": "Some description",
        "text": "Check this checkbox",
        "type": "boolean",
        "default": true,
    },
    {
        "identifier": "myText",
        "name": "I am textbox",
        "description": "Please enter your text:",
        "type": "text",
        "default": "This can be a really long text\nwith multiple lines.",
    },
    {
        "identifier": "myInt",
        "name": "I am a number selector",
        "description": "Please enter a number:",
        "type": "integer",
        "default": 42,
    },
    {
        "identifier": "myFile",
        "name": "I am a file selector",
        "description": "Please select the file:",
        "type": "file",
        "default": "pandoc",
    },
    {
        "identifier": "myDirectory",
        "name": "I am a directory selector",
        "description": "Please select the directory:",
        "type": "directory",
        "default": "/home",
    },
    {
        "identifier": "mySelection",
        "name": "I am an item selector",
        "description": "Please select an item:",
        "type": "selection",
        "default": "option2",
        "items": {"option1": "Text for option 1", "option2": "Text for option 2", "option3": "Text for option 3"},
    }
];
```

ويمكنك أيضا تغطية `settingsVariables` بالدالة الخاصة `registerSettingsVariables()` مثل:

### مثال

```js
/**
 * Registers the settings variables again
 *
 * Use this method if you want to use code to override your variables, like setting
 * default values depending on the operating system.
 */
function registerSettingsVariables() {
  if (script.platformIsWindows()) {
    // override the myFile default value
    settingsVariables[3].default = "pandoc.exe";
  }
}
```

ربما تحب أيضا أن تلقي نظرة على المثال [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

## تخزين وقراءة متغيرات مستديمة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Stores a persistent variable
 * These variables are accessible globally over all scripts
 * Please use a meaningful prefix in your key like "PersistentVariablesTest/myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                             const QVariant &value);

/**
 * Loads a persistent variable
 * These variables are accessible globally over all scripts
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                 const QVariant &defaultValue);
```

### مثال

```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(
  script.getPersistentVariable(
    "PersistentVariablesTest/myVar",
    "nothing here yet",
  ),
);
```

أسبِق أسماء متغيراتك المستديمة بعبارة مميزة، مثل: `PersistentVariablesTest/myVar` لأن جميع البُريمجات تستطيع الوصول إلى جميع المتغيرات المستديمة.

ربما تحب أيضا أن تلقي نظرة على المثال [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

## تحميل متغيرات إعدادات التطبيق

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Loads an application settings variable
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                          const QVariant &defaultValue);
```

### مثال

```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

تذكر أن الإعدادات يمكنها أن تكون فارغة؛ عليك التعامل مع هذا الأمر بنفسك. تُستخدم `defaultValue` فقط عندما لا يكون الإعداد موجودا من الأساس.

## Creating a cache directory

You can cache files at the default cache location of your system.

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### مثال

```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

## Clearing a cache directory

You can clear the cache directory of your script by passing its name to clearCacheDir().

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### مثال

```js
// clear cache directory of my-script-id
script.clearCacheDir("my-script-id");
```

## قراءة مسار مجلد بُريمجك

إذا احتجت إلى الحصول على مسار المجلد الذي يقع بُريمِجك فيه، لتحميل ملفات أخرى مثلا، عليك تسجيل الخاصية النصية `scriptDirPath` (بالكود <code dir="ltr">property string scriptDirPath;</code>). ستُضبط هذه الخاصية إلى مسار مجلد بُريمِجك.

### مثال

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    // the path to the script's directory will be set here
    property string scriptDirPath;

    function init() {
        script.log(scriptDirPath);
    }
}
```

## تحويل فواصل المسار إلى الفواصل الخاصة بالنظام

### نداء الدالة ومُعامِلاتها

```cpp
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
QString ScriptingService::toNativeDirSeparators(QString path);
```

### مثال

```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

## تحويل فواصل المسار من الفواصل الخاصة بالنظام

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Returns path using '/' as file separator.
 * On Windows, for instance, fromNativeDirSeparators("c:\\winnt\\system32")
 * returns "c:/winnt/system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::fromNativeDirSeparators(QString path);
```

### مثال

```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

## جلب فاصل المسارات الخاص بالنظام

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### مثال

```js
// will return "\" on Windows
script.log(script.dirSeparator());
```

## جلب قائمة بمسارات الملاحظات المحددة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Returns a list of the paths of all selected notes
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### مثال

```js
// ستُرجع قائمة بمسارات الملاحظات المحددة جميعها
script.log(script.selectedNotesPaths());
```

ربما تحب أن تلقي نظرة على المثال [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml).

## جلب قائمة بمُعرِّفات الملاحظات المحددة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Returns a list of the ids of all selected notes
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### مثال

```js
// ستُرجع قائمة بمعرفات الملاحظات المحددة جميعها
script.log(script.selectedNotesIds());
```

ربما تحب أن تلقي نظرة على المثال [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## تفعيل إجراء قائمة

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Triggers a menu action
 *
 * @param objectName {QString} object name of the action to trigger
 * @param checked {QString} only trigger the action if checked-state is
 *                          different than this parameter (optional, can be 0 or 1)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### مثال

```js
// تبديل وضع القراءة فقط
script.triggerMenuAction("actionAllow_note_editing");

// تعطيل وضع القراءة فقط
script.triggerMenuAction("actionAllow_note_editing", 1);
```

ربما تحب أن تلقي نظرة على المثال [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
يمكنك الحصول على أسماء كائنات إجراءات القائمة من [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). فقط ابحث بالعنوان الإنجليزي للقائمة. لاحظ أن هذه النصوص قد تتغير بمرور الوقت.
:::

## إظهار حوار إدخال بصندوق اختيار

### نداء الدالة ومُعامِلاتها

```cpp
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
        int current, bool editable);
```

سترجع سلسلة نصية فارغة عند نقر `Cancel` أو ضغط `Escape`.

### مثال

```js
var result = script.inputDialogGetItem("combo box", "Please select an item", [
  "Item 1",
  "Item 2",
  "Item 3",
]);
script.log(result);
```

ربما تحب أن تلقي نظرة على المثال [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## إظهار حوار لإدخال سطر نصي

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Opens an input dialog with a line edit
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text);
```

سترجع سلسلة نصية فارغة عند نقر `Cancel` أو ضغط `Escape`.

### مثال

```js
var result = script.inputDialogGetText(
  "line edit",
  "Please enter a name",
  "current text",
);
script.log(result);
```

## إظهار حوار لإدخال سطور نصية

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Opens an input dialog with a multi-line text edit
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetMultiLineText(
        const QString &title, const QString &label, const QString &text);
```

سترجع سلسلة نصية فارغة عند نقر `Cancel` أو ضغط `Escape`.

### مثال

```js
var result = script.inputDialogGetMultiLineText(
  "multi-line edit",
  "Please enter a text",
  "current text",
);
script.log(result);
```

## فتح مربع حوار لإظهار الاختلافات بين نصين

### نداء الدالة ومُعامِلاتها

```cpp
/**
* Opens a dialog to show the differences between two texts and lets the user edit the result
*
* @param title {QString} title of the dialog
* @param label {QString} label text of the dialog
* @param text1 {QString} first text
* @param text2 {QString} second text
* @return
  */
  QString ScriptingService::textDiffDialog(const QString &title, const QString &label,
                                           const QString &text1, const QString &text2);
```

و`text2` هو النص الذي سيظهر في مربع الحوار لتحريره. سترجع سلسلة نصية فارغة عند نقر `Cancel` أو ضغط `Escape`.

### مثال

```js
const text = script.noteTextEditSelectedText();
const aiPrompt = "Translate the text to English";
const aiResult = script.aiComplete(aiPrompt + ":\n\n" + text);

var result = script.textDiffDialog(
  "AI Text Tool",
  "Resulting text",
  text,
  aiResult,
);
script.log(result);
```

## التحقق من وجود ملف

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * تتحقق من وجود ملف
 * @param filePath مسار الملف
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### مثال

```js
var result = script.fileExists(filePath);
script.log(result);
```

## قراءة نص من ملف

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Read text from a file
 *
 * @param filePath {QString} path of the file to load
 * @param codec {QString} file encoding (default: UTF-8)
 * @return the file data or null if the file does not exist
 */
QString ScriptingService::readFromFile(const QString &filePath, const QString &codec)
```

### مثال

```js
if (script.fileExists(filePath)) {
  var data = script.readFromFile(filePath);
  script.log(data);
}
```

## كتابة نص إلى ملف

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Writes a text to a file
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} optional (default: false)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### مثال

```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

ربما تحب أن تلقي نظرة على المثال [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## العمل مع مقابس الويب

يمكنك التحكم في QOwnNotes عن بعد باستخدام `WebSocketServer` (خادم مقبس الويب).

برجاء إلقاء نظرة على [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml). يمكنك اختبار خادم مقبس الويب بتوصيله على [اختبار Websocket](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

يمكنك أيضا الاستماع إلى المقابس باستخدام `WebSocket`. برجاء إلقاء نظرة على مثال [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml).

تذكر أنك بحاجة إلى أن تكون مكتبة `websocket` من Qt QML مثبتة على جهازك. مثلا على أوبنتو، يمكنك تثبيت حزمة `qml-module-qtwebsockets`.

## إضافة قاعدة تلوين إلى المحرر

يمكنك إضافة قواعد تلوين إلى المحرر مباشرةً، بتعريف تعابير نمطية وإسنادها إلى حالة تلوين.

### نداء الدالة ومُعامِلاتها

```cpp
/**
 * Adds a highlighting rule to the syntax highlighter of the editor
 *
 * @param pattern {QString} the regular expression pattern to highlight
 * @param shouldContain {QString} a string that must be contained in the highlighted text for the pattern to be parsed
 * @param state {int} the state of the syntax highlighter to use
 * @param capturingGroup {int} the capturing group for the pattern to use for highlighting (default: 0)
 * @param maskedGroup {int} the capturing group for the pattern to use for masking (default: 0)
 */
void ScriptingService::addHighlightingRule(const QString &pattern,
                                           const QString &shouldContain,
                                           int state,
                                           int capturingGroup,
                                           int maskedGroup);
```

### حالات التلوين

| الاسم                  | الرقم |
| ---------------------- | ----- |
| بلا حالة               | -1    |
| رابط                   | 0     |
| صورة                   | 3     |
| كتلة كود               | 4     |
| تعليق في كتلة كود      | 5     |
| مائل                   | 7     |
| عريض                   | 8     |
| قائمة                  | 9     |
| تعليق                  | 11    |
| H1                     | 12    |
| H2                     | 13    |
| H3                     | 14    |
| H4                     | 15    |
| H5                     | 16    |
| H6                     | 17    |
| كتلة اقتباس            | 18    |
| HorizontalRuler        | 21    |
| جدول                   | 22    |
| InlineCodeBlock        | 23    |
| MaskedSyntax           | 24    |
| لون خلفية السطر الحالي | 25    |
| رابط تالف              | 26    |
| كتلة تمهيد             | 27    |
| مسافة في نهاية السطر   | 28    |
| CheckBoxUnChecked      | 29    |
| CheckBoxChecked        | 30    |
| StUnderline            | 31    |

### مثال

```js
// Highlight a text line like "BLOCK: some text" as blockquote (state 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Mask out (state 24) all characters after 32 characters in a line
// capturingGroup 1 means the expression from the first bracketed part of the pattern will be highlighted
// maskedGroup -1 means that no masking should be done
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

ربما تحب أن تلقي نظرة على الأمثلة في [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).
