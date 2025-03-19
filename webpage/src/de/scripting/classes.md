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

// gib alle Unterordnernamen aus
noteSubFolderQmlObj
  .fetchNoteSubFoldersByParentId(parentId)
  .forEach(function (nsf) {
    script.log(nsf.name);
  });

// hole den aktuellen Notiz-Unterordner
var noteSubFolder = noteSubFolderQmlObj.activeNoteSubFolder();

// gib den vollständigen und relativen Pfad des aktuellen Notiz-Unterordners aus
script.log(noteSubFolder.fullPath());
script.log(noteSubFolder.relativePath());

script.log(noteSubFolder.id);
script.log(noteSubFolder.name);

// iteriere durch die Notizen im Notizen-Unterordner
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

// hole das Schlagwort "home"
var tag = script.getTagByNameBreadcrumbList(["home"]);
// hole alle Notizen mit dem Schlagwort
var notes = tag.notes;

// iteriere durch die Notizen des Schlagwortes
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
// Erzwinge das Neuladen der Notizliste
mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);

// Erstellt einen neuen Unterordner "My fancy folder" im aktuellen Unterordner
mainWindow.createNewNoteSubFolder("My fancy folder");

// Fügt HTML in die aktuelle Notiz als Markdown ein
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>my headline</h2>some text");

// Legt "Edit"-Arbeitsumgebung als aktuelle Arbeitsumgebung fest
mainWindow.setCurrentWorkspace(mainWindow.getWorkspaceUuid("Edit"));

// Springt zum Schlagwort "test" im Schlagwortbaum
// Es gibt ein Beispiel auf https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml
var tag = script.getTagByNameBreadcrumbList(["test"]);
mainWindow.jumpToTag(tag.id);

// Erhalte alle Notizen, die in Tabs geöffnet sind
var noteIds = mainWindow.getNoteTabNoteIdList();
noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // mach was mit der Notiz
});
```
