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
    // تنشئ مجلد ملاحظة فرعي جديد في المجلد الفرعي الحالي
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // تضيف هتمل إلى الملاحظة الحالية بصيغة ماركداون
    // وتنزّل أيضا الصور البعيدة وتحوّل روابط البيانات
    // ("data:image")
    // إلى صور محلية مخزنة في مجلد الوسائط
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // تعيد تحميل الملاحظة الحالية، بدلالة معرّفها
    // تفيد هذه الدالة عندما يتغير مسار الملاحظة الحالية أو اسم ملفها
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // ترجع قائمة بالمعرّفات العالمية الفريدة لمساحات العمل (UUIDs)
    Q_INVOKABLE QStringList getWorkspaceUuidList();
    // ترجع المعرّف العالمي الفريد لمساحة عمل، بدلالة اسمها
    Q_INVOKABLE QString getWorkspaceUuid(const QString &workspaceName);
    // تضبط مساحة العمل الحالية، بدلالة معرّفها العالمي الفريد
    Q_INVOKABLE void setCurrentWorkspace(const QString &uuid);
    // تغلق تبويب ملاحظة، بدلالة دليله، وعند النجاح ترجع القيمة المنطقية الصادقة (true)
    Q_INVOKABLE bool removeNoteTab(int index);
    // ترجع قائمة بمعرِّفات الملاحظات المفتوحة في تبويبات
    Q_INVOKABLE QList<int> getNoteTabNoteIdList();
    // تنتقل إلى وسم في شجرة الوسوم
    Q_INVOKABLE bool jumpToTag(int tagId);
};
```

### مثال
```js
// أعد تحميل قائمة الملاحظات بالقوة
mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);

// أنشى مجلد ملاحظات فرعي جديد باسم «مجلدي الفخيم» في المجلد الفرعي الحالي
mainWindow.createNewNoteSubFolder("مجلدي الفخيم");

// أضف هتمل في الملاحظة الحالية بصيغة ماركداون
mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>عنوان</h2>بعض النص");

// اجعل مساحة العمل التي اسمها «تحرير» هي مساحة العمل الحالية
mainWindow.setCurrentWorkspace(mainWindow.getWorkspaceUuid("تحرير"));

// انتقل إلى الوسم «تجربة» في قائمة الوسوم
// يوجد مثال في
// https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml
var tag = script.getTagByNameBreadcrumbList(["تجربة"]);
mainWindow.jumpToTag(tag.id);

// اجلب جميع الملاحظات المفتوحة في تبويبات
var noteIds = mainWindow.getNoteTabNoteIdList();
noteIds.forEach(function (noteId){
    var note = script.fetchNoteById(noteId);

    // افعل شيئا بالملاحظة
});

```
