# Classes exposées

Note
----

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
};
```

You can use the methods from [Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date) to work with `fileCreated` or `fileLastModified`.

### Exemple
```js
script.log(note.fileCreated.toISOString());
script.log(note.fileLastModified.getFullYear());

// renames a note to "new name.md"
note.renameNoteFile("new name");

// check if it is allowed to have a different note file name than the headline
script.log(note.allowDifferentFileName());
```

NoteSubFolder
----

### Propriétés et méthodes
```cpp
class NoteSubFolderApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(QQmlListProperty<NoteApi> notes)
    Q_INVOKABLE static NoteSubFolderApi *fetchNoteSubFolderById(int id);
    Q_INVOKABLE static QList<QObject*> fetchNoteSubFoldersByParentId(int parentId);
};
```

### Example
```js
script.log(noteSubFolder.id);
script.log(noteSubFolder.name);

// iterate through notes in note subfolder
for (var idx in noteSubFolder.notes) {
    var note = noteSubFolder.notes[idx];
}

// print all subfolder names
noteSubFolder.fetchNoteSubFoldersByParentId(parentId).forEach(function(nsf) {
    script.log(nsf.name);
});
```

Tag
---

### Propriétés et méthodes
```cpp
class TagApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(int parentId)
    Q_INVOKABLE TagApi fetchByName(const QString &name, int parentId = 0)
    Q_INVOKABLE QStringList getParentTagNames()
};
```

MainWindow
----------

### Propriétés et méthodes
```cpp
class MainWindow {
    Q_INVOKABLE void reloadTagTree();
    Q_INVOKABLE void reloadNoteSubFolderTree();
    Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList(
            bool forceBuild = false, bool forceLoad = false);
    Q_INVOKABLE void focusNoteTextEdit();
    // Creates a new note subfolder in the current subfolder
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // Inserts html in the current note as markdown
    // This method also downloads remote images and transforms "data:image"
    // urls to local images stored in the media directory
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // Reloads the current note by id
    // This is useful when the path or filename of the current note changed
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // Returns the list of workspace UUIDs
    Q_INVOKABLE QStringList getWorkspaceUuidList();
    // Returns the UUID of a workspace, passing in the workspace name
    Q_INVOKABLE QString getWorkspaceUuid(const QString &workspaceName);
    // Sets the current workspace by UUID
    Q_INVOKABLE void setCurrentWorkspace(const QString &uuid);
};
```

### Exemple
```js
// Forcer un rechargement de la liste de notes
mainWindow.buildNotesIndexAndLoadNoteDirectoryList (vrai, vrai);

// Crée un nouveau sous-dossier de notes "Mon dossier fantaisie" dans le sous-dossier actuel
mainWindow.createNewNoteSubFolder ("Mon dossier fantaisie");

// Insère html dans la note actuelle comme markdown
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2> mon titre </h2> du texte");

// Définit l'espace de travail 'Modifier' comme espace de travail courant
mainWindow.setCurrentWorkspace (mainWindow.getWorkspaceUuid ("Modifier"));
```
