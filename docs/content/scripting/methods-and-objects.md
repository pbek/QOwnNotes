# Methods and objects QOwnNotes provides

Starting an external program in the background
----------------------------------------------


!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to start a detached process
     *
     * @param executablePath the path of the executable
     * @param parameters a list of parameter strings
     * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function (optional)
     * @param callbackParameter an additional parameter for loops or the like (optional)
     * @param processData data written to the process if the callback is used (optional)
     * @return true on success, false otherwise
     */
    bool startDetachedProcess(QString executablePath, QStringList parameters,
                              QString callbackIdentifier, QVariant callbackParameter,
                              QByteArray processData);
    ```

!!! example
    ```js
    script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
    ```
    ```js
    for (var i = 0; i < 100; i++) {
        var dur = Math.floor(Math.random() * 10) + 1;
        script.startDetachedProcess("sleep", [`${dur}s`], "my-callback", i);
    }

    function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread) {
        if (callbackIdentifier == "my-callback") {
            script.log(`#${thread[1]} i[${thread[0]}] t${cmd[1]}`);
        }
    }
    ```

You may want to take a look at the example
[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-actions.qml),
[callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/callback.qml)
or
[execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/execute-command-after-note-update.qml).

You also might want to take a look at the [onDetachedProcessCallback](../hooks/#ondetachedprocesscallback) hook.

!!! tip
    You can also assign local and global shortcuts to your custom actions
    in the *Shortcuts settings*.

Starting an external program and wait for the output
----------------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to start a synchronous process
     *
     * @param executablePath the path of the executable
     * @param parameters a list of parameter strings
     * @param data the data that will be written to the process (optional)
     * @return the text that was returned by the process
    QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data);
    ```

!!! example
    ```js
    var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
    ```

You may want to take a look at the example
[encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/encryption-keybase.qml).

Getting the path of the current note folder
-------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to get the current note folder path
     *
     * @return the path of the current note folder
     */
    QString currentNoteFolderPath();
    ```

!!! example
    ```js
    var path = script.currentNoteFolderPath();
    ```

You may want to take a look at the example
[absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/absolute-media-links.qml).

Getting the current note
------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to get the current note
     *
     * @returns {NoteApi} the the current note object
     */
    NoteApi currentNote();
    ```

!!! example
    ```js
    var note = script.currentNote();
    ```

You may want to take a look at the example
[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-actions.qml).

Logging to the log widget
-------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to log to the log widget
     *
     * @param text
     */
    void log(QString text);
    ```

!!! example
    ```js
    script.log("my text");
    ```

Downloading an url to a string
------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to download an url and returning it as text
     *
     * @param url
     * @return {QString} the content of the downloaded url
     */
    QString downloadUrlToString(QUrl url);
    ```

!!! example
    ```js
    var html = script.downloadUrlToString("https://www.qownnotes.org");
    ```

You may want to take a look at the example
[insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/insert-headline-with-link-from-github-url.qml).

Downloading an url to the media folder
--------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to download an url to the media folder and returning the media
     * url or the markdown image text of the media relative to the current note
     *
     * @param {QString} url
     * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
     * @return {QString} the media markdown or url
     */
    QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
    ```

!!! example
    ```js
    var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
    ```

You may want to take a look at the example
[paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/paste-latex-image.qml).

Inserting a media file into the media folder
--------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * QML wrapper to insert a media file into the media folder and returning
     * the media url or the markdown image text of the media  relative to the current note
     *
     * @param {QString} mediaFilePath
     * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
     * @return {QString} the media markdown or url
     */
    QString ScriptingService::insertMediaFile(QString mediaFilePath,
                                          bool returnUrlOnly);
    ```

!!! example
    ```js
    var markdown = script.insertMediaFile("/path/to/your/image.png");
    ```

You may want to take a look at the example
[scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/scribble.qml).

Regenerating the note preview
-----------------------------

Refreshes the note preview.

!!! help "Method call and parameters"
    ```cpp
    /**
     * Regenerates the note preview
     */
    QString ScriptingService::regenerateNotePreview();
    ```

