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
    // Renvoie l’URL Markdown de la note pour lier à la note avec noteId
    Q_INVOKABLE QString getNoteUrlForLinkingToNoteId(int noteId)
};
```

Vous pouvez utiliser les méthodes de [Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date) pour travailler avec `fileCreated` ou `fileLastModified`.

### Exemple

```js
script.log(note.fileCreated.toISOString());
script.log(note.fileLastModified.getFullYear());

// renomme une note en « nouveau nom.md »
note.renameNoteFile("nouveau nom");

// vérifie s’il est permis d'avoir un nom de fichier de note différent du titre
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

// affiche tous les noms de sous-dossiers
noteSubFolderQmlObj
  .fetchNoteSubFoldersByParentId(parentId)
  .forEach(function (nsf) {
    script.log(nsf.name);
  });

// récupérer le sous-dossier de notes actif
var noteSubFolder = noteSubFolderQmlObj.activeNoteSubFolder();

// affiche les chemins complet et relatif du sous-dossier de notes actif
script.log(noteSubFolder.fullPath());
script.log(noteSubFolder.relativePath());

script.log(noteSubFolder.id);
script.log(noteSubFolder.name);

// itère les notes dans le sous-dossier de notes
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

// Récupérer le tag « home »
var tag = script.getTagByNameBreadcrumbList(["home"]);
// Récupérer toutes les notes taguées avec ce tag
var notes = tag.notes;

// Itérer les notes du tag
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
    // Créé un nouveau sous-dossier de notes dans le sous-dossier courant
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // Insère du HTML dans la note courante en tant que Markdown
    // Cette méthode télécharge aussi les images distantes et transforme les URL « data:image »
    // en images locales stockées dans le dossier media
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // Recharge la note courante par son identifiant
    // Utile quand le chemin ou le nom de fichier de la note courante ont changé
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // Renvoie une liste d’UUID de dispositions
    Q_INVOKABLE QStringList getLayoutUuidList();
    // Renvoie l’UUID d’une disposition, en passant son nom
    Q_INVOKABLE QString getLayoutUuid(const QString &layoutName);
    // Change la disposition courante par UUID
    Q_INVOKABLE void setCurrentLayout(const QString &uuid);
    // Ferme un onglet de note à un indice spécifique (renvoie true en cas de succès)
    Q_INVOKABLE bool removeNoteTab(int index);
    // Renvoie la liste des identifiants des notes qui sont ouvertes dans des onglets
    Q_INVOKABLE QList<int> getNoteTabNoteIdList();
    // Va à un tag dans l’arbre des tags
    Q_INVOKABLE bool jumpToTag(int tagId);
};
```

### Exemple

```js
// Force un rechargement de la liste des notes
mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);

// Créé un nouveau sous-dossier de notes « My fancy folder » dans le sous-dossier de notes courant
mainWindow.createNewNoteSubFolder("My fancy folder");

// Insère du HTML dans la note courante en tant que Markdown
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>my headline</h2>some text");

// Charge la dispositon « Edit » en tant que disposition courante
mainWindow.setCurrentLayout(mainWindow.getLayoutUuid("Edit"));

// Va au tag « test » dans l’arbre des tags
// Il y a un exemple dans https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml
var tag = script.getTagByNameBreadcrumbList(["test"]);
mainWindow.jumpToTag(tag.id);

// Récupère toutes les notes ouvertes dans des onglets
var noteIds = mainWindow.getNoteTabNoteIdList();
noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // faire quelque chose avec la note
});
```
