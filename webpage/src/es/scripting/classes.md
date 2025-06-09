# Clases expuestas

## Nota

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
    // Returns the Markdown note url for linking to the note with noteId
    Q_INVOKABLE QString getNoteUrlForLinkingToNoteId(int noteId)
};
```

Puede utilizar los métodos de [Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date) para trabajar con `fileCreated` o `fileLastModified`.

### Ejemplo

```js
script.log(note.fileCreated.toISOString());
script.log(note.fileLastModified.getFullYear());

// renombra una nota a "nuevo nombre.md"
note.renameNoteFile("nuevo nombre");

// verifica si está permitido tener un nombre de archivo de nota diferente al del título
script.log(note.allowDifferentFileName());
```

## NoteSubFolder

### Propiedades y métodos

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

### Ejemplo

```js
var noteSubFolderQmlObj = Qt.createQmlObject(
  "import QOwnNotesTypes 1.0; NoteSubFolder{}",
  mainWindow,
  "noteSubFolder",
);

// imprimir todos los nombres de las subcarpetas
noteSubFolderQmlObj
  .fetchNoteSubFoldersByParentId(parentId)
  .forEach(function (nsf) {
    script.log(nsf.name);
  });

// obtener la subcarpeta de la nota activa
var noteSubFolder = noteSubFolderQmlObj.activeNoteSubFolder();

// imprimir la ruta completa y relativa de la subcarpeta de la nota activa
script.log(noteSubFolder.fullPath());
script.log(noteSubFolder.relativePath());

script.log(noteSubFolder.id);
script.log(noteSubFolder.name);

// iterar a través de las notas en la subcarpeta de la nota
for (var idx in noteSubFolder.notes) {
  var nota = noteSubFolder.notes[idx];
}
```

## Etiqueta

### Propiedades y métodos

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

### Ejemplo

```js
// ¡No olvide usar “import QOwnNotesTypes 1.0” en la parte superior de su script!

// Obtener etiqueta "home"
var etiqueta = script.getTagByNameBreadcrumbList(["home"]);
// Obtener todas las notas etiquetadas con la etiqueta
var notas = etiqueta.notes;

// Iterar a través de las notas de la etiqueta
for (var idx in notas) {
  var nota = notas[idx];
  script.log(nota.name);
}
```

Puede encontrar mas ejemplos donde se usa TagApi en [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml).

## MainWindow

### Propiedades y métodos

```cpp
class MainWindow {
    Q_INVOKABLE void reloadTagTree();
    Q_INVOKABLE void reloadNoteSubFolderTree();
    Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList(
            bool forceBuild = false, bool forceLoad = false);
    Q_INVOKABLE void focusNoteTextEdit();
    // Crea una nueva subcarpeta de notas en la subcarpeta actual
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // Inserta HTML en la nota actual como Markdown
    // Este método también descarga imágenes remotas y transforma las URL de "data:image"
    // en imágenes almacenadas en el directorio de medios
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // Recarga la nota actual por ID
    // Esto es util cuando la ruta o el nombre de archivo de la nota actual ha cambiado
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // Devuelve la lista de los UUID de los espacios de trabajo
    Q_INVOKABLE QStringList getWorkspaceUuidList();
    // Devuelve el UUID de un espacio de trabajo, pasando el nombre del espacio de trabajo
    Q_INVOKABLE QString getWorkspaceUuid(const QString &workspaceName);
    // Establece el espacio de trabajo actual por medio de un UUID
    Q_INVOKABLE void setCurrentWorkspace(const QString &uuid);
    // Cierra una pestaña de nota en un índice específico(devuelve verdadero si es exitoso)
    Q_INVOKABLE bool removeNoteTab(int index);
    // Devuelve una lista de IDs de las notas que están abiertas en pestañas
    Q_INVOKABLE QList<int> getNoteTabNoteIdList();
    // Salta a una etiqueta en el árbol de etiquetas
    Q_INVOKABLE bool jumpToTag(int tagId);
};
```

### Ejemplo

```js
// Forzar una recarga de la lista de notas
mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);

// Crear una nueva subcarpeta de notas "Mi carpeta estrafalaria" en la subcarpeta actual
mainWindow.createNewNoteSubFolder("Mi carpeta estrafalaria");

// Insertar HTML en la nota actual como Markdown
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>mi título</h2>algo de texto");

// Establecer el espacio de trabajo "Edición" como espacio de trabajo actual
mainWindow.setCurrentWorkspace(mainWindow.getWorkspaceUuid("Edición"));

// Saltar a la etiqueta "prueba" en el árbol de etiquetas
// Hay un ejemplo en https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml
var etiqueta = script.getTagByNameBreadcrumbList(["prueba"]);
mainWindow.jumpToTag(etiqueta.id);

// Obtener todas las notas de las pestañas que están abiertas 
var IDnotas = mainWindow.getNoteTabNoteIdList();
IDnotas.forEach(function (IDnota){
  var nota = script.fetchNoteById(IDnota);

  // Hacer algo con la nota
});
```
