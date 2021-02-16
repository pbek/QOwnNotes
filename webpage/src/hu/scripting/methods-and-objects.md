# Módszerek és objektumok, amelyeket a QOwnNotes biztosít

Külső program indítása a háttérben
----------------------------------------------


### Módszerhívás és paraméterek
```cpp
/**
  * QML-csomagoló egy leválasztott folyamat elindításához
  *
  * @param végrehajthatóPath a futtatható fájl elérési útja
  * @param paraméterek a paraméterláncok listája
  * @param callbackIdentifier az onDetachedProcessCallback () függvényben használandó azonosító (opcionális)
  * @param callbackParameter egy további paraméter a hurkokhoz vagy hasonlókhoz (opcionális)
  * @param processA visszahívás használata esetén a folyamatba írt adatok (opcionális)
  * @retret true a sikerre, hamis egyébként
  */
bool startDetachedProcess (QString végrehajtható útvonal, QStringList paraméterek,
                             QString callbackIdentifier, QVariant callbackParameter,
                             QByteArray processData);
```

### Példa

Egyszerű példa:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Sok folyamat futtatása:

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

Érdemes megnézni a példát [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) or [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

Érdemes egy pillantást vetni az [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) kampóra is.

::: tip Helyi és globális parancsikonokat is rendelhet az egyéni műveletekhez a *Shortcuts settings* részben. :::

Indítson egy külső programot, és várja meg a kimenetet
----------------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * QML-csomagoló a szinkron folyamat elindításához
  *
  * @param végrehajthatóPath a futtatható fájl elérési útja
  * @param paraméterek a paraméterláncok listája
  * @param adatok a folyamatba írandó adatok (nem kötelező)
  * @ visszaadja a folyamat által visszaadott szöveget
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data);
```

### Példa
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

Érdemes megnézni a példát [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Az aktuális jegyzet mappa elérési útjának lekérése
-------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * QML-csomagoló az aktuális jegyzetmappa elérési útjának megszerzéséhez
  *
  * @ visszaállítja az aktuális jegyzet mappa elérési útját
  */
QString currentNoteFolderPath();
```

### Példa
```js
var path = script.currentNoteFolderPath();
```

