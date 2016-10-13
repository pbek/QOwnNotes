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


### Check if platform is Linux, OS X or Windows

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
