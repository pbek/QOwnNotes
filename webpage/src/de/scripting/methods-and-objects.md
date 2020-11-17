# Methoden und Objekte, die QOwnNotes bereitstellt

Starten eines externen Programms im Hintergrund
----------------------------------------------


### Methodenaufruf und Parameter
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

### Beispiel

Einfaches Beispiel:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Viele Prozesse ausführen:

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

Vielleicht möchten Sie sich das Beispiel ansehen: [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) oder: [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

Vielleicht möchten Sie auch einen Blick auf den Hook [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) werfen.

::: tip
Sie können Ihren benutzerdefinierten Aktionen auch lokale und globale Verknüpfungen in den *Verknüpfungseinstellungen* zuweisen.
:::

Starten Sie ein externes Programm und warten Sie auf die Ausgabe
----------------------------------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

Vielleicht möchten Sie sich das Beispiel ansehen [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Abrufen des Pfads des aktuellen Notizordners
-------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
 */
QString currentNoteFolderPath();
```

### Beispiel
```js
var path = script.currentNoteFolderPath();
```

Vielleicht möchten Sie sich das Beispiel [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml) ansehen.

Abrufen der aktuellen Notiz
------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * QML wrapper to get the current note
 *
 * @returns {NoteApi} the the current note object
 */
NoteApi currentNote();
```

### Beispiel
```js
var note = script.currentNote();
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Protokollierung beim Protokoll-Widget
-------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * QML wrapper to log to the log widget
 *
 * @param text
 */
void log(QString text);
```

### Beispiel
```js
script.log("my text");
```

Herunterladen einer URL in eine Zeichenfolge
------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString downloadUrlToString(QUrl url);
```

### Beispiel
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

You may want to take a look at the example [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Herunterladen einer URL in den Medienordner
--------------------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

You may want to take a look at the example [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Einfügen einer Mediendatei in den Medienordner
--------------------------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Vielleicht möchten Sie sich das Beispiel ansehen [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Die Notizenvorschau erneuern
-----------------------------

Aktualisiert die Notizvorschau.

### Methodenaufruf und Parameter
```cpp
/**
 * Regenerates the note preview
 */
QString ScriptingService::regenerateNotePreview();
```

### Beispiel
```js
script.regenerateNotePreview();
```

Vielleicht möchten Sie sich das Beispiel ansehen [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Registrieren einer benutzerdefinierten Aktion
---------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
// eine benutzerdefinierte Aktion ohne Schaltfläche hinzufügen
script.registerCustomAction ("mycustomaction1", "Menütext");

// eine benutzerdefinierte Aktion mit einer Schaltfläche hinzufügen
script.registerCustomAction ("mycustomaction1", "Menütext", "Schaltflächentext");

// füge eine benutzerdefinierte Aktion mit einer Schaltfläche und einem Freedesktop-Themensymbol hinzu
script.registerCustomAction ("mycustomaction1", "Menütext", "Schaltflächentext", "task-new");

// eine benutzerdefinierte Aktion mit einer Schaltfläche und einem Symbol aus einer Datei hinzufügen
script.registerCustomAction ("mycustomaction1", "Menütext", "Schaltflächentext", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

You may then want to use the identifier with function `customActionInvoked` in a script like [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Ein Label registrieren
-------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Beispiel
```js
script.registerLabel ("html-label", "<strong> Starker </ strong> HTML-Text <br /> mit drei Zeilen <br /> und einem <a href='https://www.qownnotes.org'> Link zu einer Website </a>. ");

script.registerLabel ("long-label", "ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, der umbrochen wird ");

script.registerLabel ("counter-label");
```

Die Beschriftungen werden im Skript-Dock-Widget angezeigt.

You can use both plain text or html in the labels. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Einstellen des Textes eines registrierten Etiketts
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Beispiel
```js
script.setLabelText("counter-label", "counter text");
```

You can use both plain text or html in the labels. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Neue Notiz erstellen
-------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### Beispiel
```js
script.createNote("My note headline\n===\n\nMy text");
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Zugriff auf die Zwischenablage
-----------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Beispiel
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Schreiben Sie Text in die Notiztextbearbeitung
--------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Beispiel
```js
/ schreibe Text in die Notiz Textbearbeitung
script.noteTextEditWrite ("Mein benutzerdefinierter Text");
```

Möglicherweise möchten Sie die benutzerdefinierte Aktion `transformTextRot13` im Beispiel [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) anzeigen.

Sie können dies zusammen mit ` noteTextEditSelectAll ` verwenden, um den gesamten Text der aktuellen Notiz zu überschreiben.

Lesen Sie den ausgewählten Text in der Notiztextbearbeitung
--------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Beispiel
```js
// lese den ausgewählten Text aus der Notiztextbearbeitung
var text = script.noteTextEditSelectedText ();
```

Möglicherweise möchten Sie die benutzerdefinierte Aktion ` transformTextRot13 </ 0> im Beispiel anzeigen
<a href="https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml">custom-actions.qml</a>.
</p>

<h2 spaces-before="0">Wählen Sie den gesamten Text in der Notiztextbearbeitung aus</h2>

<h3 spaces-before="0">Methodenaufruf und Parameter</h3>

<pre><code class="cpp">/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll();
`</pre>

### Beispiel
```js
script.noteTextEditSelectAll();
```

Sie können dies zusammen mit ` noteTextEditWrite ` verwenden, um den gesamten Text der aktuellen Notiz zu überschreiben.

Wählen Sie die aktuelle Zeile in der Notiztextbearbeitung aus
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Beispiel
```js
script.noteTextEditSelectCurrentLine();
```

Wählen Sie das aktuelle Wort in der Notiztextbearbeitung aus
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Beispiel
```js
script.noteTextEditSelectCurrentWord();
```

Stellen Sie den aktuell ausgewählten Text in der Notiztextbearbeitung ein
-----------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Beispiel
```js
// erweitert die aktuelle Auswahl um ein Zeichen
script.noteTextEditSetSelection (
     script.noteTextEditSelectionStart () - 1,
     script.noteTextEditSelectionEnd () + 1);
```

Holen Sie sich die Startposition der aktuellen Auswahl in der Notiztextbearbeitung
---------------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Beispiel
```js
script.log(script.noteTextEditSelectionStart());
```

Holen Sie sich die Endposition der aktuellen Auswahl in der Notiztextbearbeitung
-------------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Returns the end position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Beispiel
```js
script.log(script.noteTextEditSelectionEnd());
```

Set the text cursor in the note text edit to a certain position
---------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Sets the text cursor in the note text edit to a certain position
 * 0 would be the beginning of the note
 * special case: -1 would be the end of the note
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Beispiel
```js
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

Get the current position of the text cursor in the note text edit
-----------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Beispiel
```js
script.log(script.noteTextEditCursorPosition());
```

Read the current word from the note text edit
---------------------------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
// read the current word in the note text edit
var text = script.noteTextEditCurrentWord();
```

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Check whether platform is Linux, OS X or Windows
------------------------------------------------

### Methodenaufruf und Parameter
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Beispiel
```js
if (script.platformIsLinux()) {
    // only will be executed if under Linux
}
```

Tag the current note
--------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Beispiel
```js
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

Vielleicht möchten Sie sich die benutzerdefinierte Aktion ` favorNote ` in der Beispiel [ Favoritennotiz.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Create or fetch a tag by its name breadcrumb list
-------------------------------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
// creates all tags until the 3rd level and returns the tag object for
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

Search for tags by name
-----------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Beispiel
```js
// searches for all tags with the word game in it
var tags = script.searchTagsByName("game");
```

Vielleicht möchten Sie sich das Beispiel ansehen [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Search for notes by note text
-----------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId){
    var note = script.fetchNoteById(noteId);

    // do something with the note
});
```

Vielleicht möchten Sie sich das Beispiel ansehen

 unique-note-id.qml </ 0>.</p> 





Add a custom stylesheet
-----------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```




### Beispiel


```js
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```


Vielleicht möchten Sie sich das Beispiel ansehen [ custom-stylesheet.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

You can get the object names from the `*.ui` files, for example [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Schauen Sie sich das [ Stylesheet für Referenz ](http://doc.qt.io/qt-5/stylesheet-reference.html) für eine Referenz, welche Stile verfügbar sind.

Wenn Sie Stile in die HTML-Vorschau einfügen möchten, um die Vorschau von Notizen zu ändern, lesen Sie bitte [ notetomarkdownhtmlhook ](hooks.html#notetomarkdownhtmlhook).





Reloading the scripting engine
------------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Reloads the scripting engine
 */
void ScriptingService::reloadScriptingEngine();
```




### Beispiel


```js
// reload the scripting engine
script.reloadScriptingEngine();
```






Fetching a note by its file name
--------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// fetch note by file name
script.fetchNoteByFileName("my note.md");
```






Fetching a note by its id
-------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```




### Beispiel


```js
// fetch note by id
script.fetchNoteById(243);
```


Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Checking if a note exists by its file name
------------------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// check if note exists, but ignore the id of "note"
script.noteExistsByFileName("my note.md", note.id);
```


You may want to take a look at the example





Copying text into the clipboard
-------------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```




### Beispiel


```js
// copy text to the clipboard
script.setClipboardText("text to copy");
```


Vielleicht möchten Sie sich das Beispiel ansehen [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).





Jumping to a note
-----------------



### Methodenaufruf und Parameter


```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 */
void ScriptingService::setCurrentNote(NoteApi *note);
```




### Beispiel


```js
// jump to the note
script.setCurrentNote(note);
```


Vielleicht möchten Sie sich das Beispiel ansehen 





Jumping to a note subfolder
---------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// jump to the note subfolder "a sub folder"
script.jumpToNoteSubFolder("a sub folder");

// jump to the note subfolder "sub" inside of "a sub folder"
script.jumpToNoteSubFolder("a sub folder/sub");
```


::: tip
Sie können einen neuen Notizunterordner im aktuellen Unterordner erstellen, indem Sie aufrufen
:::





Showing an information message box
----------------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```




### Beispiel


```js
// show a information message box
script.informationMessageBox("The text I want to show", "Some optional title");
```






Showing a question message box
------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
    "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
script.log(result);
```


Informationen zu Schaltflächen finden Sie unter [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Vielleicht möchten Sie sich auch das Beispiel ansehen [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).





Showing an open file dialog
---------------------------



### Method call and parameters


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




### Beispiel


```js
// show an open file dialog
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```






Showing a save file dialog
--------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// show a save file dialog
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```


You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Registering script settings variables
-------------------------------------

Sie müssen Eigenschaften in Ihrem Skript definieren und in einer weiteren Eigenschaft mit dem Namen `settingsVariables ` registrieren.

Der Benutzer kann diese Eigenschaften dann in den Skripteinstellungen festlegen.



### Example


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


Darüber hinaus können Sie die ` settingsVariables ` mit einer speziellen Funktion ` registerSettingsVariables () ` wie folgt überschreiben:



### Beispiel


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


Vielleicht möchten Sie sich auch das Beispiel ansehen [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).





Storing and loading persistent variables
----------------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```


Stellen Sie sicher, dass Sie in Ihrem Schlüssel ein aussagekräftiges Präfix wie ` PersistentVariablesTest / myVar ` verwenden, da auf die Variablen von allen Skripten aus zugegriffen werden kann.

Sie können sich auch das Beispiel [ persistent-variables.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml) ansehen.





Loading application settings variables
--------------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```


Denken Sie daran, dass die Einstellungen tatsächlich leer sein können. Sie sorgen sich darum selbst. ` defaultValue ` wird nur verwendet, wenn die Einstellung überhaupt nicht vorhanden ist.





Creating a cache directory
--------------------------

Sie können Dateien am Standard-Cache-Speicherort Ihres Systems zwischenspeichern.



### Method call and parameters


```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```




### Example


```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```






Clearing a cache directory
--------------------------

Sie können die Cache-Dateien Ihres Skripts löschen, indem Sie den Namen an clearCacheDir () übergeben.



### Method call and parameters


```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```




### Example


```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```






Reading the path to the directory of your script
------------------------------------------------

Wenn Sie den Pfad zu dem Verzeichnis abrufen müssen, in dem sich Ihr Skript befindet, um beispielsweise andere Dateien zu laden, müssen Sie eine ` -Eigenschaftszeichenfolge scriptDirPath; ` registrieren. Diese Eigenschaft wird mit dem Pfad zum Verzeichnis des Skripts festgelegt.



### Beispiel


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



### Methodenaufruf und Parameter


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




### Beispiel


```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```






Converting path separators from native ones
-------------------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```






Getting the native directory separator
--------------------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```




### Beispiel


```js
// will return "\" on Windows
script.log(script.dirSeparator());
```






Getting a list of the paths of all selected notes
-------------------------------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Returns a list of the paths of all selected notes
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```




### Beispiel


```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```


Vielleicht möchten Sie sich das Beispiel ansehen [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).





Getting a list of the ids of all selected notes
-----------------------------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Returns a list of the ids of all selected notes
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```




### Beispiel


```js
// returns a list of the ids of all selected notes
script.log(script.selectedNotesIds());
```


Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Triggering a menu action
------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```


Vielleicht möchten Sie sich das Beispiel ansehen [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

Sie können die Objektnamen der Menüaktion von abrufen [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).





Opening an input dialog with a select box
-----------------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
var result = script.inputDialogGetItem(
    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
script.log(result);
```


Vielleicht möchten Sie sich das Beispiel ansehen [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).





Opening an input dialog with a line edit
----------------------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
var result = script.inputDialogGetText(
    "line edit", "Please enter a name", "current text");
script.log(result);
```






Checking if a file exists
-------------------------



### Methodenaufruf und Parameter


```cpp
/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```




### Beispiel


```js
var result = script.fileExists(filePath);
script.log(result);
```






Reading text from a file
------------------------



### Method call and parameters


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




### Example


```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```








Writing text to a file
----------------------



### Method call and parameters


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




### Example


```js
var result = script.writeToFile(filePath, html);
script.log(result);
```


Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Working with websockets
-----------------------

Sie können QOwnNotes mithilfe von ` WebSocketServer ` fernsteuern.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Du kannst die Socket Server Verbindung testen indem du dich mit [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345) zu ihr verbindest.

Sie können Sockets auch mit ` WebSocket ` abhören. Bitte schauen Sie sich das Beispiel an: [ websocket-client.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Beachten Sie, dass Sie die QML ` Websocket ` -Bibliothek von Qt installiert haben müssen, um dies zu verwenden. Unter Ubuntu Linux können Sie beispielsweise ` qml-module-qtwebsockets ` installieren.