Érdemes megnézni a példát [abszolút-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

Az aktuális jegyzet megszerzése
------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * QML-csomagoló az aktuális jegyzet megszerzéséhez
  *
  * @returns {NoteApi} az aktuális jegyzetobjektumot
  */
NoteApi currentNote ();
```

### Példa
```js
var note = script.currentNote();
```

Érdemes megnéznie az [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) példát.

Naplózás a napló widgetbe
-------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * QML-csomagoló a napló widgetbe való bejelentkezéshez
  *
  * @param szöveg
  */
void log(QString text);
```

### Példa
```js
script.log("my text");
```

URL letöltése egy karakterláncra
------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Wrapper QML pour télécharger une URL et la renvoyer sous forme de texte
  *
  * @param url
  * @return {QString} le contenu de l'url téléchargée
  */
QString downloadUrlToString (QUrl url);
```

### Példa
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

Érdemes megnézni a példát [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

URL letöltése a média mappába
--------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * QML-csomagoló egy URL letöltésére a média mappába és a média visszaküldése
  * URL vagy az adathordozó jelölési képszövege az aktuális jegyzethez képest
  *
  * @param {QString} URL
  * @param {bool} returnUrlCsak akkor, ha igaz, csak a média URL kerül visszaadásra (alapértelmezett hamis)
  * @return {QString} a média jelölése vagy URL-je
  */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### Példa
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

Érdemes megnézni a példát [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Inserting a media file into the media folder
--------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * QML-csomagoló médiafájl beszúrásához a média mappába és visszatérés
  * az adathordozó URL-je vagy a hordozó jelölési képszövege az aktuális jegyzethez képest
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlCsak akkor, ha igaz, csak a média URL kerül visszaadásra (alapértelmezett hamis)
  * @return {QString} a média jelölése vagy URL-je
  */
QString ScriptingService :: insertMediaFile (QString mediaFilePath,
                                         bool returnUrlOnly);
```

### Példa
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Érdemes megnézni a példát [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

A jegyzet előnézetének regenerálása
-----------------------------

Frissíti a jegyzet előnézetét.

### Módszerhívás és paraméterek
```cpp
/**
  * Regenerálja a jegyzet előnézetét
  */
QString ScriptingService::regenerateNotePreview();
```

### Példa
```js
script.regenerateNotePreview();
```

Érdemes megnézni a példát [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Egyéni művelet regisztrálása
---------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Egyéni műveletet regisztrál
 *
 * @param azonosítója a művelet azonosítója
 * @param menü Szöveg a menüben látható szöveg
 * @param gombText a gombon látható szöveg
 * (üres gomb nem jelenik meg)
 * @param ikon az ikonfájl útvonala vagy a freeskesk téma téma neve
 * az ikonok listáját itt találja:
 * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu, ha igaz, használja a műveletet a jegyzet szerkesztésében
 * helyi menü (alapértelmezett: hamis)
 * @param hideButtonInToolbar ha igaz, akkor a gomb nem jelenik meg a
 * egyéni műveleti eszköztár (alapértelmezett: hamis)
 * @param useInNoteListContextMenu, ha igaz, használja a jegyzetlista műveletét
 * helyi menü (alapértelmezett: hamis)
 */
void ScriptingService::registerCustomAction(QString identifier,
                                            QString menuText,
                                            QString buttonText,
                                            QString ikon,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

### Példa
```js
// egyéni művelet hozzáadása gomb nélkül
script.registerCustomAction ("mycustomaction1", "Menüszöveg");

// egyéni művelet hozzáadása egy gombbal
script.registerCustomAction ("mycustomaction1", "Menüszöveg", "Gombszöveg");

// egyéni művelet hozzáadása egy gombbal és a freeskesktop téma ikonnal
script.registerCustomAction ("mycustomaction1", "Menüszöveg", "Gombszöveg", "task-new");

// hozzáad egy egyéni műveletet egy gombbal és egy ikonnal egy fájlból
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

Ezután érdemes használni az azonosítót a function funkcióval `customActionInvoked` egy hasonló szkriptben [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Címke regisztrálása
-------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Regisztrál egy címkét, amelyhez írhat
  *
  * @param azonosítója a címke azonosítója
  * @param szöveg a címkén látható szöveg (nem kötelező)
  */
void ScriptingService :: registerLabel (QString azonosító, QString szöveg);
```

### Példa
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel ("long-label", "egy másik nagyon hosszú, egy másik nagyon hosszú, egy másik nagyon hosszú, egy másik nagyon hosszú, egy másik nagyon hosszú, egy másik nagyon hosszú, egy másik nagyon hosszú, egy másik nagyon hosszú szöveg , egy másik nagyon hosszú szöveg, egy másik nagyon hosszú szöveg, egy másik nagyon hosszú szöveg, amely be fog burkolni ");

script.registerLabel ("ellencímke");
```

A címkék láthatók lesznek a szkriptek dokkoló moduljában.

A címkékben használhat sima szöveget vagy HTML-t is. A szöveg választható lesz, és a linkekre kattintani lehet.

Ezután érdemes megnéznie a példa szkriptet [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Bejegyzett címke szövegének beállítása
--------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Beállítja a bejegyzett címke szövegét
  *
  * @param azonosítója a címke azonosítója
  * @param szöveg a címkén látható szöveget
  */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Példa
```js
script.setLabelText("counter-label", "counter text");
```

A címkékben használhat sima szöveget vagy HTML-t is. A szöveg választható lesz, és a linkekre kattintani lehet.

Ezután érdemes megnéznie a példa szkriptet [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Új jegyzet létrehozása
-------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Új jegyzetet hoz létre
  *
  * @param írja be a jegyzet szövegét
  */
void ScriptingService::createNote(QString text);
```

### Példa
```js
script.createNote("My note headline\n===\n\nMy text");
```

Érdemes megnézni a példát [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterwards, like this:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

A vágólap elérése
-----------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Példa
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Írjon szöveget a jegyzet szövegszerkesztésébe
--------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Példa
```js
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

Érdemes megnéznie az `transformTextRot13` egyéni műveletet a [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) példában.

You can use this together with `noteTextEditSelectAll` to overwrite the whole text of the current note.

Olvassa el a kijelölt szöveget a jegyzet szövegszerkesztésében
--------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Példa
```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Jelölje ki az összes szöveget a jegyzet szövegszerkesztésében
-------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll();
```

### Példa
```js
script.noteTextEditSelectAll();
```

You can use this together with `noteTextEditWrite` to overwrite the whole text of the current note.

Válassza ki az aktuális sort a jegyzet szövegszerkesztésében
---------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Példa
```js
script.noteTextEditSelectCurrentLine();
```

Válassza ki az aktuális szót a jegyzet szövegszerkesztésében
---------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Példa
```js
script.noteTextEditSelectCurrentWord();
```

Állítsa be az aktuálisan kiválasztott szöveget a jegyzet szövegszerkesztésében
-----------------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Példa
```js
// expands the current selection by one character
script.noteTextEditSetSelection(
    script.noteTextEditSelectionStart() - 1,
    script.noteTextEditSelectionEnd() + 1);
```

Az aktuális kijelölés kiindulási helyének megszerzése a jegyzetszöveg szerkesztésében
---------------------------------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Példa
```js
script.log(script.noteTextEditSelectionStart());
```

Az aktuális kijelölés végpozíciója a jegyzetszöveg szerkesztésében
-------------------------------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Returns the end position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Példa
```js
script.log(script.noteTextEditSelectionEnd());
```

Állítsa a kurzort a jegyzet szövegszerkesztésében egy bizonyos pozícióra
---------------------------------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

Szerezze be a szövegmutató aktuális helyzetét a jegyzetszöveg szerkesztésében
-----------------------------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Példa
```js
script.log(script.noteTextEditCursorPosition());
```

Olvassa el az aktuális szót a jegyzet szövegszerkesztéséből
---------------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// read the current word in the note text edit
var text = script.noteTextEditCurrentWord();
```

Érdemes megnéznie az [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml) példát.

Ellenőrizze, hogy a platform Linux, OS X vagy Windows
------------------------------------------------

### Módszerhívás és paraméterek
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Példa
```js
if (script.platformIsLinux()) {
    // only will be executed if under Linux
}
```

Jelölje be az aktuális jegyzetet
--------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Példa
```js
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Hozzon létre vagy hozzon létre egy címkét a nevének rákattintási listáján
-------------------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// creates all tags until the 3rd level and returns the tag object for
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

Címkék keresése név szerint
-----------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Példa
```js
// searches for all tags with the word game in it
var tags = script.searchTagsByName("game");
```

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Jegyzetek keresése jegyzetszöveg alapján
-----------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId){
    var note = script.fetchNoteById(noteId);

    // do something with the note
});
```

You may want to take a look at the example [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

Adjon hozzá egy egyedi stíluslapot
-----------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Példa
```js
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

You may want to take a look at the example [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

You can get the object names from the `*.ui` files, for example [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

A parancsfájlmotor újratöltése
------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Reloads the scripting engine
 */
void ScriptingService::reloadScriptingEngine();
```

### Példa
```js
// reload the scripting engine
script.reloadScriptingEngine();
```

Jegyzet letöltése fájlneve alapján
--------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// fetch note by file name
script.fetchNoteByFileName("my note.md");
```

Megjegyzés behozása az azonosítója alapján
-------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Példa
```js
// fetch note by id
script.fetchNoteById(243);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Annak ellenőrzése, hogy létezik-e jegyzet a fájlneve alapján
------------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// check if note exists, but ignore the id of "note"
script.noteExistsByFileName("my note.md", note.id);
```

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Szöveg másolása a vágólapra
-------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Példa
```js
// copy text to the clipboard
script.setClipboardText("text to copy");
```

You may want to take a look at the example [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Ugrás egy jegyzethez
-----------------

### Módszerhívás és paraméterek
```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 */
void ScriptingService::setCurrentNote(NoteApi *note);
```

### Példa
```js
// jump to the note
script.setCurrentNote(note);
```

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Ugrás egy jegyzet almappájára
---------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// jump to the note subfolder "a sub folder"
script.jumpToNoteSubFolder("a sub folder");

// jump to the note subfolder "sub" inside of "a sub folder"
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: tip You can create a new note subfolder in the current subfolder by calling [`mainWindow.createNewNoteSubFolder`](classes.html#example-2). :::

Információs üzenetdoboz megjelenítése
----------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Példa
```js
// show a information message box
script.informationMessageBox("The text I want to show", "Some optional title");
```

Kérdés üzenetmező megjelenítése
------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
    "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
script.log(result);
```

For information about buttons see [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

You may also want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Megnyitott fájl párbeszédpanel megjelenítése
---------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// show an open file dialog
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Fájl mentési párbeszédpanel megjelenítése
--------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// show a save file dialog
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registering script settings variables
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

### Példa
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

### Példa
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

Tartós változók tárolása és betöltése
----------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```

Please make sure to use a meaningful prefix in your key like `PersistentVariablesTest/myVar` because the variables are accessible from all scripts.

You may also want to take a look at the example [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Az alkalmazás beállításainak változóinak betöltése
--------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Keep in mind that settings actually can be empty, you have to take care about that yourself. `defaultValue` is only used if the setting doesn't exist at all.

Gyorsítótár könyvtár létrehozása
--------------------------

You can cache files at the default cache location of your system.

### Módszerhívás és paraméterek
```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Példa
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Gyorsítótár könyvtár törlése
--------------------------

You can clear the cache files of your script by passing its name to clearCacheDir().

### Módszerhívás és paraméterek
```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Példa
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

A szkript könyvtárának elérési útjának elolvasása
------------------------------------------------

If you need to get the path to the directory where your script is placed to for example load other files you have to register a `property string scriptDirPath;`. This property will be set with the path to the script's directory.

### Példa
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

Az útvonalelválasztók átalakítása natívakká
-----------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

Az ösvényelválasztók konvertálása a natívakról
-------------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

A natív könyvtár elválasztó megszerzése
--------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Példa
```js
// will return "\" on Windows
script.log(script.dirSeparator());
```

Az összes kijelölt jegyzet elérési útjának felsorolása
-------------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Returns a list of the paths of all selected notes
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Példa
```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

You may want to take a look at the example [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Az összes kijelölt jegyzet azonosítóinak listájának lekérése
-----------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Returns a list of the ids of all selected notes
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Példa
```js
// returns a list of the ids of all selected notes
script.log(script.selectedNotesIds());
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Menüművelet kiváltása
------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

You may want to take a look at the example [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Beviteli párbeszédpanel megnyitása egy kiválasztott mezővel
-----------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
var result = script.inputDialogGetItem(
    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
script.log(result);
```

You may want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Beviteli párbeszédpanel megnyitása sorszerkesztéssel
----------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
var result = script.inputDialogGetText(
    "line edit", "Please enter a name", "current text");
script.log(result);
```

Ellenőrizze, hogy létezik-e fájl
-------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### Példa
```js
var result = script.fileExists(filePath);
script.log(result);
```

Szöveg olvasása egy fájlból
------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```


Szöveg írása fájlba
----------------------

### Módszerhívás és paraméterek
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

### Példa
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Webhálózatok használata
-----------------------

You can remote control QOwnNotes by using `WebSocketServer`.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). You can test the socket server by connecting to it on [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

You can also listen to sockets with `WebSocket`. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library installed to use this. For example under Ubuntu Linux you can install `qml-module-qtwebsockets`.
