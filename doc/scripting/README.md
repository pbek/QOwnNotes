# QOwnNotes Scripting

- you may need to restart QOwnNotes to let changes in a script take action 
- if you need access to a certain functionality in QOwnNotes or have 
  questions or ideas please open an issue on the 
  [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues)


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
