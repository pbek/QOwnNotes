# الفصائل (الكلاسات) المكشوفة

Note (ملاحظة)
----

### الخصائص والدوال
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

يمكنك استخدام دوال [Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date) للتعامل مع `fileCreated` أو `fileLastModified`.

### مثال
```js
script.log(note.fileCreated.toISOString());
script.log(note.fileLastModified.getFullYear());

// يغيّر اسم ملف الملاحظة إلى "new name.md"
note.renameNoteFile("new name");

// يتحقق إذا كان مسموحا بجعل اسم ملف الملاحظة مختلفًا عن عنوانها
script.log(note.allowDifferentFileName());
```

NoteSubFolder (مجلد فرعي لملاحظة)
----

### الخصائص والدوال
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

### مثال
```js
var noteSubFolderQmlObj = Qt.createQmlObject("import QOwnNotesTypes 1.0; NoteSubFolder{}", mainWindow, "noteSubFolder");

// اطبع أسماء جميع المجلدات الفرعية
noteSubFolderQmlObj.fetchNoteSubFoldersByParentId(parentId).forEach(function(nsf) {
    script.log(nsf.name);
});

// اجلب كائن المجلد الفرعي للملاحظة الفعّالة حاليا
var noteSubFolder = noteSubFolderQmlObj.activeNoteSubFolder();

// اطبع المسار الكامل والمسار النسبي لهذا المجلد الفرعي
script.log(noteSubFolder.fullPath());
script.log(noteSubFolder.relativePath());

script.log(noteSubFolder.id);
script.log(noteSubFolder.name);

// نفذ كود لكل ملاحظة في هذا المجلد الفرعي
for (var idx in noteSubFolder.notes) {
    var note = noteSubFolder.notes[idx];
}
```

Tag (وسم)
---

### الخصائص والدوال
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

### مثال
```js
// لا تنس أن تبدأ بُريمجك بالعبارة "import QOwnNotesTypes 1.0"!

// أحضر الوسم "home"
var tag = script.getTagByNameBreadcrumbList(["home"]);
// أحضر جميع الملاحظات الموسومة بهذا الوسم
var notes = tag.notes;

// كرر كودًا على جميع الملاحظات ذات الوسم المحدد
for (var idx in notes) {
    var note = notes[idx];
    script.log(note.name);
}
```

يمكنك أن تجد أمثلة أكثر على استخدام TagApi في [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml).

MainWindow (النافذة الرئيسية)
----------

### الخصائص والدوال
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
    // Closes a note tab on a specific index (returns true if successful)
    Q_INVOKABLE bool removeNoteTab(int index);
    // Jumps to a tag in the tag tree
    Q_INVOKABLE bool jumpToTag(int tagId);
};
```

### مثال
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
// There is an example in https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml
var tag = script.getTagByNameBreadcrumbList(["test"]);
mainWindow.jumpToTag(tag.id);
```
