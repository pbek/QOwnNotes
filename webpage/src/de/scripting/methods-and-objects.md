# Methoden und Objekte, die QOwnNotes bereitstellt

Starten eines externen Programms im Hintergrund
----------------------------------------------


### Methodenaufruf und Parameter
```cpp
/**
     * QML-Wrapper zum Starten eines getrennten Prozesses
     *
     * @param executeablePath der Pfad der ausführbaren Datei
     * @param parameters eine Liste von Parameterzeichenfolgen
     * @param callbackIdentifier Ein Bezeichner, der in der Funktion onDetachedProcessCallback () verwendet werden soll (optional).
     * @param callbackParameter ein zusätzlicher Parameter für Schleifen oder ähnliches (optional)
     * @param processData-Daten, die in den Prozess geschrieben werden, wenn der Rückruf verwendet wird (optional)
     * @return true bei Erfolg, andernfalls false
     */
bool startDetachedProcess (QString ausführbarer Pfad, QStringList-Parameter,
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

Starten Sie ein externes Programm und warten Sie auf die Ausgabe
----------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * QML-Wrapper zum Starten eines synchronen Prozesses
  *
  * @param executeablePath - der Pfad der ausführbaren Datei
  * @param parameters - eine Liste von Parameterzeichenfolgen
  * @param data - die Daten, die in den Prozess geschrieben werden (optional)
  * @return - den Text, der vom Prozess zurückgegeben wurde
QByteArray startSynchronousProcess (QString ausführbarer Pfad, QStringList-Parameter, QByteArray-Daten);
```

### Beispiel
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

