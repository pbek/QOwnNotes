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

NoteSubFolder
----

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
var noteSubFolderQmlObj = Qt.createQmlObject("import QOwnNotesTypes 1.0; NoteSubFolder{}", mainWindow, "noteSubFolder");

// imprimer tous les noms de sous-dossiers
noteSubFolderQmlObj.fetchNoteSubFoldersByParentId(parentId).forEach(function(nsf) {
    script.log(nsf.name);
});

// obtenir le sous-dossier de la note active
var noteSubFolder = noteSubFolderQmlObj.activeNoteSubFolder();

// imprimer l'intégralité du chemin relatif du sous-dossier de la note active
script.log(noteSubFolder.fullPath());
script.log(noteSubFolder.relativePath());

script.log(noteSubFolder.id);
script.log(noteSubFolder.name);

// itérer en suivant les notes dans le sous-dossier de notes
for (var idx in noteSubFolder.notes) {
    var note = noteSubFolder.notes[idx];
}
```

Tag
---

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

// Récupérer l'étiquette "home"
var tag = script.getTagByNameBreadcrumbList(["home"]);
// Récupérer toutes les notes portant cette étiquette
var notes = tag.notes;

// Itérer les notes portant l'étiquette
for (var idx in notes) {
    var note = notes[idx];
    script.log(note.name);
}
```

Vous trouverez ici d'autres exemples dans lesquels TagApi est utilisé [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml).

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
// Force un re-chargement de la liste des notes
mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);

// Créé le nouveau sous-dossier de notes "Mon joli dossier" dans le sous-dossier courant
mainWindow.createNewNoteSubFolder("Mon joli dossier");

// Insère du HTML en tant que MarkDown dans la note courante
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>my headline</h2>some text");

// Définir l'espace de travail 'Edit' comme espace de travail par défaut
mainWindow.setCurrentWorkspace(mainWindow.getWorkspaceUuid("Edit"));

// Sauter à l'étiquette "test" dans l'arbre des étiquettes
// Un exemple est dispponible à cette adresse https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml
var tag = script.getTagByNameBreadcrumbList(["test"]);
mainWindow.jumpToTag(tag.id);

```