!!! example
    ```js
    script.regenerateNotePreview();
    ```

You may want to take a look at the example
[scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/scribble.qml).

Registering a custom action
---------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Registers a custom action
     *
     * @param identifier the identifier of the action
     * @param menuText the text shown in the menu
     * @param buttonText the text shown in the button
     *                   (no button will be viewed if empty)
     * @param icon the icon file path or the name of a freedesktop theme icon
     *             you will find a list of icons here:
     *             https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
     * @param useInNoteEditContextMenu if true use the action in the note edit
     *                                 context menu (default: false)
     * @param hideButtonInToolbar if true the button will not be shown in the
     *                            custom action toolbar (default: false)
     * @param useInNoteListContextMenu if true use the action in the note list
     *                                 context menu (default: false)
     */
    void ScriptingService::registerCustomAction(QString identifier,
                                                QString menuText,
                                                QString buttonText,
                                                QString icon,
                                                bool useInNoteEditContextMenu,
                                                bool hideButtonInToolbar,
                                                bool useInNoteListContextMenu);
    ```

!!! example
    ```js
    // add a custom action without a button
    script.registerCustomAction("mycustomaction1", "Menu text");

    // add a custom action with a button
    script.registerCustomAction("mycustomaction1", "Menu text", "Button text");

    // add a custom action with a button and freedesktop theme icon
    script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "task-new");

    // add a custom action with a button and an icon from a file
    script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
    ```

You may then want to use the identifier with function
`customActionInvoked` in a script like
[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-actions.qml).

Registering a label
-------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Registers a label to write to
     *
     * @param identifier the identifier of the label
     * @param text the text shown in the label (optional)
     */
    void ScriptingService::registerLabel(QString identifier, QString text);
    ```

!!! example
    ```js
    script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

    script.registerLabel("long-label", "an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text that will wrap");

    script.registerLabel("counter-label");
    ```

The labels will be visible in the scripting dock widget.

You can use both plain text or html in the labels. The text will be
selectable and links can be clicked.

You may then want to take a look at the example script
[scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/scripting-label-demo.qml).

Setting the text of a registered label
--------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Sets the text of a registered label
     *
     * @param identifier the identifier of the label
     * @param text the text shown in the label
     */
    void ScriptingService::setLabelText(QString identifier, QString text);
    ```

!!! example
    ```js
    script.setLabelText("counter-label", "counter text");
    ```

You can use both plain text or html in the labels. The text will be
selectable and links can be clicked.

You may then want to take a look at the example script
[scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/scripting-label-demo.qml).

Creating a new note
-------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Creates a new note
     *
     * @param text the note text
     */
    void ScriptingService::createNote(QString text);
    ```

!!! example
    ```js
    script.createNote("My note headline\n===\n\nMy text");
    ```

You may want to take a look at the example
[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-actions.qml).

Accessing the clipboard
-----------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns the content of the clipboard as text or html
     *
     * @param asHtml returns the clipboard content as html instead of text
     */
    QString ScriptingService::clipboard(bool asHtml);
    ```

!!! example
    ```js
    var clipboardText = script.clipboard();
    var clipboardHtml = script.clipboard(true);
    ```

You may want to take a look at the example
[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-actions.qml).

Write text to the note text edit
--------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Writes text to the current cursor position in the note text edit
     *
     * @param text
     */
    void ScriptingService::noteTextEditWrite(QString text);
    ```

!!! example
    ```js
    // write text to the note text edit
    script.noteTextEditWrite("My custom text");
    ```

You might want to look at the custom action `transformTextRot13` in the
example
[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-actions.qml).

You can use this together with `noteTextEditSelectAll` to overwrite the
whole text of the current note.

Read the selected text in the note text edit
--------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Reads the selected text in the note text edit
     *
     * @return
     */
    QString ScriptingService::noteTextEditSelectedText();
    ```

!!! example
    ```js
    // read the selected text from the note text edit
    var text = script.noteTextEditSelectedText();
    ```

You might want to look at the custom action `transformTextRot13` in the
example
[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-actions.qml).

Select all text in the note text edit
-------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Selects all text in the note text edit
     */
    void ScriptingService::noteTextEditSelectAll();
    ```

