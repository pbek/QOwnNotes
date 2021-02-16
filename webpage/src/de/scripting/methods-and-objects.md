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

::: tip
Sie können Ihren benutzerdefinierten Aktionen auch lokale und globale Verknüpfungen in den *Verknüpfungseinstellungen* zuweisen.
:::

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

Vielleicht möchten Sie sich das Beispiel ansehen [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

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

Vielleicht möchten Sie sich das Beispiel [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml) ansehen.

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

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

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

Vielleicht möchten Sie sich das Beispiel ansehen: [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

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

Vielleicht möchten Sie sich das Beispiel ansehen [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

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

Vielleicht möchten Sie sich das Beispiel ansehen [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Die Notizenvorschau erneuern
-----------------------------

Aktualisiert die Notizvorschau.

### Methodenaufruf und Parameter
```cpp
/**
 * Regeneriert die Notenvorschau
 */
QString ScriptingService::regenerateNotePreview ();
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
    * Registriert eine benutzerdefinierte Aktion
    *
    * @param bezeichner der bezeichner der aktion
    * @param menuText den im Menü angezeigten Text
    * @param buttonText den in der Schaltfläche angezeigten Text
    *               (keine Schaltfläche wird angezeigt, wenn leer)
    * @param icon Der Pfad der Symboldatei oder der Name eines Freedesktop-Themensymbols
    * Eine Liste der Symbole finden Sie hier:
    * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
    * @param useInNoteEditContextMenu Wenn true, verwenden Sie die Aktion in der Notizbearbeitung
    *                Kontextmenü (Standard: false)
    * @param hideButtonInToolbar Wenn true, wird die Schaltfläche nicht in der angezeigt
    *                Symbolleiste für benutzerdefinierte Aktionen (Standard: false)
    * @param useInNoteListContextMenu Wenn true, verwenden Sie die Aktion in der Notizliste
    *                Kontextmenü (Standard: false)
    */
void ScriptingService::registerCustomAction (QString-ID,
                                            QString menuText,
                                            QString buttonText,
                                            QString-Symbol,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

### Beispiel
```js
// eine benutzerdefinierte Aktion ohne Schaltfläche hinzufügen
script.registerCustomAction ("mycustomaction1", "Menütext");

// eine benutzerdefinierte Aktion mit einer Schaltfläche hinzufügen
script.registerCustomAction ("mycustomaction1", "Menu text", "Button text");

// füge eine benutzerdefinierte Aktion mit einer Schaltfläche und einem Freedesktop-Themensymbol hinzu
script.registerCustomAction ("mycustomaction1", "Menu text", "Button text", "task-new");

// eine benutzerdefinierte Aktion mit einer Schaltfläche und einem Symbol aus einer Datei hinzufügen
script.registerCustomAction ("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

Möglicherweise möchten Sie dann den Bezeichner mit Funktion verwenden `customActionInvoked` in einem Skript wie [ custom-action.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Ein Label registrieren
-------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Registriert ein Etikett, auf das geschrieben werden soll
     *
     * @param bezeichner der bezeichner des etiketts
     * @param text der auf dem Etikett angezeigte Text (optional)
     */
void ScriptingService :: registerLabel (QString-ID, QString-Text);
```

### Beispiel
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel("long-label", "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap");

script.registerLabel("counter-label");
```

Die Beschriftungen werden im Skript-Dock-Widget angezeigt.

Sie können sowohl einfachen Text als auch HTML in den Beschriftungen verwenden. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

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

Sie können sowohl einfachen Text als auch HTML in den Beschriftungen verwenden. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Neue Notiz erstellen
-------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Erstellt eine neue Notiz
     **
     * @param text der notentext
     */
void ScriptingService::createNote (QString-Text);
```

### Beispiel
```js
script.createNote("My note headline\n===\n\nMy text");
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

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

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

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
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

Möglicherweise möchten Sie die benutzerdefinierte Aktion `transformTextRot13` im Beispiel anzeigen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

You can use this together with `noteTextEditSelectAll` to overwrite the whole text of the current note.

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
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Wählen Sie den gesamten Text in der Notiztextbearbeitung aus
-------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Selects all text in the note text edit
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
 * Returns the end position of the current selection in the note text edit
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

Ruft die aktuelle Position des Textcursors in der Notiztextbearbeitung ab
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

Lesen Sie das aktuelle Wort aus der Notiztextbearbeitung
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

Vielleicht möchten Sie sich das Beispiel ansehen [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

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

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Erstellen oder holen Sie ein Tag anhand seiner Namens-Breadcrumb-Liste
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

Suche nach Tags nach Namen
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

Suchen Sie nach Notizen gemäß Notiztext
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

Fügen Sie ein benutzerdefiniertes Stylesheet hinzu
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

You can get the object names from the `*.ui` files, for example [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

Skript-Engine neu laden
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

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

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

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Kopieren von Text in die Zwischenablage
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

You may want to take a look at the example [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Zu einer Notiz springen
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

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

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
You can create a new note subfolder in the current subfolder by calling [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
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

For information about buttons see [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

You may also want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Anzeigen eines geöffneten Dateidialogs
---------------------------

### Methodenaufruf und Parameter
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
// show a save file dialog
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registrieren von Skripteinstellungsvariablen
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

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

In addition you can override the `settingsVariables` with a special function `registerSettingsVariables()` like this:

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

You may also want to take a look at the example [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

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

Please make sure to use a meaningful prefix in your key like `PersistentVariablesTest/myVar` because the variables are accessible from all scripts.

You may also want to take a look at the example [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Laden von Anwendungseinstellungsvariablen
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

Keep in mind that settings actually can be empty, you have to take care about that yourself. `defaultValue` is only used if the setting doesn't exist at all.

Erstellen eines Cache-Verzeichnisses
--------------------------

You can cache files at the default cache location of your system.

### Methodenaufruf und Parameter
```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Beispiel
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Löschen eines Cache-Verzeichnisses
--------------------------

You can clear the cache files of your script by passing its name to clearCacheDir().

### Methodenaufruf und Parameter
```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Beispiel
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

Lesen Sie den Pfad zum Verzeichnis Ihres Skripts
------------------------------------------------

If you need to get the path to the directory where your script is placed to for example load other files you have to register a `property string scriptDirPath;`. This property will be set with the path to the script's directory.

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

Konvertieren von Pfadtrennzeichen von nativen
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

You may want to take a look at the example [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Abrufen einer Liste der IDs aller ausgewählten Notizen
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

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Auslösen einer Menüaktion
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

You may want to take a look at the example [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Öffnen eines Eingabedialogs mit einem Auswahlfeld
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

Öffnen eines Eingabedialogs mit einer Zeilenbearbeitung
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

Überprüfen, ob eine Datei vorhanden ist
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

Text aus einer Datei lesen
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


Text in eine Datei schreiben
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

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Arbeiten mit Websockets
-----------------------

You can remote control QOwnNotes by using `WebSocketServer`.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). You can test the socket server by connecting to it on [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

You can also listen to sockets with `WebSocket`. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library installed to use this. For example under Ubuntu Linux you can install `qml-module-qtwebsockets`.
