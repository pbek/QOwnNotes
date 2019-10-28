QOwnNotes Scripting
===================

-  QOwnNotes scripts consist basically of JavaScript embedded in QML files.
-  Take a look at the `example scripts <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting>`__
   to get started fast.
-  If you need access to a certain functionality in QOwnNotes or have
   questions or ideas please open an issue on the `QOwnNotes issue
   page <https://github.com/pbek/QOwnNotes/issues>`__.
-  Since debug output is disabled in the releases of QOwnNotes, so you
   might want to use ``console.warn()`` instead of ``console.log()`` to
   actually see an output.

   -  Additionally you can also use the ``script.log()`` command to log
      to the log widget.

Methods and objects QOwnNotes provides
--------------------------------------

Starting an external program in the background
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * QML wrapper to start a detached process
     *
     * @param executablePath the path of the executable
     * @param parameters a list of parameter strings
     * @return true on success, false otherwise
     */
    bool startDetachedProcess(QString executablePath, QStringList parameters);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.startDetachedProcess("/path/to/my/program", ["my parameter"]);

You may want to take a look at the example
`custom-actions.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-actions.qml>`__ or
`execute-command-after-note-update.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/execute-command-after-note-update.qml>`__.

.. code:: cpp

    /**
     * QML wrapper to start a synchronous process
     *
     * @param executablePath the path of the executable
     * @param parameters a list of parameter strings
     * @param data the data that will be written to the process (optional)
     * @return the text that was returned by the process
    QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data);


Starting an external program and wait for the output
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");

You may want to take a look at the example
`encryption-keybase.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml>`__.


Getting the path of the current note folder
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * QML wrapper to get the current note folder path
     *
     * @return the path of the current note folder
     */
    QString currentNoteFolderPath();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var path = script.currentNoteFolderPath();

You may want to take a look at the example
`absolute-media-links.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/absolute-media-links.qml>`__.


Getting the current note
~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * QML wrapper to get the current note
     *
     * @returns {NoteApi} the the current note object
     */
    NoteApi currentNote();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var note = script.currentNote();

You may want to take a look at the example
`custom-actions.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-actions.qml>`__.


Logging to the log widget
~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * QML wrapper to log to the log widget
     *
     * @param text
     */
    void log(QString text);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.log("my text");


Downloading an url to a string
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * QML wrapper to download an url and returning it as text
     *
     * @param url
     * @return {QString} the content of the downloaded url
     */
    QString downloadUrlToString(QUrl url);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var html = script.downloadUrlToString("https://www.qownnotes.org");

You may want to take a look at the example
`insert-headline-with-link-from-github-url.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/insert-headline-with-link-from-github-url.qml>`__.


Downloading an url to the media folder
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * QML wrapper to download an url to the media folder and returning the media
     * url or the markdown image text of the media relative to the current note
     *
     * @param {QString} url
     * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
     * @return {QString} the media markdown or url
     */
    QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");

You may want to take a look at the example
`paste-latex-image.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/paste-latex-image.qml>`__.


Inserting a media file into the media folder
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

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

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var markdown = script.insertMediaFile("/path/to/your/image.png");

You may want to take a look at the example
`scribble.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/scribble.qml>`__.


Regenerating the note preview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Refreshes the note preview.

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Regenerates the note preview
     */
    QString ScriptingService::regenerateNotePreview();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.regenerateNotePreview();

You may want to take a look at the example
`scribble.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/scribble.qml>`__.


Registering a custom action
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

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

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // add a custom action without a button
    script.registerCustomAction("mycustomaction1", "Menu text");

    // add a custom action with a button
    script.registerCustomAction("mycustomaction1", "Menu text", "Button text");

    // add a custom action with a button and freedesktop theme icon
    script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "task-new");

    // add a custom action with a button and an icon from a file
    script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");

You may then want to use the identifier with function
``customActionInvoked`` in a script like
`custom-actions.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-actions.qml>`__.