!!! example
    ```js
    script.noteTextEditSelectAll();
    ```

You can use this together with `noteTextEditWrite` to overwrite the
whole text of the current note.

Select the current line in the note text edit
---------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Selects the current line in the note text edit
     */
    void ScriptingService::noteTextEditSelectCurrentLine();
    ```

!!! example
    ```js
    script.noteTextEditSelectCurrentLine();
    ```

Select the current word in the note text edit
---------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Selects the current line in the note text edit
     */
    void ScriptingService::noteTextEditSelectCurrentWord();
    ```

!!! example
    ```js
    script.noteTextEditSelectCurrentWord();
    ```

Set the currently selected text in the note text edit
-----------------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Sets the currently selected text in the note text edit
     *
     * @param start
     * @param end
     */
    void ScriptingService::noteTextEditSetSelection(int start, int end);
    ```

!!! example
    ```js
    // expands the current selection by one character
    script.noteTextEditSetSelection(
        script.noteTextEditSelectionStart() - 1,
        script.noteTextEditSelectionEnd() + 1);
    ```

Get the start position of the current selection in the note text edit
---------------------------------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns the start position of the current selection in the note text edit
     */
    int ScriptingService::noteTextEditSelectionStart();
    ```

!!! example
    ```js
    script.log(script.noteTextEditSelectionStart());
    ```

Get the end position of the current selection in the note text edit
-------------------------------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns the end position of the current selection in the note text edit
     */
    int ScriptingService::noteTextEditSelectionEnd();
    ```

!!! example
    ```js
    script.log(script.noteTextEditSelectionEnd());
    ```

Read the current word from the note text edit
---------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Reads the current word in the note text edit
     *
     * @param withPreviousCharacters also get more characters at the beginning
     *                               to get characters like "@" that are not
     *                               word-characters
     * @return
     */
    QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
    ```

!!! example
    ```js
    // read the current word in the note text edit
    var text = script.noteTextEditCurrentWord();
    ```

You may want to take a look at the example
[autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/autocompletion.qml).

Check whether platform is Linux, OS X or Windows
------------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    bool ScriptingService::platformIsLinux();
    bool ScriptingService::platformIsOSX();
    bool ScriptingService::platformIsWindows();
    ```

!!! example
    ```js
    if (script.platformIsLinux()) {
        // only will be executed if under Linux
    }
    ```

Tag the current note
--------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Tags the current note with a tag named tagName
     *
     * @param tagName
     */
    void ScriptingService::tagCurrentNote(QString tagName);
    ```

!!! example
    ```js
    // add a "favorite" tag to the current note
    script.tagCurrentNote("favorite");
    ```

You might want to look at the custom action `favoriteNote` in the
example
[favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/favorite-note.qml).

Create or fetch a tag by its name breadcrumb list
-------------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Fetches or creates a tag by its "breadcrumb list" of tag names
     * Element nameList[0] would be highest in the tree (with parentId: 0)
     *
     * @param nameList
     * @param createMissing {bool} if true (default) all missing tags will be created
     * @return TagApi object of deepest tag of the name breadcrumb list
     */
    TagApi *ScriptingService::getTagByNameBreadcrumbList(
        const QStringList &nameList, bool createMissing);
    ```

!!! example
    ```js
    // creates all tags until the 3rd level and returns the tag object for
    // tag "level3", which would look like that in the tag tree:
    // level1 > level2 > level3
    var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
    ```

Search for tags by name
-----------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Fetches all tags by doing a substring search on the name field
     *
     * @param name {QString} name to search for
     * @return {QStringList} list of tag names
     */
    QStringList ScriptingService::searchTagsByName(QString name);
    ```

!!! example
    ```js
    // searches for all tags with the word game in it
    var tags = script.searchTagsByName("game");
    ```

You may want to take a look at the example
[autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/autocompletion.qml).

