# QOwnNotes Scripting

- if you need access to a certain functionality in QOwnNotes or have 
  questions or ideas please open an issue on the 
  [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues)
- since debug output is disabled in the releases of QOwnNotes, so you might want
  to use `console.warn()` instead of `console.log()` to actually see an output
    - additionally you can also use the `script.log()` command to log to the 
      log widget

## Methods and objects QOwnNotes provides

### Starting an external program in the background

#### Parameters

```cpp
/**
 * QML wrapper to start a detached process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @return true on success, false otherwise
 */
bool startDetachedProcess(QString executablePath, QStringList parameters);
```

#### Usage in QML

```javascript
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

You may want to take a look at the example [custom-actions.qml](custom-actions.qml)
or [execute-command-after-note-update.qml](execute-command-after-note-update.qml).


```cpp
/**
 * QML wrapper to start a synchronous process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process
 * @return the text that was returned by the process
bool QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data);
```

#### Usage in QML

```javascript
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

You may want to take a look at the example [encryption-keybase.qml](encryption-keybase.qml).


### Getting the path of the current note folder

#### Parameters

```cpp
/**
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
 */
QString currentNoteFolderPath();
```

#### Usage in QML

```javascript
var path = script.currentNoteFolderPath();
```

You may want to take a look at the example [absolute-media-links.qml](absolute-media-links.qml).


### Getting the current note

#### Parameters

```cpp
/**
 * QML wrapper to get the current note
 *
 * @returns {NoteApi} the the current note object
 */
NoteApi currentNote();
```

#### Usage in QML

```javascript
var note = script.currentNote();
```

You may want to take a look at the example [custom-actions.qml](custom-actions.qml).


### Logging to the log widget

#### Parameters

```cpp
/**
 * QML wrapper to log to the log widget
 *
 * @param text
 */
void log(QString text);
```

#### Usage in QML

```javascript
script.log("my text");
```


### Downloading an url to a string

#### Parameters

```cpp
/**
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString downloadUrlToString(QUrl url);
```

#### Usage in QML

```javascript
var html = script.downloadUrlToString("http://www.qownnotes.org");
```

You may want to take a look at the example
[insert-headline-with-link-from-github-url.qml](insert-headline-with-link-from-github-url.qml).


### Register a custom action

#### Parameters

```cpp
/**
 * Registers a custom action
 * 
 * @param identifier the identifier of the action 
 * @param menuText the text shown in the menu
 * @param buttonText the text shown on the button
 *                   (no button will be viewed if empty)
 * @param icon the icon file path or the name of a freedesktop theme icon
 *             you will find a list of icons here:
 *             https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 */
void ScriptingService::registerCustomAction(QString identifier,
                                            QString menuText,
                                            QString buttonText,
                                            QString icon);
```

#### Usage in QML

```javascript
// add a custom action without a button
script.registerCustomAction("mycustomaction1", "Menu text");

// add a custom action with a button
script.registerCustomAction("mycustomaction1", "Menu text", "Button text");

// add a custom action with a button and freedesktop theme icon 
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "task-new");

// add a custom action with a button and an icon from a file 
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

You may then want to use the identifier with function `customActionInvoked`
in a script like [custom-actions.qml](custom-actions.qml).


### Create a new note

#### Parameters

```cpp
/**
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

#### Usage in QML

```javascript
script.createNote("My note headline\n===\n\nMy text");
```

You may want to take a look at the example [custom-actions.qml](custom-actions.qml).


### Accessing the clipboard

#### Parameters

```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

#### Usage in QML

```javascript
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

You may want to take a look at the example [custom-actions.qml](custom-actions.qml).


### Write text to the note text edit

#### Parameters

```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

#### Usage in QML

```javascript
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

You might want to look at the custom action `transformTextRot13` in the
example [custom-actions.qml](custom-actions.qml).


### Read the selected text in the note text edit

#### Parameters

```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

#### Usage in QML

```javascript
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

You might want to look at the custom action `transformTextRot13` in the
example [custom-actions.qml](custom-actions.qml).


### Check whether platform is Linux, OS X or Windows

#### Parameters

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

#### Usage in QML

```javascript
if (script.platformIsLinux()) {
    // only will be executed if under Linux
}
```


### Tag the current note

#### Parameters

```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

#### Usage in QML