Registering a label
~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Registers a label to write to
     *
     * @param identifier the identifier of the label
     * @param text the text shown in the label (optional)
     */
    void ScriptingService::registerLabel(QString identifier, QString text);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

    script.registerLabel("long-label", "an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text, an other very long text that will wrap");

    script.registerLabel("counter-label");

The labels will be visible in the scripting dock widget.

You can use both plain text or html in the labels. The text will be
selectable and links can be clicked.

You may then want to take a look at the example script
`scripting-label-demo.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/scripting-label-demo.qml>`__.


Setting the text of a registered label
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Sets the text of a registered label
     *
     * @param identifier the identifier of the label
     * @param text the text shown in the label
     */
    void ScriptingService::setLabelText(QString identifier, QString text);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.setLabelText("counter-label", "counter text");

You can use both plain text or html in the labels. The text will be
selectable and links can be clicked.

You may then want to take a look at the example script
`scripting-label-demo.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/scripting-label-demo.qml>`__.


Creating a new note
~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Creates a new note
     *
     * @param text the note text
     */
    void ScriptingService::createNote(QString text);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.createNote("My note headline\n===\n\nMy text");

You may want to take a look at the example
`custom-actions.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-actions.qml>`__.

Accessing the clipboard
~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns the content of the clipboard as text or html
     *
     * @param asHtml returns the clipboard content as html instead of text
     */
    QString ScriptingService::clipboard(bool asHtml);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var clipboardText = script.clipboard();
    var clipboardHtml = script.clipboard(true);

You may want to take a look at the example
`custom-actions.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-actions.qml>`__.

Write text to the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Writes text to the current cursor position in the note text edit
     *
     * @param text
     */
    void ScriptingService::noteTextEditWrite(QString text);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // write text to the note text edit
    script.noteTextEditWrite("My custom text");

You might want to look at the custom action ``transformTextRot13`` in
the example `custom-actions.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-actions.qml>`__.

You can use this together with ``noteTextEditSelectAll`` to overwrite the whole
text of the current note.

Read the selected text in the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Reads the selected text in the note text edit
     *
     * @return
     */
    QString ScriptingService::noteTextEditSelectedText();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // read the selected text from the note text edit
    var text = script.noteTextEditSelectedText();

You might want to look at the custom action ``transformTextRot13`` in
the example `custom-actions.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-actions.qml>`__.

Select all text in the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Selects all text in the note text edit
     */
    void ScriptingService::noteTextEditSelectAll();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.noteTextEditSelectAll();

You can use this together with ``noteTextEditWrite`` to overwrite the whole
text of the current note.

Select the current line in the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Selects the current line in the note text edit
     */
    void ScriptingService::noteTextEditSelectCurrentLine();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.noteTextEditSelectCurrentLine();

Set the currently selected text in the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Sets the currently selected text in the note text edit
     *
     * @param start
     * @param end
     */
    void ScriptingService::noteTextEditSetSelection(int start, int end);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // expands the current selection by one character
    script.noteTextEditSetSelection(
        script.noteTextEditSelectionStart() - 1,
        script.noteTextEditSelectionEnd() + 1);

Get the start position of the current selection in the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns the start position of the current selection in the note text edit
     */
    int ScriptingService::noteTextEditSelectionStart();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.log(script.noteTextEditSelectionStart());

Get the end position of the current selection in the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns the end position of the current selection in the note text edit
     */
    int ScriptingService::noteTextEditSelectionEnd();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    script.log(script.noteTextEditSelectionEnd());

Read the current word from the note text edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Reads the current word in the note text edit
     *
     * @param withPreviousCharacters also get more characters at the beginning
     *                               to get characters like "@" that are not
     *                               word-characters
     * @return
     */
    QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // read the current word in the note text edit
    var text = script.noteTextEditCurrentWord();

You may want to take a look at the example
`autocompletion.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/autocompletion.qml>`__.

Check whether platform is Linux, OS X or Windows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    bool ScriptingService::platformIsLinux();
    bool ScriptingService::platformIsOSX();
    bool ScriptingService::platformIsWindows();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    if (script.platformIsLinux()) {
        // only will be executed if under Linux
    }

