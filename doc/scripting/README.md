# QOwnNotes Scripting

- you may need to restart QOwnNotes to let changes in a script take action 
- if you need access to a certain functionality in QOwnNotes or have 
  questions or ideas please open an issue on the 
  [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues)
- since debug output is disabled in the releases of QOwnNotes you might want to use `console.warn()` instead of `console.log()` to actually see an output
    - additionally you need to run QOwnNotes from the command line to see the outputs on the command line

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

### Getting the current note

#### Parameters

```cpp
/**
 * QML wrapper to get the current note folder path
 *
 * @returns {NoteApi} the the current note object
 */
QString currentNote();
```

#### Usage in QML

```javascript
var note = script.currentNote();
```

### Logging to the log dialog

#### Parameters

```cpp
/**
 * QML wrapper to log to the log dialog
 *
 * @param text
 */
void log(QString text);
```

#### Usage in QML

```javascript
script.log("my text");
```

## Exposed classes

### Note

```cpp
class NoteApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(QString fileName)
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
