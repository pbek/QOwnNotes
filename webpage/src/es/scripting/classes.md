# Clases expuestas

Nota
----

### Propiedades y métodos
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

Puede utilizar los métodos de [Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date) para trabajar con `fileCreated` o `fileLastModified`.

### Ejemplo
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

### Propiedades y métodos
```cpp
class NoteSubFolderApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(QQmlListProperty<NoteApi> notes)
    Q_INVOKABLE static NoteSubFolderApi *fetchNoteSubFolderById(int id);
    Q_INVOKABLE static QList<QObject*> fetchNoteSubFoldersByParentId(int parentId);
};
```

### Ejemplo
```js
script.log (noteSubFolder.id);
script.log (noteSubFolder.name);

// iterar a través de notas en la subcarpeta de notas
para (var idx en noteSubFolder.notes) {
     var note = noteSubFolder.notes [idx];
}

// imprime todos los nombres de las subcarpetas
noteSubFolder.fetchNoteSubFoldersByParentId (parentId).forEach (function (nsf) {
     script.log (nsf.name);
});
```

Etiqueta
---

### Properties and methods
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

### Propiedades y métodos
```cpp
class MainWindow {
    Q_INVOKABLE void reloadTagTree ();
    Q_INVOKABLE void reloadNoteSubFolderTree ();
    Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList (
            bool forceBuild = falso, bool forceLoad = falso);
    Q_INVOKABLE void focusNoteTextEdit ();
    // Crea una nueva subcarpeta de notas en la subcarpeta actual
    Q_INVOKABLE bool createNewNoteSubFolder (QString folderName = "");
    // Inserta html en la nota actual como rebaja
    // Este método también descarga imágenes remotas y transforma "data: image"
    // URL a imágenes locales almacenadas en el directorio de medios
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote (QString html);
    // Recarga la nota actual por id
    // Esto es útil cuando cambia la ruta o el nombre de archivo de la nota actual
    Q_INVOKABLE void reloadCurrentNoteByNoteId ();
    // Devuelve la lista de UUID del espacio de trabajo
    Q_INVOKABLE QStringList getWorkspaceUuidList ();
    // Devuelve el UUID de un espacio de trabajo, pasando el nombre del espacio de trabajo
    Q_INVOKABLE QString getWorkspaceUuid (const QString & nombre de espacio de trabajo);
    // Establece el espacio de trabajo actual por UUID
    Q_INVOKABLE void setCurrentWorkspace (const QString & uuid);
};
```

### Ejemplo
```js
// Forzar una recarga de la lista de notas
mainWindow.buildNotesIndexAndLoadNoteDirectoryList (verdadero, verdadero);

// Crea una nueva subcarpeta de notas "Mi carpeta elegante" en la subcarpeta actual
mainWindow.createNewNoteSubFolder ("Mi carpeta de fantasía");

// Inserta html en la nota actual como rebaja
mainWindow.insertHtmlAsMarkdownIntoCurrentNote ("<h2>mi título</h2> algo de texto");

// Establecer el espacio de trabajo 'Editar' como espacio de trabajo actual
mainWindow.setCurrentWorkspace (mainWindow.getWorkspaceUuid ("Editar"));
```