Search for notes by note text
-----------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns a list of note ids of all notes with a certain text in the note text
     *
     * Unfortunately there is no easy way to use a QList<NoteApi*> in QML, so we
     * can only transfer the note ids
     *
     * @return {QList<int>} list of note ids
     */
    QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);
    ```

!!! example
    ```js
    var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

    noteIds.forEach(function (noteId){
        var note = script.fetchNoteById(noteId);

        // do something with the note
    });
    ```

You may want to take a look at the example
[unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/unique-note-id.qml).

Add a custom stylesheet
-----------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Adds a custom stylesheet to the application
     *
     * @param stylesheet
     */
    void ScriptingService::addStyleSheet(QString stylesheet);
    ```

!!! example
    ```js
    // make the text in the note list bigger
    script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
    ```

You may want to take a look at the example
[custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/custom-stylesheet.qml).

You can get the object names from the `*.ui` files, for example
[mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Take a look at [Style Sheet
Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a
reference of what styles are available.

If you want to inject styles into html preview to alter the way notes
are previewed please look at
[notetomarkdownhtmlhook](../hooks/#notetomarkdownhtmlhook).

Reloading the scripting engine
------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Reloads the scripting engine
     */
    void ScriptingService::reloadScriptingEngine();
    ```

!!! example
    ```js
    // reload the scripting engine
    script.reloadScriptingEngine();
    ```

Fetching a note by its file name
--------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Fetches a note by its file name
     *
     * @param fileName string the file name of the note (mandatory)
     * @param noteSubFolderId integer id of the note subfolder
     * @return NoteApi*
     */
    NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                   int noteSubFolderId);
    ```

!!! example
    ```js
    // fetch note by file name
    script.fetchNoteByFileName("my note.md");
    ```

Fetching a note by its id
-------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Fetches a note by its id
     *
     * @param id int the id of the note
     * @return NoteApi*
     */
    NoteApi* ScriptingService::fetchNoteById(int id);
    ```

!!! example
    ```js
    // fetch note by id
    script.fetchNoteById(243);
    ```

You may want to take a look at the example
[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/export-notes-as-one-html.qml).

Checking if a note exists by its file name
------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Checks if a note file exists by its file name
     *
     * @param fileName string the file name of the note (mandatory)
     * @param ignoreNoteId integer id of a note to ignore in the check
     * @param noteSubFolderId integer id of the note subfolder
     * @return bool
     */
    bool ScriptingService::noteExistsByFileName(QString fileName,
                                                int ignoreNoteId,
                                                int noteSubFolderId);
    ```

!!! example
    ```js
    // check if note exists, but ignore the id of "note"
    script.noteExistsByFileName("my note.md", note.id);
    ```

You may want to take a look at the example
[use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/use-tag-names-in-filename.qml).

Copying text into the clipboard
-------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Copies text into the clipboard as plain text or html mime data
     *
     * @param text string text to put into the clipboard
     * @param asHtml bool if true the text will be set as html mime data
     */
    void ScriptingService::setClipboardText(QString text, bool asHtml);
    ```

!!! example
    ```js
    // copy text to the clipboard
    script.setClipboardText("text to copy");
    ```

You may want to take a look at the example
[selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/selected-markdown-to-bbcode.qml).

Jumping to a note
-----------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Sets the current note if the note is visible in the note list
     *
     * @param note NoteApi note to jump to
     */
    void ScriptingService::setCurrentNote(NoteApi *note);
    ```

!!! example
    ```js
    // jump to the note
    script.setCurrentNote(note);
    ```

You may want to take a look at the example
[journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/journal-entry.qml).

Jumping to a note subfolder
---------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Jumps to a note subfolder
     *
     * @param noteSubFolderPath {QString} path of the subfolder, relative to the note folder
     * @param separator {QString} separator between parts of the path, default "/"
     * @return true if jump was successful
     */
    bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                               QString separator);
    ```

!!! example
    ```js
    // jump to the note subfolder "a sub folder"
    script.jumpToNoteSubFolder("a sub folder");

    // jump to the note subfolder "sub" inside of "a sub folder"
    script.jumpToNoteSubFolder("a sub folder/sub");
    ```

Showing an information message box
----------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Shows an information message box
     *
     * @param text
     * @param title (optional)
     */
    void ScriptingService::informationMessageBox(QString text, QString title);
    ```

!!! example
    ```js
    // show a information message box
    script.informationMessageBox("The text I want to show", "Some optional title");
    ```

Showing a question message box
------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Shows a question message box
     *
     * For information about buttons see:
     * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
     *
     * @param text
     * @param title (optional)
     * @param buttons buttons that should be shown (optional)
     * @param defaultButton default button that will be selected (optional)
     * @return id of pressed button
     */
    int ScriptingService::questionMessageBox(
            QString text, QString title, int buttons, int defaultButton);
    ```

!!! example
    ```js
    // show a question message box with an apply and a help button
    // see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
    var result = script.questionMessageBox(
        "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
    script.log(result);
    ```

For information about buttons see
[StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

You may also want to take a look at the example
[input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/input-dialogs.qml).

Showing an open file dialog
---------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Shows an open file dialog
     *
     * @param caption (optional)
     * @param dir (optional)
     * @param filter (optional)
     * @return QString
     */
    QString ScriptingService::getOpenFileName(QString caption, QString dir,
                                              QString filter);
    ```

!!! example
    ```js
    // show an open file dialog
    var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
    ```

Showing a save file dialog
--------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Shows a save file dialog
     *
     * @param caption (optional)
     * @param dir (optional)
     * @param filter (optional)
     * @return QString
     */
    QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                              QString filter);
    ```

!!! example
    ```js
    // show a save file dialog
    var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
    ```

You may want to take a look at the example
[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/export-notes-as-one-html.qml).

Registering script settings variables
-------------------------------------

You need to define properties in your script and register them in an
further property named `settingsVariables`.

The user can then set these properties in the script settings.

!!! example
    ```js
    // you have to define your registered variables so you can access them later
    property string myString;
    property bool myBoolean;
    property string myText;
    property int myInt;
    property string myFile;
    property string mySelection;

    // register your settings variables so the user can set them in the script settings
    // use this property if you don't need
    //
    // unfortunately there is no QVariantHash in Qt, we only can use
    // QVariantMap (that has no arbitrary ordering) or QVariantList (which at
    // least can be ordered arbitrarily)
    property variant settingsVariables: [
        {
            "identifier": "myString",
            "name": "I am a line edit",
            "description": "Please enter a valid string:",
            "type": "string",
            "default": "My default value",
        },
        {
            "identifier": "myBoolean",
            "name": "I am a checkbox",
            "description": "Some description",
            "text": "Check this checkbox",
            "type": "boolean",
            "default": true,
        },
        {
            "identifier": "myText",
            "name": "I am textbox",
            "description": "Please enter your text:",
            "type": "text",
            "default": "This can be a really long text\nwith multiple lines.",
        },
        {
            "identifier": "myInt",
            "name": "I am a number selector",
            "description": "Please enter a number:",
            "type": "integer",
            "default": 42,
        },
        {
            "identifier": "myFile",
            "name": "I am a file selector",
            "description": "Please select the file:",
            "type": "file",
            "default": "pandoc",
        },
        {
            "identifier": "mySelection",
            "name": "I am an item selector",
            "description": "Please select an item:",
            "type": "selection",
            "default": "option2",
            "items": {"option1": "Text for option 1", "option2": "Text for option 2", "option3": "Text for option 3"},
        }
    ];
    ```

In addition you can override the `settingsVariables` with a special
function `registerSettingsVariables()` like this:

!!! example
    ```js
    /**
     * Registers the settings variables again
     *
     * Use this method if you want to use code to override your variables, like setting
     * default values depended on the operating system.
     */
    function registerSettingsVariables() {
        if (script.platformIsWindows()) {
            // override the myFile default value
            settingsVariables[3].default = "pandoc.exe"
        }
    }
    ```

You may also want to take a look at the example
[variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/variables.qml).

Storing and loading persistent variables
----------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Stores a persistent variable
     * These variables are accessible globally over all scripts
     * Please use a meaningful prefix in your key like "PersistentVariablesTest/myVar"
     *
     * @param key {QString}
     * @param value {QVariant}
     */
    void ScriptingService::setPersistentVariable(const QString &key,
                                                 const QVariant &value);

    /**
     * Loads a persistent variable
     * These variables are accessible globally over all scripts
     *
     * @param key {QString}
     * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
     * @return
     */
    QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                     const QVariant &defaultValue);
    ```

!!! example
    ```js
    // store persistent variable
    script.setPersistentVariable("PersistentVariablesTest/myVar", result);

    // load and log persistent variable
    script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
    ```

Please make sure to use a meaningful prefix in your key like
`PersistentVariablesTest/myVar` because the variables are accessible
from all scripts.

You may also want to take a look at the example
[persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/persistent-variables.qml).

Loading application settings variables
--------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Loads an application settings variable
     *
     * @param key {QString}
     * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
     * @return
     */
    QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                              const QVariant &defaultValue);
    ```

!!! example
    ```js
    // load and log an application settings variable
    script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
    ```

Keep in mind that settings actually can be empty, you have to take care
about that yourself. `defaultValue` is only used if the setting doesn't
exist at all.

Creating a cache directory
--------------------------

You can cache files at the default cache location of your system.

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns a cache directory for a script
     *
     * @param {QString} subDir the subfolder to create and use
     * @return {QString} the cache dir path
     */
    QString ScriptingService::cacheDir(const QString &subDir) const;
    ```

!!! example
    ```js
    // create the cache directory for my-script-id
    var cacheDirForScript = script.cacheDir("my-script-id");
    ```

Clearing a cache directory
--------------------------

You can clear the cache files of your script by passing its name to clearCacheDir().

!!! help "Method call and parameters"
    ```cpp
    /**
     * Clears the cache directory for a script
     *
     * @param {QString} subDir the subfolder to clear
     * @return {bool} true on success
     */
    bool ScriptingService::clearCacheDir(const QString &subDir) const;
    ```

!!! example
    ```js
    // clear cache directory of my-script-id 
    script.clearCacheDir("my-script-id");
    ```

Reading the path to the directory of your script
------------------------------------------------

If you need to get the path to the directory where your script is placed
to for example load other files you have to register a
`property string scriptDirPath;`. This property will be set with the
path to the script's directory.

!!! example
    ```js
    import QtQml 2.0
    import QOwnNotesTypes 1.0

    Script {
        // the path to the script's directory will be set here
        property string scriptDirPath;

        function init() {
            script.log(scriptDirPath);
        }
    }
    ```

Converting path separators to native ones
-----------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns path with the '/' separators converted to separators that are
     * appropriate for the underlying operating system.
     *
     * On Windows, toNativeDirSeparators("c:/winnt/system32") returns
     * "c:\winnt\system32".
     *
     * @param path
     * @return
     */
    QString ScriptingService::toNativeDirSeparators(QString path);
    ```

!!! example
    ```js
    // will return "c:\winnt\system32" on Windows
    script.log(script.toNativeDirSeparators("c:/winnt/system32"));
    ```

Converting path separators from native ones
-------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns path using '/' as file separator.
     * On Windows, for instance, fromNativeDirSeparators("c:\\winnt\\system32")
     * returns "c:/winnt/system32".
     *
     * @param path
     * @return
     */
    QString ScriptingService::fromNativeDirSeparators(QString path);
    ```

!!! example
    ```js
    // will return "c:/winnt/system32" on Windows
    script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
    ```

Getting the native directory separator
--------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns the native directory separator "/" or "\" on Windows
     *
     * @return
     */
    QString ScriptingService::dirSeparator();
    ```

!!! example
    ```js
    // will return "\" on Windows
    script.log(script.dirSeparator());
    ```

Getting a list of the paths of all selected notes
-------------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns a list of the paths of all selected notes
     *
     * @return {QStringList} list of selected note paths
     */
    QStringList ScriptingService::selectedNotesPaths();
    ```

!!! example
    ```js
    // returns a list of the paths of all selected notes
    script.log(script.selectedNotesPaths());
    ```

You may want to take a look at the example
[external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/external-note-diff.qml).

Getting a list of the ids of all selected notes
-----------------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Returns a list of the ids of all selected notes
     *
     * @return {QList<int>} list of selected note ids
     */
    QList<int> ScriptingService::selectedNotesIds();
    ```

!!! example
    ```js
    // returns a list of the ids of all selected notes
    script.log(script.selectedNotesIds());
    ```

You may want to take a look at the example
[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/export-notes-as-one-html.qml).

Triggering a menu action
------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Triggers a menu action
     *
     * @param objectName {QString} object name of the action to trigger
     * @param checked {QString} only trigger the action if checked-state is
     *                          different than this parameter (optional, can be 0 or 1)
     */
    void ScriptingService::triggerMenuAction(QString objectName, QString checked);
    ```

!!! example
    ```js
    // toggle the read-only mode
    script.triggerMenuAction("actionAllow_note_editing");

    // disable the read-only mode
    script.triggerMenuAction("actionAllow_note_editing", 1);
    ```

You may want to take a look at the example
[disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/disable-readonly-mode.qml).

You can get the object names of the menu action from
[mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Opening an input dialog with a select box
-----------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Opens an input dialog with a select box
     *
     * @param title {QString} title of the dialog
     * @param label {QString} label text of the dialog
     * @param items {QStringList} list of items to select
     * @param current {int} index of the item that should be selected (default: 0)
     * @param editable {bool} if true the text in the dialog can be edited (default: false)
     * @return {QString} text of the selected item
     */
    QString ScriptingService::inputDialogGetItem(
            const QString &title, const QString &label, const QStringList &items,
            int current, bool editable);
    ```

!!! example
    ```js
    var result = script.inputDialogGetItem(
        "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
    script.log(result);
    ```

You may want to take a look at the example
[input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/input-dialogs.qml).

Opening an input dialog with a line edit
----------------------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Opens an input dialog with a line edit
     *
     * @param title {QString} title of the dialog
     * @param label {QString} label text of the dialog
     * @param text {QString} text in the dialog (optional)
     * @return
     */
    QString ScriptingService::inputDialogGetText(
            const QString &title, const QString &label, const QString &text);
    ```

!!! example
    ```js
    var result = script.inputDialogGetText(
        "line edit", "Please enter a name", "current text");
    script.log(result);
    ```

Checking if a file exists
-------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Check if a file exists
     * @param filePath
     * @return
     */
    bool ScriptingService::fileExists(QString &filePath);
    ```

!!! example
    ```js
    var result = script.fileExists(filePath);
    script.log(result);
    ```

Reading text from a file
------------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Read text from a file
     *
     * @param filePath {QString} path of the file to load
     * @param codec {QString} file encoding (default: UTF-8)
     * @return the file data or null if the file does not exist
     */
    QString ScriptingService::readFromFile(const QString &filePath, const QString &codec)
    ```

!!! example
    ```js
    if(script.fileExists(filePath)){
        var data = script.readFromFile(filePath);
        script.log(data);
    }
    ```


Writing text to a file
----------------------

!!! help "Method call and parameters"
    ```cpp
    /**
     * Writes a text to a file
     *
     * @param filePath {QString}
     * @param data {QString}
     * @param createParentDirs {bool} optional (default: false)
     * @return
     */
    bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
    ```

!!! example
    ```js
    var result = script.writeToFile(filePath, html);
    script.log(result);
    ```

You may want to take a look at the example
[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/export-notes-as-one-html.qml).

Working with websockets
-----------------------

You can remote control QOwnNotes by using `WebSocketServer`.

Please take a look at the example
[websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/websocket-server.qml).
You can test the socket server by connecting to it on [Websocket
test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

You can also listen to sockets with `WebSocket`. Please take look at the
example
[websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library
installed to use this. For example under Ubuntu Linux you can install
`qml-module-qtwebsockets`.
