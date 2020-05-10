# Exposed classes

Note
----

!!! help "Properties and methods"
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

You can use the methods from
[Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date)
to work with `fileCreated` or `fileLastModified`.

!!! example
    ```js
    script.log(note.fileCreated.toISOString());
    script.log(note.fileLastModified.getFullYear());

    // renames a note to "new name.md"
    note.renameNoteFile("new name");

    // check if it is allowed to have a different note file name than the headline
    script.log(note.allowDifferentFileName());
    ```

Tag
---

!!! help "Properties and methods"
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

!!! help "Properties and methods"
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
    };
    ```

!!! example
    ```js
    // Force a reload of the note list
    mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);
    
    // Creates a new note subfolder "My fancy folder" in the current subfolder
    mainWindow.createNewNoteSubFolder("My fancy folder");
    
    // Inserts html in the current note as markdown
    mainWindow.insertHtmlAsMarkdownIntoCurrentNote("<h2>my headline</h2>some text");
    ```