```javascript
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

You might want to look at the custom action `favoriteNote` in the
example [favorite-note.qml](favorite-note.qml).


### Add a custom stylesheet

#### Parameters

```cpp
/**
 * Adds a custom stylesheet to the application
 * 
 * @param stylesheet 
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

#### Usage in QML

```javascript
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

You may want to take a look at the example [custom-stylesheet.qml](custom-stylesheet.qml).

You can get the object names from the `*.ui` files, for example
[mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html)
for a reference of what styles are available.


## Hooks

### noteOpenedHook

```javascript
/**
 * This function is called after a note was opened
 * 
 * @param {NoteApi} note - the note object that was opened
 */
function noteOpenedHook(note) {
    script.log(note.fullNoteFilePath);
}
```

You may want to take a look at the example [on-note-opened.qml](on-note-opened.qml).


### insertMediaHook

```javascript
/**
 * This function is called when media file is inserted into the note
 * If this function is defined in multiple scripts, then the first script that returns a non-empty string wins
 * 
 * @param fileName string the file path of the source media file before it was copied to the media folder
 * @param mediaMarkdownText string the markdown text of the media file, e.g. ![my-image](file://media/505671508.jpg)
 * @return string the new markdown text of the media file
 */
function insertMediaHook(fileName, mediaMarkdownText);
```

You may want to take a look at the example [example.qml](example.qml).


### insertingFromMimeDataHook

```javascript
/**
 * This function is called when html or a media file is pasted to a note with `Ctrl + Shift + V`
 * 
 * @param text text of the QMimeData object
 * @param html html of the QMimeData object
 * @returns the string that should be inserted instead of the text from the QMimeData object
 */
function insertingFromMimeDataHook(text, html);
```

You may want to take a look at the example [example.qml](example.qml),
[insert-headline-with-link-from-github-url.qml](insert-headline-with-link-from-github-url.qml)
or [note-text-from-5pm-mail.qml](note-text-from-5pm-mail.qml).


### handleNoteTextFileNameHook

```javascript
/**
 * This function is called when a note gets stored to disk if
 * "Allow note file name to be different from headline" is enabled 
 * in the settings
 * 
 * It allows you to modify the name of the note file
 * Return an empty string if the file name of the note should 
 * not be modified
 * 
 * @param {NoteApi} note - the note object of the stored note
 * @return {string} the file name of the note
 */
function handleNoteTextFileNameHook(note);
```

You may want to take a look at the example [example.qml](example.qml) or
[use-tag-names-in-filename.qml](use-tag-names-in-filename.qml).


### handleNewNoteHeadlineHook

```javascript
/**
 * This function is called before a note note is created
 * 
 * It allows you to modify the headline of the note before it is created
 * Note that you have to take care about a unique note name, otherwise
 * the new note will not be created, it will just be found in the note list
 *
 * You can use this method for creating note templates
 * 
 * @param headline text that would be used to create the headline
 * @return {string} the headline of the note
 */
function handleNewNoteHeadlineHook(headline);
```

You may want to take a look at the example [custom-new-note-headline.qml](custom-new-note-headline.qml).


### noteToMarkdownHtmlHook

```javascript
/**
 * This function is called when the markdown html of a note is generated
 * 
 * It allows you to modify this html
 * This is for example called before by the note preview
 * 
 * @param {NoteApi} note - the note object
 * @param {string} html - the html that is about to being rendered
 * @return {string} the modfied html or an empty string if nothing should be modified
 */
function noteToMarkdownHtmlHook(note, html);
```

You may want to take a look at the example [example.qml](example.qml) or
[preview-styling.qml](preview-styling.qml).


### encryptionHook

```javascript
/**
 * This function is called when text has to be encrypted or decrypted
 * 
 * @param text string the text to encrypt or descrypt
 * @param password string the password
 * @param decrypt bool if false encryption is demanded, if true decryption is demanded
 * @return the exncrypted or decrypted text
 */
function encryptionHook(text, password, decrypt);
```

You may want to take a look at the example [encryption-keybase.qml](encryption-keybase.qml),
[encryption-pgp.qml](encryption-pgp.qml) or [encryption-rot13.qml](encryption-rot13.qml).


## Exposed classes

### Note

```cpp
class NoteApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(QString fileName)
    Q_PROPERTY(QString fullNoteFilePath)
    Q_PROPERTY(int noteSubFolderId)
    Q_PROPERTY(QString noteText)
    Q_PROPERTY(QString decryptedNoteText)
    Q_PROPERTY(bool hasDirtyData)
    Q_PROPERTY(QQmlListProperty<TagApi> tags)
    Q_INVOKABLE QStringList tagNames();
};
```

### Tag

```cpp
class TagApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(int parentId)
};
```
