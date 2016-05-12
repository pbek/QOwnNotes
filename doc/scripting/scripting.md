# QOwnNotes Scripting
 
## Methods and objects QOwnNotes provides

### Starting an external program in the background

```javascript
script.startDetachedProcess("/path/to/my/program", [fileName]);
```

```cpp
bool startDetachedProcess(QString executablePath, QStringList parameters);
```
