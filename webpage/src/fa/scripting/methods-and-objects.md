# روش ها و اهدافی که QOwnNotes فراهم می کند

## شروع یک برنامه خارجی در پس زمینه

### فراخوانی شگرد و پارامترها

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

مثال ساده:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

اجرای فرآیند های متعدد:

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

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) or [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml).

همچنین می توانید هوک [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) را ملاحظه نمایید.

## آغاز یک برنامه خارجی و انتظار برای خروجی

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml).

## دستیابی به مسیر پوشه یادداشت کنونی

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml).

## دستیابی به یادداشت کنونی

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## ورود به ابزارک کارنگاری

### فراخوانی شگرد و پارامترها

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
script.log("my text");
```

## بارگیری URL به رشته

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

## بارگیری URL به پوشه رسانه

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml).

## درج پرونده رسانه در پوشه رسانه

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## درج پرونده پیوست در پوشه پیوست ها

### فراخوانی شگرد و پارامترها

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

## بازسازی پیش نمایش یادداشت

پیش نمایش یادداشت را نوسازی می کند.

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Regenerates the note preview
 */
QString ScriptingService::regenerateNotePreview();
```

### مثال

```js
script.regenerateNotePreview();
```

You may want to take a look at the example [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## ثبت فعالیت سفارشی

### فراخوانی شگرد و پارامترها

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
توجه داشته باشید که [آیکون های تم freedesktop](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) اغلب تنها در سیستم عامل لینوکس در دسترس قرار دارند. پس در صورتی که واقعاً تمایل به استفاده از آیکونی تحت سیستم عامل های مکینتاش یا ویندوز داشته باشید، باید یکی از آنها را در اسکیت خود قرار دهید. برای دستیابی به مسیر اسکریپت خود جهت تنظیم یک مسیر صحیح مربوط به آیکون، می توانید از [scriptDirPath property](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script) استفاده کنید.
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

For some more examples please see [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
شما همچنین می توانید عملیات سفارشی را پس از شروع به کار برنامه با پارامتر `--action customAction_<identifier>` راه‌اندازی کنید. برای کسب اطلاعات بیشتر لطفاً به [راه‌اندازی منو اکشن ها بعد از شروع به کار](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup) مراجعه نمایید.
:::

## ثبت برچسب

### فراخوانی شگرد و پارامترها

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

شما می توانید هم از متن ساده و هم html در برچسب ها استفاده کنید. متن قابل انتخاب بوده و می توان روی پیوندها کلیک کرد.

You may then want to take a look at the example script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

## تنظیم متن برچسب ثبت شده

### فراخوانی شگرد و پارامترها

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

شما می توانید هم از متن ساده و هم html در برچسب ها استفاده کنید. متن قابل انتخاب بوده و می توان روی پیوندها کلیک کرد.

You may then want to take a look at the example script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

::: tip
The scripting label also supports **external links**, as well as **internal links** to notes, tasks, deck cards and more. You might get a grasp of what's supported by taking a look at [URL handling](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

## ایجاد یادداشت جدید

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterward, like this:

```js
var note = script.currentNote();
note.renameNoteFile("your-filename");
```

:::

## دسترسی به تخته برش

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### مثال

```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## نوشتن متن برای ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### مثال

```js
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

شما می توانید از این به همراه `noteTextEditSelectAll` برای بازنویسی کل متن یادداشت کنونی استفاده کنید.

## خواندن متن منتخب در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### مثال

```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## خواندن کل متن در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll();
```

### مثال

```js
script.noteTextEditSelectAll();
```

شما می توانید از این به همراه `noteTextEditWrite` برای بازنویسی کل متن یادداشت کنونی استفاده کنید.

## انتخاب خط کنونی در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### مثال

```js
script.noteTextEditSelectCurrentLine();
```

## انتخاب کلمه کنونی در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Selects the current word in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### مثال

```js
script.noteTextEditSelectCurrentWord();
```

## تنظیم متن منتخب کنونی در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

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

## قرار گرفتن در موقعیت شروع انتخاب کنونی در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart();
```

### مثال

```js
script.log(script.noteTextEditSelectionStart());
```

## قرار گرفتن در موقعیت پایانی انتخاب کنونی در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Returns the end position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### مثال

```js
script.log(script.noteTextEditSelectionEnd());
```

## تنظیم نشانگر متن در ویرایش یادداشت کنونی نسبت به موقعیت معین

### فراخوانی شگرد و پارامترها

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
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

## قرارگیری موقعیت کنونی نشانگر متن در ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### مثال

```js
script.log(script.noteTextEditCursorPosition());
```

## خواندن کلمه کنونی از ویرایش متن یادداشت

### فراخوانی شگرد و پارامترها

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
// read the current word in the note text edit
var text = script.noteTextEditCurrentWord();
```

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Read the current block from the note text edit

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Use a completion prompt on the currently selected AI model

The AI completion prompt is a feature that allows you to use a completion prompt on the currently selected AI model, like ChatGPT, Claude or systems like Ollama.

The AI system needs to be enabled in the AI toolbar or main menu for this to work.

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## بررسی اینکه آیا پلتفرم لینوکس، سیستم عامل X یا ویندوز است

### فراخوانی شگرد و پارامترها

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

## برچسب گذاری یادداشت کنونی

### فراخوانی شگرد و پارامترها

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

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

## Create or fetch a tag by its name breadcrumb list

### فراخوانی شگرد و پارامترها

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

## جستجوی برچسب ها بر اساس نام

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## جستجوی یادداشت ها براساس متن یادداشت

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml).

## افزودن برگه سبک نگارش سفارشی

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml).

