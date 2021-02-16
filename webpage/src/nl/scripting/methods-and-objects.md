# Methoden en objecten die QOwnNotes biedt

Het starten van een extern programma op de achtergrond
----------------------------------------------


### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om een losgekoppeld proces te starten
  *
  * @param executable Pad het pad naar het uitvoerbare bestand
  * @param parameters een lijst met parameterreeksen
  * @param callbackIdentifier een ID die moet worden gebruikt in de functie onDetachedProcessCallback () (optioneel)
  * @param callbackParameter een extra parameter voor loops en dergelijke (optioneel)
  * @param processData-gegevens die naar het proces worden geschreven als de callback wordt gebruikt (optioneel)
  * @return true bij succes, anders false
  */
bool startDetachedProcess (QString executablePath, QStringList parameters,
                             QString callbackIdentifier, QVariant callbackParameter,
                             QByteArray processData);
```

### Voorbeeld

Eenvoudig voorbeeld:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Veel processen uitvoeren:

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

Misschien wilt u het voorbeeld eens bekijken [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) of [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

Misschien wil je ook eens kijken naar de [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) hook.

::: tip
U kunt ook lokale en algemene snelkoppelingen aan uw aangepaste acties toewijzen in de *Shortcuts settings*.
:::

Start een extern programma en wacht op de uitvoer
----------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om een synchroon proces te starten
  *
  * @param executable Pad het pad naar het uitvoerbare bestand
  * @param parameters een lijst met parameterreeksen
  * @param data de data die naar het proces worden geschreven (optioneel)
  * @return de tekst die door het proces is geretourneerd
QByteArray startSynchronousProcess (QString executablePath, QStringList-parameters, QByteArray-gegevens);
```

### Voorbeeld
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

