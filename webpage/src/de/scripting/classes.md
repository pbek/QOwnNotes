# Exponierte Klassen

## Notiz

### Eigenschaften und Methoden

```cpp
class NoteApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(QString fileName)
    Q_PROPERTY(QString fullNoteFilePath)
    Q_PROPERTY(QString fullNoteFileDirPath)
    Q_PROPERTY(QString relativeNoteFileDirPath)
    Q_PROPERTY(int noteSubFolderId)
    Q_PROPERTY(QString noteText)
    Q_PROPERTY(QString decryptedNoteText)
    Q_PROPERTY(bool hasDirtyData)
    Q_PROPERTY(QQmlListProperty<TagApi> tags)
    Q_PROPERTY(QDateTime fileCreated)
    Q_PROPERTY(QDateTime fileLastModified)
    Q_INVOKABLE QStringList tagNames()
    Q_INVOKABLE bool addTag(QString tagName)
    Q_INVOKABLE bool removeTag(QString tagName)
    Q_INVOKABLE bool renameNoteFile(QString newName)
    Q_INVOKABLE QString toMarkdownHtml(bool forExport = true)
    Q_INVOKABLE QString getFileURLFromFileName(QString localFileName)
    Q_INVOKABLE bool allowDifferentFileName()
    // Returns the Markdown note url for linking to the note with noteId
    Q_INVOKABLE QString getNoteUrlForLinkingToNoteId(int noteId)
};
```

Sie können die Methoden von [Datum](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date) verwenden, um mit `fileCreated` oder `fileLastModified` zu arbeiten.

### Beispiel

```js
script.log(note.fileCreated.toISOString());
script.log(note.fileLastModified.getFullYear());

// benennt eine Notiz in "new name.md" um
note.renameNoteFile("new name");

// prüfe ob es einen anderen Namen der Notendatei als die Überschrift haben darf
script.log(note.allowDifferentFileName());
```

## NoteSubFolder

### Eigenschaften und Methoden

```cpp
class NoteSubFolderApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(QQmlListProperty<NoteApi> notes)
    Q_INVOKABLE static NoteSubFolderApi *fetchNoteSubFolderById(int id);
    Q_INVOKABLE static NoteSubFolderApi *activeNoteSubFolder();
    Q_INVOKABLE static QList<QObject*> fetchNoteSubFoldersByParentId(int parentId);
    Q_INVOKABLE QString relativePath();
    Q_INVOKABLE QString fullPath();
};
```

### Beispiel

```js
var noteSubFolderQmlObj = Qt.createQmlObject(
  "import QOwnNotesTypes 1.0; NoteSubFolder{}",
  mainWindow,
  "noteSubFolder",
);

// print all subfolder names
noteSubFolderQmlObj
  .fetchNoteSubFoldersByParentId(parentId)
  .forEach(function (nsf) {
    script.log(nsf.name);
  });

// get the active note subfolder
var noteSubFolder = noteSubFolderQmlObj.activeNoteSubFolder();

// print the full and relative path of the active note subfolder
script.log(noteSubFolder.fullPath());
script.log(noteSubFolder.relativePath());

script.log(noteSubFolder.id);
script.log(noteSubFolder.name);

// iterate through notes in note subfolder
for (var idx in noteSubFolder.notes) {
  var note = noteSubFolder.notes[idx];
}
```

## Schlagwort

### Eigenschaften und Methoden

```cpp
class TagApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(int parentId)
    Q_PROPERTY(QQmlListProperty<NoteApi> notes)
    Q_INVOKABLE TagApi fetchByName(const QString &name, int parentId = 0)
    Q_INVOKABLE QStringList getParentTagNames()
};
```

### Beispiel

```js
// Vergessen Sie nicht, "import QOwnNotesTypes 1.0" am Anfang Ihres Skripts zu verwenden!

// Fetch tag "home"
var tag = script.getTagByNameBreadcrumbList(["home"]);
// Fetch all notes tagged with the tag
var notes = tag.notes;

// Iterate through notes of the tag
for (var idx in notes) {
  var note = notes[idx];
  script.log(note.name);
}
```

Sie können weitere Beispiele unter [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) finden, wo die TagApi benutzt wird.

## HauptFenster

### Eigenschaften und Methoden

```cpp
class MainWindow {
    Q_INVOKABLE void reloadTagTree();
    Q_INVOKABLE void reloadNoteSubFolderTree();
    Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList(
            bool forceBuild = false, bool forceLoad = false);
    Q_INVOKABLE void focusNoteTextEdit();
    // Erstellt einen neuen Notiz-Unterordner im aktuellen Unterordner
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // Fügt HTML in die aktuelle Notiz als Markdown ein
    // Diese Methode läd auch remote Bilder herunter und transformiert "data:image"
    // URLs zu lokalen Bildern, die im Medienverzeichnis gespeichert sind
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // Lädt die aktuelle Notiz per ID neu
    // Das ist sinnvoll, wenn der Pfad oder der Dateiname der aktuellen Notiz geändert wurde
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // Gibt die List der Arbeitsumgebungs UUIDs aus
    Q_INVOKABLE QStringList getWorkspaceUuidList();
    // Gibt die UUID einer Arbeitsumgebung aus, indem der Name der Arbeitsumgebung eingegeben wird
    Q_INVOKABLE QString getWorkspaceUuid(const QString &workspaceName);
    // Legt die aktuelle Arbeitsumgebung per UUID fest
    Q_INVOKABLE void setCurrentWorkspace(const QString &uuid);
    // Schließt einen Notiz-Tab mit einem speziellen Index (gibt bei Erfolg true aus)
    Q_INVOKABLE bool removeNoteTab(int index);
    // Gibt eine Liste von Notiz-IDs aus, die in Tabs geöffnet werden
    Q_INVOKABLE QList<int> getNoteTabNoteIdList();
    // Springt zu einem Schlagwort im Schlagwortbaum
    Q_INVOKABLE bool jumpToTag(int tagId);
};
```

### Beispiel

```js
// Force a reload of the note list
mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);

// Creates a new note subfolder "My fancy folder" in the current subfolder
mainWindow.createNewNoteSubFolder("My fancy folder");

// Inserts html in the current note as markdown
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>my headline</h2>some text");

// Set 'Edit' workspace as current workspace
mainWindow.setCurrentWorkspace(mainWindow.getWorkspaceUuid("Edit"));

// Jump to the tag "test" in the tag tree
// There is an example in https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml
var tag = script.getTagByNameBreadcrumbList(["test"]);
mainWindow.jumpToTag(tag.id);

// Get all notes that are opened in tabs
var noteIds = mainWindow.getNoteTabNoteIdList();
noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // do something with the note
});
```