Tag the current note
~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Tags the current note with a tag named tagName
     *
     * @param tagName
     */
    void ScriptingService::tagCurrentNote(QString tagName);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // add a "favorite" tag to the current note
    script.tagCurrentNote("favorite");

You might want to look at the custom action ``favoriteNote`` in the
example `favorite-note.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/favorite-note.qml>`__.

Search for tags by name
~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Fetches all tags by doing a substring search on the name field
     *
     * @param name {QString} name to search for
     * @return {QStringList} list of tag names
     */
    QStringList ScriptingService::searchTagsByName(QString name);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // searches for all tags with the word game in it
    var tags = script.searchTagsByName("game");

You may want to take a look at the example
`autocompletion.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/autocompletion.qml>`__.


Search for notes by note text
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns a list of note ids of all notes with a certain text in the note text
     *
     * Unfortunately there is no easy way to use a QList<NoteApi*> in QML, so we
     * can only transfer the note ids
     *
     * @return {QList<int>} list of note ids
     */
    QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

    noteIds.forEach(function (noteId){
        var note = script.fetchNoteById(noteId);

        // do something with the note
    });

You may want to take a look at the example
`unique-note-id.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/unique-note-id.qml>`__.


Add a custom stylesheet
~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Adds a custom stylesheet to the application
     *
     * @param stylesheet
     */
    void ScriptingService::addStyleSheet(QString stylesheet);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // make the text in the note list bigger
    script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");

You may want to take a look at the example
`custom-stylesheet.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-stylesheet.qml>`__.

You can get the object names from the ``*.ui`` files, for example
`mainwindow.ui <https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui>`__.

Take a look at `Style Sheet
Reference <http://doc.qt.io/qt-5/stylesheet-reference.html>`__ for a
reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at `notetomarkdownhtmlhook <#notetomarkdownhtmlhook>`__.


Reloading the scripting engine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Reloads the scripting engine
     */
    void ScriptingService::reloadScriptingEngine();

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // reload the scripting engine
    script.reloadScriptingEngine();


Fetching a note by its file name
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Fetches a note by its file name
     *
     * @param fileName string the file name of the note (mandatory)
     * @param noteSubFolderId integer id of the note subfolder
     * @return NoteApi*
     */
    NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                   int noteSubFolderId);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // fetch note by file name
    script.fetchNoteByFileName("my note.md");


Fetching a note by its id
~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Fetches a note by its id
     *
     * @param id int the id of the note
     * @return NoteApi*
     */
    NoteApi* ScriptingService::fetchNoteById(int id);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // fetch note by id
    script.fetchNoteById(243);

You may want to take a look at the example
`export-notes-as-one-html.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/export-notes-as-one-html.qml>`__.


Checking if a note exists by its file name
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

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

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // check if note exists, but ignore the id of "note"
    script.noteExistsByFileName("my note.md", note.id);

You may want to take a look at the example
`use-tag-names-in-filename.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/use-tag-names-in-filename.qml>`__.


Copying text into the clipboard
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Copies text into the clipboard as plain text or html mime data
     *
     * @param text string text to put into the clipboard
     * @param asHtml bool if true the text will be set as html mime data
     */
    void ScriptingService::setClipboardText(QString text, bool asHtml);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // copy text to the clipboard
    script.setClipboardText("text to copy");

You may want to take a look at the example
`selected-markdown-to-bbcode.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/selected-markdown-to-bbcode.qml>`__.


Jumping to a note
~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Sets the current note if the note is visible in the note list
     *
     * @param note NoteApi note to jump to
     */
    void ScriptingService::setCurrentNote(NoteApi *note);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // jump to the note
    script.setCurrentNote(note);

You may want to take a look at the example
`journal-entry.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/journal-entry.qml>`__.