Misschien wilt u het voorbeeld eens bekijken [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Het pad ophalen van de huidige notitiemap
-------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om het huidige pad naar de notitiemap op te halen
  *
  * @return het pad van de huidige notitiemap
  */
QString currentNoteFolderPath ();
```

### Voorbeeld
```js
var path = script.currentNoteFolderPath();
```

Misschien wilt u het voorbeeld eens bekijken [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

De huidige notitie ophalen
------------------------

### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om de huidige notitie te krijgen
  *
  * @ retourneert {NoteApi} het huidige nootobject
  */
OpmerkingApi currentNote ();
```

### Voorbeeld
```js
var note = script.currentNote();
```

Misschien wilt u het voorbeeld eens bekijken [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Inloggen op de log-widget
-------------------------

### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om in te loggen op de log-widget
  *
  * @param tekst
  */
ongeldig logboek (QString-tekst);
```

### Voorbeeld
```js
script.log("my text");
```

Het downloaden van een url naar een string
------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om een url te downloaden en als tekst terug te sturen
  *
  * @param url
  * @return {QString} de inhoud van de gedownloade url
  */
QString downloadUrlToString (QUrl url);
```

### Voorbeeld
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

Misschien wilt u het voorbeeld eens bekijken [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Een url downloaden naar de mediamap
--------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om een url naar de mediamap te downloaden en de media terug te sturen
  * url of de markdown-afbeeldingstekst van de media ten opzichte van de huidige notitie
  *
  * @param {QString} url
  * @param {bool} returnUrlAlleen als waar alleen de media-URL wordt geretourneerd (standaard false)
  * @return {QString} de media-markdown of url
  */
QString downloadUrlToMedia (QUrl url, bool returnUrlOnly);
```

### Voorbeeld
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

Misschien wilt u het voorbeeld eens bekijken [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Een mediabestand invoegen in de mediamap
--------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * QML-wrapper om een mediabestand in de mediamap in te voegen en terug te keren
  * de media-url of de afgeprijsde afbeeldingstekst van de media ten opzichte van de huidige notitie
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlAlleen als waar alleen de media-URL wordt geretourneerd (standaard false)
  * @return {QString} de media-markdown of url
  */
QString ScriptingService :: insertMediaFile (QString mediaFilePath,
                                         bool returnUrlOnly);
```

### Voorbeeld
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Misschien wilt u het voorbeeld eens bekijken [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Het notitievoorbeeld opnieuw genereren
-----------------------------

Vernieuwt het notitievoorbeeld.

### Methodeaanroep en parameters
```cpp
/**
  * Genereert het notitievoorbeeld opnieuw
  */
QString ScriptingService::regenerateNotePreview();
```

### Voorbeeld
```js
script.regenerateNotePreview();
```

Misschien wilt u het voorbeeld eens bekijken [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Registering a custom action
---------------------------

### Methodeaanroep en parameters
```cpp
**
 * Registreert een aangepaste actie
 *
 * @param identifier de identifier van de actie
 * @param menu Tekst de tekst die in het menu wordt weergegeven
 * @param button Sms de tekst die in de knop wordt weergegeven
 * (er wordt geen knop weergegeven als deze leeg is)
 * @param icon het pictogrambestandspad of de naam van een freedesktop-themapictogram
 * u vindt hier een lijst met pictogrammen:
 * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu indien true, gebruik dan de actie in de notitiebewerking
 * contextmenu (standaard: false)
 * @param hideButtonInToolbar indien waar zal de knop niet getoond worden in het
 * aangepaste actiewerkbalk (standaard: false)
 * @param useInNoteListContextMenu indien waar, gebruik dan de actie in de notitielijst
 * contextmenu (standaard: false)
 */
void ScriptingService :: registerCustomAction (QString identifier,
                                            QString menuText,
                                            QString-knop Tekst,
                                            QString-pictogram,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

### Voorbeeld
```js
// voeg een aangepaste actie toe zonder een knop
script.registerCustomAction ("mycustomaction1", "Menutekst");

// voeg een aangepaste actie toe met een knop
script.registerCustomAction ("mycustomaction1", "Menutekst", "Knoptekst");

// voeg een aangepaste actie toe met een knop en een freedesktop-themapictogram
script.registerCustomAction ("mycustomaction1", "Menutekst", "Knoptekst", "taak-nieuw");

// voeg een aangepaste actie toe met een knop en een pictogram uit een bestand
script.registerCustomAction ("mycustomaction1", "Menutekst", "Knoptekst", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

U kunt dan de identifier met functie `customActionInvoked` gebruiken in een script zoals [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Een label registreren
-------------------

### Methodeaanroep en parameters
```cpp
/**
  * Registreert een label om naar te schrijven
  *
  * @param identifier de identifier van het label
  * @param tekst de tekst die in het label wordt getoond (optioneel)
  */
void ScriptingService :: registerLabel (QString-ID, QString-tekst);
```

### Voorbeeld
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel("long-label", "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap");

script.registerLabel("counter-label");
```

De labels zijn zichtbaar in de scriptdock-widget.

U kunt zowel platte tekst als html in de labels gebruiken. De tekst kan worden geselecteerd en er kan op links worden geklikt.

Misschien wilt u het voorbeeld eens bekijken [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

De tekst van een geregistreerd label instellen
--------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Stelt de tekst van een geregistreerd label in
  *
  * @param identifier de identifier van het label
  * @param tekst de tekst die in het label wordt weergegeven
  */
void ScriptingService :: setLabelText (QString-ID, QString-tekst);
```

### Voorbeeld
```js
script.setLabelText("counter-label", "counter text");
```

U kunt zowel platte tekst als html in de labels gebruiken. De tekst kan worden geselecteerd en er kan op links worden geklikt.

Misschien wilt u het voorbeeld eens bekijken [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Een nieuwe notitie maken
-------------------

### Methodeaanroep en parameters
```cpp
/**
  * Maakt een nieuwe notitie
  *
  * @param sms de notitietekst
  */
void ScriptingService :: createNote (QString-tekst);
```

### Voorbeeld
```js
script.createNote("My note headline\n===\n\nMy text");
```

Misschien wilt u het voorbeeld eens bekijken [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterwards, like this:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

Toegang tot het klembord
-----------------------

### Methodeaanroep en parameters
```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Voorbeeld
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Schrijf tekst naar de tekst van de notitie
--------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Voorbeeld
```js
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

Misschien wilt u de aangepaste actie `transformTextRot13` in voorbeeld [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) bekijken.

You can use this together with `noteTextEditSelectAll` to overwrite the whole text of the current note.

Lees de geselecteerde tekst in de tekstbewerking van de notitie
--------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Voorbeeld
```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Selecteer alle tekst in de tekstbewerking van de notitie
-------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll();
```

### Voorbeeld
```js
script.noteTextEditSelectAll();
```

You can use this together with `noteTextEditWrite` to overwrite the whole text of the current note.

Selecteer de huidige regel in de tekstbewerking van de notitie
---------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Voorbeeld
```js
script.noteTextEditSelectCurrentLine();
```

Selecteer de huidige woord in de tekstbewerking van de notitie
---------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Voorbeeld
```js
script.noteTextEditSelectCurrentWord();
```

Stel de momenteel geselecteerde tekst in bij het bewerken van de notitietekst
-----------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Voorbeeld
```js
// expands the current selection by one character
script.noteTextEditSetSelection(
    script.noteTextEditSelectionStart() - 1,
    script.noteTextEditSelectionEnd() + 1);
```

Verkrijg de startpositie van de huidige selectie in de notitietekst
---------------------------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Voorbeeld
```js
script.log(script.noteTextEditSelectionStart());
```

Haal de eindpositie van de huidige selectie op in de tekstbewerking van de notitie
-------------------------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Returns the end position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Voorbeeld
```js
script.log(script.noteTextEditSelectionEnd());
```

Set the text cursor in the note text edit to a certain position
---------------------------------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

Haal de huidige positie van de tekstcursor op in de tekstbewerking van de notitie
-----------------------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Voorbeeld
```js
script.log(script.noteTextEditCursorPosition());
```

Lees het huidige woord uit de tekst van de notitie
---------------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// read the current word in the note text edit
var text = script.noteTextEditCurrentWord();
```

Misschien wilt u het voorbeeld eens bekijken [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Controleer of het platform Linux, OS X of Windows is
------------------------------------------------

### Methodeaanroep en parameters
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Voorbeeld
```js
if (script.platformIsLinux()) {
    // only will be executed if under Linux
}
```

Tag de huidige notitie
--------------------

### Methodeaanroep en parameters
```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Voorbeeld
```js
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Maak of haal een tag op met de naam van de broodkruimellijst
-------------------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// creates all tags until the 3rd level and returns the tag object for
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

Zoek naar tags op naam
-----------------------

### Methodeaanroep en parameters
```cpp
/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Voorbeeld
```js
// searches for all tags with the word game in it
var tags = script.searchTagsByName("game");
```

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Zoek naar notities op notitietekst
-----------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId){
    var note = script.fetchNoteById(noteId);

    // do something with the note
});
```

You may want to take a look at the example [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

Voeg een aangepast stylesheet toe
-----------------------

### Methodeaanroep en parameters
```cpp
/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Voorbeeld
```js
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

You may want to take a look at the example [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

You can get the object names from the `*.ui` files, for example [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

Herladen van de script-engine
------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Reloads the scripting engine
 */
void ScriptingService::reloadScriptingEngine();
```

### Voorbeeld
```js
// reload the scripting engine
script.reloadScriptingEngine();
```

Een notitie ophalen op basis van de bestandsnaam
--------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// fetch note by file name
script.fetchNoteByFileName("my note.md");
```

Een notitie ophalen op basis van zijn ID
-------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Voorbeeld
```js
// fetch note by id
script.fetchNoteById(243);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Controleren of een notitie bestaat op basis van de bestandsnaam
------------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// check if note exists, but ignore the id of "note"
script.noteExistsByFileName("my note.md", note.id);
```

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Tekst naar het klembord kopiëren
-------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Voorbeeld
```js
// copy text to the clipboard
script.setClipboardText("text to copy");
```

You may want to take a look at the example [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Naar een notitie springen
-----------------

### Methodeaanroep en parameters
```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 */
void ScriptingService::setCurrentNote(NoteApi *note);
```

### Voorbeeld
```js
// jump to the note
script.setCurrentNote(note);
```

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Springen naar een submap van een notitie
---------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// jump to the note subfolder "a sub folder"
script.jumpToNoteSubFolder("a sub folder");

// jump to the note subfolder "sub" inside of "a sub folder"
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: tip
You can create a new note subfolder in the current subfolder by calling [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

Een informatieberichtvenster weergeven
----------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Voorbeeld
```js
// show a information message box
script.informationMessageBox("The text I want to show", "Some optional title");
```

Er wordt een berichtvenster met een vraag weergegeven
------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
    "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
script.log(result);
```

For information about buttons see [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

You may also want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Een open bestandsdialoog weergeven
---------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// show an open file dialog
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Een zekere bestandsdialoog weergeven
--------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// show a save file dialog
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Variabelen voor scriptinstellingen registreren
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

### Voorbeeld
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

### Voorbeeld
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

Persistente variabelen opslaan en laden
----------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```

Please make sure to use a meaningful prefix in your key like `PersistentVariablesTest/myVar` because the variables are accessible from all scripts.

You may also want to take a look at the example [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Variabelen voor applicatie-instellingen laden
--------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Keep in mind that settings actually can be empty, you have to take care about that yourself. `defaultValue` is only used if the setting doesn't exist at all.

Een cachemap maken
--------------------------

You can cache files at the default cache location of your system.

### Methodeaanroep en parameters
```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Voorbeeld
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Een cachemap wissen
--------------------------

You can clear the cache files of your script by passing its name to clearCacheDir().

### Methodeaanroep en parameters
```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Voorbeeld
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

Het pad naar de directory van uw script lezen
------------------------------------------------

If you need to get the path to the directory where your script is placed to for example load other files you have to register a `property string scriptDirPath;`. This property will be set with the path to the script's directory.

### Voorbeeld
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

Padscheidingstekens omzetten in native
-----------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

Padscheidingstekens omzetten van native
-------------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

Het native directoryscheidingsteken ophalen
--------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Voorbeeld
```js
// will return "\" on Windows
script.log(script.dirSeparator());
```

Een lijst ophalen met de paden van alle geselecteerde notities
-------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Returns a list of the paths of all selected notes
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Voorbeeld
```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

You may want to take a look at the example [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Een lijst ophalen met de id's van alle geselecteerde notities
-----------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Returns a list of the ids of all selected notes
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Voorbeeld
```js
// returns a list of the ids of all selected notes
script.log(script.selectedNotesIds());
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Een menuactie activeren
------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

You may want to take a look at the example [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Een invoerdialoog openen met een selectievak
-----------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
var result = script.inputDialogGetItem(
    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
script.log(result);
```

You may want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Een invoerdialoog openen met een regelbewerking
----------------------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
var result = script.inputDialogGetText(
    "line edit", "Please enter a name", "current text");
script.log(result);
```

Controleren of er een bestand bestaat
-------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### Voorbeeld
```js
var result = script.fileExists(filePath);
script.log(result);
```

Tekst uit een bestand lezen
------------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```


Tekst naar een bestand schrijven
----------------------

### Methodeaanroep en parameters
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

### Voorbeeld
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Werken met websockets
-----------------------

You can remote control QOwnNotes by using `WebSocketServer`.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). You can test the socket server by connecting to it on [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

You can also listen to sockets with `WebSocket`. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library installed to use this. For example under Ubuntu Linux you can install `qml-module-qtwebsockets`.