You can get the widget names from the `*.ui` files, for example the main window is [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui).

مستندات Qt (مانند [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) می تواند به شما کمک کند تا نحوه ارتباط ابزاره ها را با یکدیگر مشاهده کنید (`Inherits` را در صفحات جستجو کنید).

ابزاره پایه برای تقریباً همه چیز [QWidget](https://doc.qt.io/qt-5/qwidget.html) است. So just styling `QWidget` with for example `QWidget {background-color: black; color: white;}` would mean everything has a black background color and a white foreground color.

::: tip
The [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) of [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) might also be a good reference for styles you can change.
:::

برای ارجاع به سبک های موجود، نگاهی به [مرجع برگ سبک نگارش](http://doc.qt.io/qt-5/stylesheet-reference.html) بیندازید.

در صورت تمایل به وارد کردن سبک های نگارشی به پیش نمایش html برای تغییر روشی که یادداشت ها نمایش داده می شوند، لطفاً به [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook) مراجعه نمایید.

::: tip
If you actually want to see how the dialogs look and what their names are you could download [Qt Creator](https://www.qt.io/product/development-tools) and open the `*.ui` files in it.
:::

## بارگذاری مجدد موتور اسکریپت نویسی

### فراخوانی شگرد و پارامترها

```cpp
/**
 * Reloads the scripting engine
 */
void ScriptingService::reloadScriptingEngine();
```

### مثال

```js
// reload the scripting engine
script.reloadScriptingEngine();
```

## فراهم‌سازی یادداشت بر اساس نام پرونده آن

### فراخوانی شگرد و پارامترها

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
// fetch note by file name
script.fetchNoteByFileName("my note.md");
```

## فراهم سازی یادداشت بر اساس شناسه کاربری آن

### فراخوانی شگرد و پارامترها

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
// fetch note by id
script.fetchNoteById(243);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## بررسی امکان وجود یادداشت براساس نام پرونده آن

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## رونوشت برداری از متن به تخته برش

### فراخوانی شگرد و پارامترها

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
// copy text to the clipboard
script.setClipboardText("text to copy");
```

You may want to take a look at the example [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml).

## پرش به یادداشت

### فراخوانی شگرد و پارامترها

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

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml).

## پرش به زیر پوشه یادداشت

### فراخوانی شگرد و پارامترها

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
شما می توانید یک زیر پوشه یادداشت جدید در زیر پوشه یادداشت کنونی با فراخوانی [`mainWindow.createNewNoteSubFolder`](classes.html#example-2) ایجاد کنید.
:::

## نمایش صندوق پیغام اطلاعات

### فراخوانی شگرد و پارامترها

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

## نمایش صندوق پیغام پرسش

### Method call and parameters

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

For information about buttons see [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

You may also want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Showing an open file dialog

### فراخوانی شگرد و پارامترها

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

### Example

```js
// show an open file dialog
var fileName = script.getOpenFileName(
  "Please select an image",
  "/home/user/images",
  "Images (*.png *.xpm *.jpg)",
);
```

## Showing a save file dialog

### فراخوانی شگرد و پارامترها

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

### Example

```js
// show a save file dialog
var fileName = script.getSaveFileName(
  "Please select HTML file to save",
  "output.html",
  "HTML (*.html)",
);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Registering script settings variables

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

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

In addition, you can override the `settingsVariables` with a special function `registerSettingsVariables()` like this:

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

You may also want to take a look at the example [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

## Storing and loading persistent variables

### فراخوانی شگرد و پارامترها

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

Please make sure to use a meaningful prefix in your key like `PersistentVariablesTest/myVar` because the variables are accessible from all scripts.

You may also want to take a look at the example [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

## بارگذاری متغیرهای تنظیمات برنامه

### Method call and parameters

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

Keep in mind that settings actually can be empty, you have to take care about that yourself. `defaultValue` is only used if the setting doesn't exist at all.

## Creating a cache directory

You can cache files at the default cache location of your system.

### فراخوانی شگرد و پارامترها

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

### فراخوانی شگرد و پارامترها

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

## Reading the path to the directory of your script

If you need to get the path to the directory where your script is placed to for example load other files you have to register a `property string scriptDirPath;`. This property will be set with the path to the script's directory.

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

## Converting path separators to native ones

### فراخوانی شگرد و پارامترها

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

## تبدیل جداکننده های مسیر از موارد محلی

### فراخوانی شگرد و پارامترها

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

## دریافت جداکننده فهرست محلی

### فراخوانی شگرد و پارامترها

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

## دریافت لیستی از مسیرهای همه یادداشت های منتخب

### فراخوانی شگرد و پارامترها

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
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

You may want to take a look at the example [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml).

## دریافت لیستی از شناسه های کاربری همه یادداشت های منتخب

### فراخوانی شگرد و پارامترها

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
// returns a list of the ids of all selected notes
script.log(script.selectedNotesIds());
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## راه اندازی منو اکشن

### فراخوانی شگرد و پارامترها

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
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

You may want to take a look at the example [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). تنها عنوان منوی انگلیسی را جستجو کنید. توجه داشته باشید که این متن ها با گذشت زمان می توانند تغییر کنند.
:::

## گشودن کادر گفتگوی ورود با صندوق انتخاب

### فراخوانی شگرد و پارامترها

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

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### مثال

```js
var result = script.inputDialogGetItem("combo box", "Please select an item", [
  "Item 1",
  "Item 2",
  "Item 3",
]);
script.log(result);
```

You may want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## گشودن کادر گفتگوی ورود با ویرایش خط

### فراخوانی شگرد و پارامترها

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

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### مثال

```js
var result = script.inputDialogGetText(
  "line edit",
  "Please enter a name",
  "current text",
);
script.log(result);
```

## Opening an input dialog with a multi-line text edit

### فراخوانی شگرد و پارامترها

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

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### مثال

```js
var result = script.inputDialogGetMultiLineText(
  "multi-line edit",
  "Please enter a text",
  "current text",
);
script.log(result);
```

## Opening a dialog to show the differences between two texts

### فراخوانی شگرد و پارامترها

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

`text2` is the text you will be able to edit in the dialog. An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

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

## بررسی امکان وجود پرونده

### Method call and parameters

```cpp
/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### Example

```js
var result = script.fileExists(filePath);
script.log(result);
```

## خواندن متن از پرونده

### Method call and parameters

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

### Example

```js
if (script.fileExists(filePath)) {
  var data = script.readFromFile(filePath);
  script.log(data);
}
```

## نوشتن متن در پرونده

### Method call and parameters

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

### Example

```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## کار با وب سوکت ها

با استفاده از `WebSocketServer` می ‌توانید QOwnNotes را از راه دور کنترل کنید.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml). شما می توانید سرور سوکت را با اتصال به آن در [تست وب سوکت](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345) بررسی کنید.

همچنین می توانید سوکت ها را با `وب سوکت` بشنوید. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml).

به خاطر داشته باشید که شما باید کتابخانه `وب سوکت` QML متعلق به Qt را به صورت نصب شده داشته باشید تا بتوانید از آن استفاده کنید. برای مثال در سیستم عامل لینوکس اوبونتو شما می توانید `qml-module-qtwebsockets` را نصب کنید.

## افزودن عملکرد «Highlighting Rule» برای ویرایشگر

You can directly inject highlighting rules into the editor by defining regular expressions and assigning them to a highlighting state.

### Method call and parameters

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

### حالات برجسته سازی

| نام                    | شماره |
| ---------------------- | ----- |
| NoState                | -1    |
| اتصال                  | 0     |
| تصویر                  | 3     |
| کد بلوک                | 4     |
| توضیحات کد بلوک        | 5     |
| خوابیده                | 7     |
| توپر                   | 8     |
| لیست                   | 9     |
| توضیحات                | 11    |
| H1                     | 12    |
| H2                     | 13    |
| H3                     | 14    |
| H4                     | 15    |
| H5                     | 16    |
| H6                     | 17    |
| بلوک نقل قول           | 18    |
| خط کش افقی             | 21    |
| جدول                   | 22    |
| کد بلوک درون خطی       | 23    |
| چیدمان پوشیده          | 24    |
| رنگ پس زمینه خط کنونی  | 25    |
| لینک شکسته             | 26    |
| بلوک پیش گفتار         | 27    |
| فضای خالی              | 28    |
| کادر انتخاب بدون علامت | 29    |
| کادر انتخاب با علامت   | 30    |
| StUnderline            | 31    |

### Example

```js
// Highlight a text line like "BLOCK: some text" as blockquote (state 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Mask out (state 24) all characters after 32 characters in a line
// capturingGroup 1 means the expression from the first bracketed part of the pattern will be highlighted
// maskedGroup -1 means that no masking should be done
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

You can also take a look at the examples in [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).