Jumping to a note subfolder
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Jumps to a note subfolder
     *
     * @param noteSubFolderPath {QString} path of the subfolder, relative to the note folder
     * @param separator {QString} separator between parts of the path, default "/"
     * @return true if jump was successful
     */
    bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                               QString separator);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // jump to the note subfolder "a sub folder"
    script.jumpToNoteSubFolder("a sub folder");

    // jump to the note subfolder "sub" inside of "a sub folder"
    script.jumpToNoteSubFolder("a sub folder/sub");


Showing an information message box
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

    /**
     * Shows an information message box
     *
     * @param text
     * @param title (optional)
     */
    void ScriptingService::informationMessageBox(QString text, QString title);


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // show a information message box
    script.informationMessageBox("The text I want to show", "Some optional title");


Showing a question message box
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

.. code:: cpp

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


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // show a question message box with an apply and a help button
    // see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
    var result = script.questionMessageBox(
        "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
    script.log(result);

For information about buttons see
`StandardButton <https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum>`__.

You may also want to take a look at the example
`input-dialogs.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/input-dialogs.qml>`__.


Showing an open file dialog
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

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

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // show an open file dialog
    var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");


Showing a save file dialog
~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

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

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // show a save file dialog
    var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");

You may want to take a look at the example
`export-notes-as-one-html.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/export-notes-as-one-html.qml>`__.


Registering script settings variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You need to define properties in your script and register them in an further
property named ``settingsVariables``.

The user can then set these properties in the script settings.

.. code:: javascript

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

In addition you can override the ``settingsVariables`` with a special function
``registerSettingsVariables()`` like this:

.. code:: javascript

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

You may also want to take a look at the example
`variables.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/variables.qml>`__.


Storing and loading persistent variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

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

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // store persistent variable
    script.setPersistentVariable("PersistentVariablesTest/myVar", result);

    // load and log persistent variable
    script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));

Please make sure to use a meaningful prefix in your key like
``PersistentVariablesTest/myVar`` because the variables are accessible from
all scripts.

You may also want to take a look at the example
`persistent-variables.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/persistent-variables.qml>`__.


Loading application settings variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

    /**
     * Loads an application settings variable
     *
     * @param key {QString}
     * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
     * @return
     */
    QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                              const QVariant &defaultValue);

Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // load and log an application settings variable
    script.log(script.getApplicationSettingsVariable("gitExecutablePath"));

Keep in mind that settings actually can be empty, you have to take care about
that yourself. ``defaultValue`` is only used if the setting doesn't exist at all.


Reading the path to the directory of your script
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you need to get the path to the directory where your script is placed to
for example load other files you have to register a
``property string scriptDirPath;``. This property will be set with the path
to the script's directory.

Example
^^^^^^^

.. code:: javascript

    import QtQml 2.0
    import QOwnNotesTypes 1.0

    Script {
        // the path to the script's directory will be set here
        property string scriptDirPath;

        function init() {
            script.log(scriptDirPath);
        }
    }


Converting path separators to native ones
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

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


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // will return "c:\winnt\system32" on Windows
    script.log(script.toNativeDirSeparators("c:/winnt/system32"));


Converting path separators from native ones
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns path using '/' as file separator.
     * On Windows, for instance, fromNativeDirSeparators("c:\\winnt\\system32")
     * returns "c:/winnt/system32".
     *
     * @param path
     * @return
     */
    QString ScriptingService::fromNativeDirSeparators(QString path);


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // will return "c:/winnt/system32" on Windows
    script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));


Getting the native directory separator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns the native directory separator "/" or "\" on Windows
     *
     * @return
     */
    QString ScriptingService::dirSeparator();


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // will return "\" on Windows
    script.log(script.dirSeparator());


Getting a list of the paths of all selected notes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns a list of the paths of all selected notes
     *
     * @return {QStringList} list of selected note paths
     */
    QStringList ScriptingService::selectedNotesPaths();


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // returns a list of the paths of all selected notes
    script.log(script.selectedNotesPaths());

You may want to take a look at the example
`external-note-diff.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/external-note-diff.qml>`__.