You may want to take a look at the example [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Abrufen des Pfads des aktuellen Notizordners
-------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * QML-Wrapper, um den aktuellen Pfad des Notizordners abzurufen
  *
  * @ den Pfad des aktuellen Notizordners zurückgeben
  */
QString currentNoteFolderPath ();
```

### Beispiel
```js
var path = script.currentNoteFolderPath();
```

You may want to take a look at the example [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

Abrufen der aktuellen Notiz
------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * QML-Wrapper, um die aktuelle Notiz zu erhalten
     *
     * @retourniert {NoteApi} das aktuelle Notizobjekt
     */
NoteApi currentNote ();
```

### Beispiel
```js
var note = script.currentNote();
```

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Protokollierung beim Protokoll-Widget
-------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * QML-Wrapper zum Protokollieren im Protokoll-Widget
     *
     * @param Text
     */
void log (QString-Text);
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
  * QML-Wrapper zum Herunterladen einer URL und zum Zurückgeben als Text
  *
  * @param url
  * @return {QString} den Inhalt der heruntergeladenen URL
  */
QString downloadUrlToString (QUrl url);
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
     * QML-Wrapper zum Herunterladen einer URL in den Medienordner und zum Zurückgeben des Mediums
     * URL oder der Markdown-Bildtext des Mediums relativ zur aktuellen Notiz
     *
     * @param {QString} URL
     * @param {bool} returnUrlOnly if true, wird nur die Medien-URL zurückgegeben (Standard false).
     * @return {QString} den Medienabschlag oder die URL
     */
QString downloadUrlToMedia (QUrl url, bool returnUrlOnly);
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
 * QML-Wrapper zum Einfügen einer Mediendatei in den Medienordner und zum Zurückgeben
 * die Medien-URL oder der Markdown-Bildtext des Mediums relativ zur aktuellen Notiz
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly wenn true, wird nur die Medien-URL zurückgegeben (Standardwert false)
 * @return {QString} den Medienabschlag oder die URL
 */
QString ScriptingService::insertMediaFile (QString mediaFilePath,
                                         bool returnUrlOnly);
```

### Beispiel
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

You may want to take a look at the example [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Inserting an attachment file into the attachments folder
--------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
 * QML wrapper to insert an attachment file into the `attachments` folder and
 * returning the attachment url or the markdown text of the attachment
 * relative to the current note
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName to use in the markdown
 * @param {bool} returnUrlOnly if true only the attachment url will be returned
 * (default false)
 * @return {QString} the attachment markdown or url
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### Beispiel
```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

Regenerating the note preview
-----------------------------

Refreshes the note preview.

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

You may want to take a look at the example [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Registering a custom action
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

::: tip
You can also assign local and global shortcuts to your custom actions in the *Shortcuts settings*.
:::

::: warning
Keep in mind that [freedesktop theme icons](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) are mostly available only under Linux. So if you really want to use an icon under macOS or Windows you need to provide one with your script. To get the path of your script to set a proper path for your icon you can use the [scriptDirPath property](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
:::

### Beispiel

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Initializes the custom actions
     */
    function init() {
        // add a custom action without a button
        script.registerCustomAction("mycustomaction1", "Menu text");

        // add a custom action with a button
        script.registerCustomAction("mycustomaction2", "Menu text", "Button text");

        // add a custom action with a button and freedesktop theme icon
        script.registerCustomAction("mycustomaction3", "Menu text", "Button text", "task-new");

        // add a custom action with a button and an icon from a file
        script.registerCustomAction("mycustomaction4", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "mycustomaction1":
                script.log("Action 1");
            break;
            case "mycustomaction2":
                script.log("Action 2");
            break;
            case "mycustomaction3":
                script.log("Action 3");
            break;
            case "mycustomaction4":
                script.log("Action 4");
            break;
        }
    }
}
```

For some more examples please see [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
You can also trigger a custom action after the application was started with the parameter `--action customAction_<identifier>`. For more information please take a look at [Trigger menu actions after startup](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

Registering a label
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
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel ("long-label", "ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, der umbrochen wird ");

script.registerLabel ("counter-label");
```

The labels will be visible in the scripting dock widget.

You can use both plain text or html in the labels. The text will be selectable and links can be clicked.

You may then want to take a look at the example script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Einstellen des Textes eines registrierten Etiketts
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Legt den Text eines registrierten Etiketts fest
     *
     * @param bezeichner der bezeichner des etiketts
     * @param text der auf dem Etikett angezeigte Text
     */
void ScriptingService::setLabelText (QString-ID, QString-Text);
```

### Beispiel
```js
script.setLabelText("counter-label", "counter text");
```

You can use both plain text or html in the labels. The text will be selectable and links can be clicked.

You may then want to take a look at the example script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

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

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterwards, like this:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

Zugriff auf die Zwischenablage
-----------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Gibt den Inhalt der Zwischenablage als Text oder HTML zurück
     *
     * @param asHtml gibt den Inhalt der Zwischenablage als HTML anstelle von Text zurück
     */
QString ScriptingService::clipboard (bool asHtml);
```

### Beispiel
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Schreiben Sie Text in die Notiztextbearbeitung
--------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Schreibt Text an die aktuelle Cursorposition in der Notiztextbearbeitung
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

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

You can use this together with `noteTextEditSelectAll` to overwrite the whole text of the current note.

Lesen Sie den ausgewählten Text in der Notiztextbearbeitung
--------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Liest den ausgewählten Text in der Notiztextbearbeitung
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

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Wählen Sie den gesamten Text in der Notiztextbearbeitung aus
-------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Wählt den gesamten Text in der Notiztextbearbeitung aus
 */
void ScriptingService::noteTextEditSelectAll();
```

### Beispiel
```js
script.noteTextEditSelectAll();
```

You can use this together with `noteTextEditWrite` to overwrite the whole text of the current note.

Wählen Sie die aktuelle Zeile in der Notiztextbearbeitung aus
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Wählt die aktuelle Zeile in der Notiztextbearbeitung aus
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
// expands the current selection by one character
script.noteTextEditSetSelection(
    script.noteTextEditSelectionStart() - 1,
    script.noteTextEditSelectionEnd() + 1);
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
 * Gibt die Endposition der aktuellen Auswahl in der Notiztextbearbeitung zurück
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Beispiel
```js
script.log(script.noteTextEditSelectionEnd());
```

Setzen Sie den Textcursor in der Notiztextbearbeitung auf eine bestimmte Position
---------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Setzt den Textcursor in der Notiztextbearbeitung auf eine bestimmte Position
 * 0 wäre der Anfang der Notiz
 * Spezialfall: -1 wäre das Ende der Notiz
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Beispiel
```js
// springe zum 11. Zeichen in der Notiz
script.noteTextEditSetCursorPosition (10);

// zum Ende der Notiz springen
script.noteTextEditSetCursorPosition (-1);
```

Ruft die aktuelle Position des Textcursors in der Notiztextbearbeitung ab
-----------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * Gibt die aktuelle Position des Textcursors in der Notiztextbearbeitung zurück
  * 0 wäre der Anfang der Notiz
  */
int ScriptingService :: noteTextEditCursorPosition ();
```

### Beispiel
```js
script.log(script.noteTextEditCursorPosition());
```

Lesen Sie das aktuelle Wort aus der Notiztextbearbeitung
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * Liest das aktuelle Wort in der Notiztextbearbeitung
  *
  * @param withPreviousCharacters erhalten am Anfang auch mehr Zeichen
  *                                    um Zeichen wie "@" zu erhalten, die es nicht sind
  *                                    Wortzeichen
  * @return
  */
QString ScriptingService::noteTextEditCurrentWord (bool withPreviousCharacters);
```

### Beispiel
```js
// Lies das aktuelle Wort in der Notiztextbearbeitung
var text = script.noteTextEditCurrentWord ();
```

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Prüfen Sie, ob die Plattform Linux, OS X oder Windows ist
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

Verschlagworten Sie die aktuelle Notiz
--------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Markiert die aktuelle Notiz mit einem Tag namens tagName
     *
     * @param tagName
     */
void ScriptingService::tagCurrentNote (QString tagName);
```

### Beispiel
```js
// fügen Sie der aktuellen Notiz ein "Favorit" -Tag hinzu
script.tagCurrentNote ("Favorit");
```

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

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

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

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

You may want to take a look at the example [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

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

You may want to take a look at the example [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

You can get the widget names from the `*.ui` files, for example the main window is [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

The Qt documentation (for example [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) can help you to see how the widgets are related to each other (search for `Inherits` on the pages).

The base widget for almost everything is [QWidget](https://doc.qt.io/qt-5/qwidget.html). So just styling `QWidget` with for example `QWidget {background-color: black; color: white;}` would mean everything has a black background color and a white foreground color.

::: tip
The [style.qss](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle/style.qss) of [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle) might also be a good reference for styles you can change.
:::

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
If you actually want to see how the dialogs look and what the names are you could download [Qt Creator](https://www.qt.io/product/development-tools) and open the `*.ui` files in it.
:::

Neuladen der Scripting-Engine
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

Abrufen einer Notiz anhand ihres Dateinamens
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

Abrufen einer Notiz anhand ihrer ID
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

Überprüfen Sie anhand des Dateinamens, ob eine Notiz vorhanden ist
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

Kopieren von Text in die Zwischenablage
-------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Kopiert Text als Nur-Text- oder HTML-MIME-Daten in die Zwischenablage
     *
     * @param Text String Text, der in die Zwischenablage eingefügt werden soll
     * @param asHtml bool Wenn true, wird der Text als HTML-MIME-Daten festgelegt
     */
void ScriptingService::setClipboardText(QString-Text, bool asHtml);
```

### Beispiel
```js
// copy text to the clipboard
script.setClipboardText("text to copy");
```

Vielleicht möchten Sie sich das Beispiel ansehen [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Zu einer Notiz springen
-----------------

### Methodenaufruf und Parameter
```cpp
/**
    * Legt die aktuelle Notiz fest, wenn die Notiz in der Notizliste sichtbar ist
    *
    * @param note NoteApi note to jump to
    */
void ScriptingService::setCurrentNote(NoteApi *note);
```

### Beispiel
```js
// zur Notiz springen
script.setCurrentNote(note);
```

Vielleicht möchten Sie sich das Beispiel ansehen [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Zu einem Notiz-Unterordner springen
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
Sie können einen neuen Notizunterordner im aktuellen Unterordner erstellen, indem Sie [`mainWindow.createNewNoteSubFolder`](classes.html#example-2) aufrufen.
:::

Anzeigen eines Informationsmeldungsfelds
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

Anzeigen eines Fragenmeldungsfelds
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

Anzeigen eines geöffneten Dateidialogs
---------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Zeigt einen Dialog zum Speichern von Dateien an
     *
     * @param caption (optional)
     * @param dir (optional)
     * @param Filter (optional)
     * @return QString
     */
QString ScriptingService::getSaveFileName (QString-Beschriftung, QString-Verzeichnis,
                                             QString-Filter);
```

### Beispiel
```js
// zeige einen geöffneten Dateidialog
var fileName = script.getOpenFileName ("Bitte wählen Sie ein Bild aus", "/ home / user / images", "Images (*.png *.xpm *.jpg)");
```

Anzeigen eines Dialogfelds zum Speichern von Dateien
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
// zeige einen Dialog zum Speichern von Dateien
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registrieren von Skripteinstellungsvariablen
-------------------------------------

Sie müssen Ihre Einstellungsvariablen als Eigenschaften in Ihrem Skript definieren und in einer weiteren Eigenschaft mit dem Namen `settingsVariables ` registrieren.

Der Benutzer kann diese Eigenschaften dann in den Skripteinstellungen festlegen.

### Beispiel
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
/ **
  * Registriert die Einstellungsvariablen erneut
  * *
  * Verwenden Sie diese Methode, wenn Sie Code verwenden möchten, um Ihre Variablen zu überschreiben, z. B. Einstellungen
  * Standardwerte abhängig vom Betriebssystem.
 */
function registerSettingsVariables() {
    if (script.platformIsWindows()) {
        // override the myFile default value
        settingsVariables[3].default = "pandoc.exe"
    }
}
```

Vielleicht möchten Sie sich auch das Beispiel ansehen [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

Speichern und Laden persistenter Variablen
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

Stellen Sie sicher, dass Sie in Ihrem Schlüssel ein aussagekräftiges Präfix wie `PersistentVariablesTest/myVar` verwenden, da auf die Variablen von allen Skripten aus zugegriffen werden kann.

Sie können sich auch das Beispiel [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml) ansehen.

Laden von Anwendungseinstellungsvariablen
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Lädt eine Anwendungseinstellungsvariable
     *
     * @param key {QString}
     * @param defaultValue {QVariant} Rückgabewert, wenn die Einstellung nicht vorhanden ist (optional)
     * @Rückkehr
     */
QVariant ScriptingService::getApplicationSettingsVariable (const QString &key,
                                                             const QVariant &defaultValue);
```

### Beispiel
```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Denken Sie daran, dass die Einstellungen tatsächlich leer sein können und Sie sich selbst darum kümmern müssen. `defaultValue` wird nur verwendet, wenn die Einstellung überhaupt nicht vorhanden ist.

Erstellen eines Cache-Verzeichnisses
--------------------------

Sie können Dateien am Standard-Cache-Speicherort Ihres Systems zwischenspeichern.

### Methodenaufruf und Parameter
```cpp
/**
 * Gibt ein Cache-Verzeichnis für ein Skript zurück
 *
 * @param {QString} subDir der zu erstellende und zu verwendende Unterordner
 * @return {QString} den Cache-Verzeichnispfad
 */
QString ScriptingService::cacheDir (const QString & subDir) const;
```

### Beispiel
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Löschen eines Cache-Verzeichnisses
--------------------------

Sie können die Cache-Dateien Ihres Skripts löschen, indem Sie den Namen an clearCacheDir () übergeben.

### Methodenaufruf und Parameter
```cpp
/**
 * Löscht das Cache-Verzeichnis für ein Skript
 *
 * @param {QString} subDir den zu löschenden Unterordner
 * @return {bool} true bei Erfolg
 */
bool ScriptingService::clearCacheDir (const QString &subDir) const;
```

### Beispiel
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

Lesen Sie den Pfad zum Verzeichnis Ihres Skripts
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

Konvertieren von Pfadtrennzeichen in native
-----------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Gibt den Pfad mit den '/' Trennzeichen zurück, die in Trennzeichen konvertiert wurden
 * passend für das zugrunde liegende Betriebssystem.
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

Konvertieren von Pfadtrennzeichen von nativen
-------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Gibt den Pfad mit '/' als Dateitrennzeichen zurück.
 * Unter Windows beispielsweise fromNativeDirSeparators("c:\\winnt\\system32")
 * gibt "c:/winnt/system32" zurück.
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

Abrufen des nativen Verzeichnis-Trennzeichens
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

Abrufen einer Liste der Pfade aller ausgewählten Noten
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

Abrufen einer Liste der IDs aller ausgewählten Notizen
-----------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Gibt eine Liste der IDs aller ausgewählten Noten zurück
     *
     * @return {QList <int>} Liste der ausgewählten Noten-IDs
     */
QList <int> ScriptingService :: selectedNotesIds ();
```

### Beispiel
```js
// gibt eine Liste der IDs aller ausgewählten Noten zurück
script.log (script.selectedNotesIds ());
```

Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Auslösen einer Menüaktion
------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Löst eine Menüaktion aus
     *
     * @param objectName {QString} Objektname der auszulösenden Aktion
     * @param checked {QString} löst die Aktion nur aus, wenn der Status check aktiviert ist
     * anders als dieser Parameter (optional, kann 0 oder 1 sein)
     */
void ScriptingService::triggerMenuAction (QString objectName, QString geprüft);
```

### Beispiel
```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Vielleicht möchten Sie sich das Beispiel ansehen [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
Die Objektnamen der Menüaktion erhalten Sie von [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Suchen Sie einfach nach dem englischen Menütitel. Note that these texts can change over time.
:::

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

You may want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

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

### Methodenaufruf und Parameter
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

### Beispiel
```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```


Writing text to a file
----------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Arbeiten mit Websockets
-----------------------

Sie können QOwnNotes mithilfe von ` WebSocketServer ` fernsteuern.

Bitte schauen Sie sich das Beispiel an: [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Du kannst die Socket Server Verbindung testen indem du dich mit [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345) zu ihr verbindest.

Sie können Sockets auch mit `WebSocket` abhören. Bitte schauen Sie sich das Beispiel an: [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Beachten Sie, dass Sie die QML `Websocket`-Bibliothek von Qt installiert haben müssen, um dies zu verwenden. Unter Ubuntu Linux können Sie beispielsweise `qml-module-qtwebsockets` installieren.
