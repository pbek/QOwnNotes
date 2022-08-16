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
    Q_INVOKABLE TagApi fetchByName(const QString &name, int parentId = 0)
    Q_INVOKABLE QStringList getParentTagNames()
};
```

يمكنك أن تجد مثالا على استخدام TagApi في [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml).

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
    // يُنشئ مجلد فرعي جديد للملاحظة في المجلد الفرعي الحالي
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // يضيف هتمل بصيغة ماركداون في الملاحظة الحالية
    // هذه الدالة أيضا تقوم بتنزيل الصور البعيدة وتحويل الروابط من النوع
    // "data:image"
    // إلى صور محلية مخزنة في مجلد الوسائط
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // يُعيد تحميل الملاحظة الحالية بمُعرِّفها
    // هذا مفيد عندما يتغير مسار أو اسم ملف الملاحظة الحالية
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // يُعيد قائمة بالمُعرِّفات العالمية الفريدة لمساحات العمل (UUID)
    Q_INVOKABLE QStringList getWorkspaceUuidList();
    // يُعطى اسم مساحة عمل ويُعيد مُعرِّفها العالمي الفريد (UUID)
    Q_INVOKABLE QString getWorkspaceUuid(const QString &workspaceName);
    // يجعل مساحة العمل الحالية هي المساحة ذات المُعرِّف العالمي الفريد المُعطى (UUID)
    Q_INVOKABLE void setCurrentWorkspace(const QString &uuid);
};
```

### مثال
```js
// إعادة تحميل قائمة الملاحظات بالقوة
mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);

// إنشاء مجلد فرعي جديد لملاحظة باسم «مجلدي الفخيم» في المجلد الفرعي الحالي
mainWindow.createNewNoteSubFolder("مجلدي الفخيم");

// إضافة هتمل إلى الملاحظة الحالية بصيغة ماركداون
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>عنوان</h2>بعض النص");

// جعل مساحة العمل «تحرير» هي المساحة الحالية
mainWindow.setCurrentWorkspace(mainWindow.getWorkspaceUuid("تحرير"));
```