Getting a list of the ids of all selected notes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

    /**
     * Returns a list of the ids of all selected notes
     *
     * @return {QList<int>} list of selected note ids
     */
    QList<int> ScriptingService::selectedNotesIds();


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // returns a list of the ids of all selected notes
    script.log(script.selectedNotesIds());

You may want to take a look at the example
`export-notes-as-one-html.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/export-notes-as-one-html.qml>`__.


Triggering a menu action
~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

    /**
     * Triggers a menu action
     *
     * @param objectName {QString} object name of the action to trigger
     * @param checked {QString} only trigger the action if checked-state is
     *                          different than this parameter (optional, can be 0 or 1)
     */
    void ScriptingService::triggerMenuAction(QString objectName, QString checked);


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    // toggle the read-only mode
    script.triggerMenuAction("actionAllow_note_editing");

    // disable the read-only mode
    script.triggerMenuAction("actionAllow_note_editing", 1);


You may want to take a look at the example
`disable-readonly-mode.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/disable-readonly-mode.qml>`__.

You can get the object names of the menu action from
`mainwindow.ui <https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui>`__.


Opening an input dialog with a select box
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

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


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var result = script.inputDialogGetItem(
        "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
    script.log(result);

You may want to take a look at the example
`input-dialogs.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/input-dialogs.qml>`__.


Opening an input dialog with a line edit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

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


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var result = script.inputDialogGetText(
        "line edit", "Please enter a name", "current text");
    script.log(result);


Writing text to a file
~~~~~~~~~~~~~~~~~~~~~~

Properties
^^^^^^^^^^

.. code:: cpp

    /**
     * Writes a text to a file
     *
     * @param filePath
     * @param data
     * @return
     */
    bool ScriptingService::writeToFile(const QString &filePath, const QString &data);


Usage in QML
^^^^^^^^^^^^

.. code:: javascript

    var result = script.writeToFile(filePath, html);;
    script.log(result);

You may want to take a look at the example
`export-notes-as-one-html.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/export-notes-as-one-html.qml>`__.


Working with websockets
~~~~~~~~~~~~~~~~~~~~~~~

You can remote control QOwnNotes by using ``WebSocketServer``.

Please take a look at the example
`websocket-server.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/websocket-server.qml>`__.
You can test the socket server by connecting to it on
`Websocket test <https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345>`__.

You can also listen to sockets with ``WebSocket``.
Please take look at the example
`websocket-client.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/websocket-client.qml>`__.

Keep in mind that you need to have Qt's QML ``websocket`` library installed to use this.
For example under Ubuntu Linux you can install ``qml-module-qtwebsockets``.


Hooks
-----

onNoteStored
~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called when a note gets stored to disk
     * You cannot modify stored notes, that would be a mess since
     * you are most likely editing them by hand at the same time
     *
     * @param {NoteApi} note - the note object of the stored note
     */
    function onNoteStored(note);

You may want to take a look at the example
`on-note-opened.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/on-note-opened.qml>`__.

noteOpenedHook
~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called after a note was opened
     *
     * @param {NoteApi} note - the note object that was opened
     */
    function noteOpenedHook(note);

You may want to take a look at the example
`on-note-opened.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/on-note-opened.qml>`__.

noteDoubleClickedHook
~~~~~~~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called after a note was double clicked
     *
     * @param {NoteApi} note - the note object that was clicked
     */
    function noteDoubleClickedHook(note);

You may want to take a look at the example
`external-note-open.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/external-note-open.qml>`__.

insertMediaHook
~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called when media file is inserted into the note
     * If this function is defined in multiple scripts, then the first script that returns a non-empty string wins
     *
     * @param fileName string the file path of the source media file before it was copied to the media folder
     * @param mediaMarkdownText string the markdown text of the media file, e.g. ![my-image](file://media/505671508.jpg)
     * @return string the new markdown text of the media file
     */
    function insertMediaHook(fileName, mediaMarkdownText);

You may want to take a look at the example
`example.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/example.qml>`__.

insertingFromMimeDataHook
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called when html or a media file is pasted to a note with `Ctrl + Shift + V`
     *
     * @param text text of the QMimeData object
     * @param html html of the QMimeData object
     * @returns the string that should be inserted instead of the text from the QMimeData object
     */
    function insertingFromMimeDataHook(text, html);

You may want to take a look at the example
`example.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/example.qml>`__,
`insert-headline-with-link-from-github-url.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/insert-headline-with-link-from-github-url.qml>`__
or `note-text-from-5pm-mail.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/note-text-from-5pm-mail.qml>`__.

handleNoteTextFileNameHook
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called when a note gets stored to disk if
     * "Allow note file name to be different from headline" is enabled
     * in the settings
     *
     * It allows you to modify the name of the note file
     * Keep in mind that you have to care about duplicate names yourself!
     *
     * Return an empty string if the file name of the note should
     * not be modified
     *
     * @param {NoteApi} note - the note object of the stored note
     * @return {string} the file name of the note
     */
    function handleNoteTextFileNameHook(note);

You may want to take a look at the example `example.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/example.qml>`__
or `use-tag-names-in-filename.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/use-tag-names-in-filename.qml>`__.

handleNoteNameHook
~~~~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called when the note name is determined for a note
     *
     * It allows you to modify the name of the note that is viewed
     *
     * Return an empty string if the name of the note should not be modified
     *
     * @param {NoteApi} note - the note object of the stored note
     * @return {string} the name of the note
     */
    function handleNoteNameHook(note);

You may want to take a look at the example `example.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/example.qml>`__.

It may not be a good idea to use this hook if the setting to use the file
name as note name is active.

handleNewNoteHeadlineHook
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called before a note note is created
     *
     * It allows you to modify the headline of the note before it is created
     * Note that you have to take care about a unique note name, otherwise
     * the new note will not be created, it will just be found in the note list
     *
     * You can use this method for creating note templates
     *
     * @param headline text that would be used to create the headline
     * @return {string} the headline of the note
     */
    function handleNewNoteHeadlineHook(headline);

You may want to take a look at the example
`custom-new-note-headline.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/custom-new-note-headline.qml>`__.

preNoteToMarkdownHtmlHook
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called before the markdown html of a note is generated
     *
     * It allows you to modify what is passed to the markdown to html converter 
     *
     * The method can for example be used in multiple scripts to render code (like LaTeX math or mermaid)
     * to its graphical representation for the preview
     *
     * The note will not be changed in this process
     *
     * @param {NoteApi} note - the note object
     * @param {string} markdown - the markdown that is about to being converted to html
     * @return {string} the modified markdown or an empty string if nothing should be modified
     */
    function preNoteToMarkdownHtmlHook(note, markdown);

You may want to take a look at the example `preview-styling.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/preview-styling.qml>`__.

noteToMarkdownHtmlHook
~~~~~~~~~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called when the markdown html of a note is generated
     *
     * It allows you to modify this html
     * This is for example called before by the note preview
     *
     * The method can be used in multiple scripts to modify the html of the preview
     *
     * @param {NoteApi} note - the note object
     * @param {string} html - the html that is about to being rendered
     * @return {string} the modified html or an empty string if nothing should be modified
     */
    function noteToMarkdownHtmlHook(note, html);

You may want to take a look at the example `example.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/example.qml>`__
or `preview-styling.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/preview-styling.qml>`__.

Please refer to the `Supported HTML Subset <http://doc.qt.io/qt-5/richtext-html-subset.html>`__
documentation for a list of all supported css styles.

encryptionHook
~~~~~~~~~~~~~~

.. code:: javascript

    /**
     * This function is called when text has to be encrypted or decrypted
     *
     * @param text string the text to encrypt or decrypt
     * @param password string the password
     * @param decrypt bool if false encryption is demanded, if true decryption is demanded
     * @return the encrypted decrypted text
     */
    function encryptionHook(text, password, decrypt);

You may want to take a look at the example
`encryption-keybase.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml>`__,
`encryption-pgp.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml>`__ or
`encryption-rot13.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-rot13.qml>`__.

noteTaggingHook
~~~~~~~~~~~~~~~

You can implement your own note tagging mechanism for example with special
text in your note like ``@tag1``, ``@tag2``, ``@tag3``.

.. code:: javascript

    /**
     * Handles note tagging for a note
     *
     * This function is called when tags are added to, removed from or renamed in
     * a note or the tags of a note should be listed
     *
     * @param note
     * @param action can be "add", "remove", "rename" or "list"
     * @param tagName tag name to be added, removed or renamed
     * @param newTagName tag name to be renamed to if action = "rename"
     * @return string or string-list (if action = "list")
     */
    function noteTaggingHook(note, action, tagName, newTagName);

-  as soon as a script is activated that implements the new function ``noteTaggingHook``
   note tagging will be handled by that function

-  following features should work via the QOwnNotes user interface

  -  initially importing tags like ``@tag`` from your notes and
     overwriting your current tag assignment

     -  you will not loose your tags tree, just the former assignment to notes
     -  you can still move tags into other tags
     -  if more than one tag has the same name in your tag tree the
        first hit will be assigned

  -  adding a tag to a note will add the tag to the note text
  -  removing a tag from a note will remove the tag from the note text
  -  removing of tags in the tag list will remove those tags from your notes
  -  renaming of tags in the tag list will rename those tags in your notes
  -  bulk tagging of notes in the note list will add those tags to your notes
  -  bulk removing of tags from notes in the note list will remove
     those tags from your notes

You may want to take a look at the example
`note-tagging.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/note-tagging.qml>`__
to implement your own tagging mechanism.

autocompletionHook
~~~~~~~~~~~~~~~~~~

You can return a list of strings to be added to the autocompletion list when
the autocompletion is invoked.

.. code:: javascript

    /**
     * Calls the autocompletionHook function for all script components
     * This function is called when autocompletion is invoked in a note
     *
     * @return QStringList of text for the autocomplete list
     */
    function callAutocompletionHook();

You may want to take a look at the example
`autocompletion.qml <https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/autocompletion.qml>`__.


Exposed classes
---------------

Note
~~~~

.. code:: cpp

    class NoteApi {
        Q_PROPERTY(int id)
        Q_PROPERTY(QString name)
        Q_PROPERTY(QString fileName)
        Q_PROPERTY(QString fullNoteFilePath)
        Q_PROPERTY(QString fullNoteFileDirPath)
        Q_PROPERTY(int noteSubFolderId)
        Q_PROPERTY(QString noteText)
        Q_PROPERTY(QString decryptedNoteText)
        Q_PROPERTY(bool hasDirtyData)
        Q_PROPERTY(QQmlListProperty<TagApi> tags)
        Q_PROPERTY(QDateTime fileCreated)
        Q_PROPERTY(QDateTime fileLastModified)
        Q_INVOKABLE QStringList tagNames();
        Q_INVOKABLE bool addTag(QString tagName);
        Q_INVOKABLE bool removeTag(QString tagName);
        Q_INVOKABLE QString toMarkdownHtml(bool forExport = true);
        Q_INVOKABLE QString getFileURLFromFileName(QString localFileName);
    };

You can use the methods from `Date <https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date>`__
to work with ``fileCreated`` or ``fileLastModified``.

For example:

.. code:: javascript

    script.log(note.fileCreated.toISOString());
    script.log(note.fileLastModified.getFullYear());

Tag
~~~

.. code:: cpp

    class TagApi {
        Q_PROPERTY(int id)
        Q_PROPERTY(QString name)
        Q_PROPERTY(int parentId)
    };

MainWindow
~~~~~~~~~~

.. code:: cpp

    class MainWindow {
        Q_INVOKABLE void reloadTagTree();
        Q_INVOKABLE void reloadNoteSubFolderTree();
        Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList(
                bool forceBuild = false, bool forceLoad = false);
        Q_INVOKABLE void focusNoteTextEdit();
    };

For example:

.. code:: javascript

    // force a reload of the note list
    mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true);
