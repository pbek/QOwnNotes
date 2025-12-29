# Classes exposées

## Note

### Propriétés et méthodes

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

Vous pouvez utiliser les méthodes de [Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date) pour travailler avec `fileCreated` ou `fileLastModified`.

### Exemple

```js
script.log(note.fileCreated.toISOString());
script.log(note.fileLastModified.getFullYear());

// renomme une note en tant que "nouveau nom.md"
note.renameNoteFile("nouveau nom");

// vérifie si il est permis d'avoir un nom de fichier de note différent du titre
script.log(note.allowDifferentFileName());
```

## NoteSubFolder

### Propriétés et méthodes

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

### Exemple

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

## Tag

### Propriétés et méthodes

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

### Exemple

```js
// Noubliez pas d'utiliser "import QOwnNotesTypes 1.0" en tête de votre script !

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

Vous trouverez ici d'autres exemples dans lesquels TagApi est utilisé : [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml).

## MainWindow

### Propriétés et méthodes

```cpp
class MainWindow {
    Q_INVOKABLE void reloadTagTree();
    Q_INVOKABLE void reloadNoteSubFolderTree();
    Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList(
            bool forceBuild = false, bool forceLoad = false);
    Q_INVOKABLE void focusNoteTextEdit();
    // Créer un nouveau sous-dossier de notes dans le sous-dossier courant
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // Insère le HTML dans la note courante au format MarkDown
    // Cette méthode télécharge également les images distantes et transforme les URL "data:image"
    // en images stockées localement dans le dossier media
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // Re-charge la note courante, par id
    // Ceci est utile quand le chemin ou le nom de fichier de la note courante ont changé
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // Retourne la liste des UUID des espaces de travail
    Q_INVOKABLE QStringList getWorkspaceUuidList();
    // Retourne l'UUID correspondant au nom de l'espace de travail entré
    Q_INVOKABLE QString getWorkspaceUuid(const QString &workspaceName);
    // Définit l'espace de travail courant par son UUID
    Q_INVOKABLE void setCurrentWorkspace(const QString &uuid);
    // Ferme un onglet de note sur un index spécifique (retourne true en cas de succès)
    Q_INVOKABLE bool removeNoteTab(int index);
    // Saute à une étiquette dans l'arbre des étiquettes
    Q_INVOKABLE bool jumpToTag(int tagId);
};
```

### Exemple

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
