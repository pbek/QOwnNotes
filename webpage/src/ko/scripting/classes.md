# 해설 클래스

## 노트

### 속성 및 방식

```cpp
클래스 NoteApi {
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
```

[날짜](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date)의 방식을 사용하여 만든 `fileCreated` 또는 `fileLastModified`로 작업할 수 있습니다.

### 예제

```js
script.log(note.fileCreated.toISOString());
script.log(note.fileLastModified.getFullYear());

// 노트 이름을 "new name.md"로 변경
note.renameNoteFile("new name");

// 헤드라인과 다른 노트 파일 이름을 가질 수 있는지 확인
script.log(note.allowDifferentFileName());
```

## 노트 하위 폴더

### 속성 및 방식

```cpp
클래스 NoteSubFolderApi {
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

### 예제

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

## 태그

### 속성 및 방식

```cpp
클래스 TagApi {
    Q_PROPERTY(int id)
    Q_PROPERTY(QString name)
    Q_PROPERTY(int parentId)
    Q_PROPERTY(QQmlListProperty<NoteApi> notes)
    Q_INVOKABLE TagApi fetchByName(const QString &name, int parentId = 0)
    Q_INVOKABLE QStringList getParentTagNames()
```

### 예제

```js
// "QownNotes 가져오기"를 사용하는 것을 잊지 마십시오. 스크립트 상단에 1.0"을 입력합니다!

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

TagApi가 [ note-tagging-by-object.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml)에서 사용되는 더 많은 예를 찾을 수 있습니다.

## 기본창

### 속성 및 방법

```cpp
클래스 MainWindow {
    Q_INVOKABLE void reloadTagTree();
    Q_INVOKABLE void reloadNoteSubFolderTree();
    Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList(
            bool forceBuild = false, bool forceLoad = false);
    Q_INVOKABLE void focusNoteTextEdit();
    // 현재 하위 폴더에 새 노트 하위 폴더를 만듭니다
    Q_INVOKABLE bool createNewNoteSubFolder(QString folderName = "");
    // 현재 노트에 html을 마크다운으로 삽입합니다
    // 이 방법은 또한 원격 이미지를 다운로드하고 "data:image"  URL을 미디어
    // 디렉토리에저장된 로컬 이미지로 변환합니다
    Q_INVOKABLE void insertHtmlAsMarkdownIntoCurrentNote(QString html);
    // ID별로 현재 노트를 다시 불러오기
    // 현재 노트의 경로 또는 파일 이름이 변경될 때 유용합니다
    Q_INVOKABLE void reloadCurrentNoteByNoteId();
    // 작업 공간 UUID 목록을 반환합니다
    Q_INVOKABLE QStringList getWorkspaceUuidList();
    // 작업 공간 이름을 전달하는 워크스페이스의 UUID를 반환합니다
    Q_INVOKABLE QString getWorkspaceUuid(const QString &workspaceName);
    // UUID별로 현재 작업 공간을 설정합니다
    Q_INVOKABLE void setCurrentWorkspace(const QString &uuid);
    // 특정 인덱스에서 노트 탭을 닫습니다 (성공한 경우 true를 반환합니다)
    Q_INVOKABLE bool removeNoteTab(int index);
    // 탭에서 열려 있는 노트 ID 목록을 반환합니다
    Q_INVOKABLE QList<int> getNoteTabNoteIdList();
    // 태그 트리의 태그로 이동합니다
    Q_INVOKABLE bool jumpToTag(int tagId);
};;
```

### 예제

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
