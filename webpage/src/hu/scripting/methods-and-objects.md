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

::: tip
Helyi és globális parancsikonokat is rendelhet az egyéni műveletekhez a *Shortcuts settings* részben.
:::

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

Mellékletfájl beszúrása a mellékletek mappájába
--------------------------------------------------------

### Módszerhívás és paraméterek
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

### Példa
```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

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

### Példa
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

Ezután érdemes használni az azonosítót a function funkcióval `customActionInvoked` egy hasonló szkriptben [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Egyéni műveletet is kiválthat az alkalmazás indítása után a(z) `--action customAction_<identifier>` paraméterrel. További információért tekintse meg a(z) [Indítás utáni menüműveletek](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup)et.
:::

Címke regisztrálása
-------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Példa
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel("long-label", "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap");

script.registerLabel("counter-label");
```

A címkék láthatók lesznek a szkriptek dokkoló moduljában.

A címkékben használhat sima szöveget vagy HTML-t is. A szöveg választható lesz, és a linkekre kattintani lehet.

Ezután érdemes megnéznie a példa szkriptet [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Setting the text of a registered label
--------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
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
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### Példa
```js
script.createNote("My note headline\n===\n\nMy text");
```

Érdemes megnéznie a [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) példát.

::: tip
Ha kikapcsolta, hogy a jegyzet címsora határozza meg a jegyzet fájlnevét, akkor utána magának kell átneveznie a jegyzetfájlt, így:

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
  * A vágólap tartalmát szövegként vagy HTML-ként adja vissza
  *
  * @param asHtml a vágólap tartalmát html-ként adja vissza szöveg helyett
  */
QString ScriptingService::clipboard(bool asHtml);
```

### Példa
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Érdemes megnéznie az [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) példát.

Írjon szöveget a jegyzet szövegszerkesztésébe
--------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Szöveget ír a kurzor aktuális pozíciójába a jegyzet szövegszerkesztésében
  *
  * @param szöveg
  */
void ScriptingService::noteTextEditWrite(QString text);
```

### Példa
```js
// szöveget ír a jegyzet szövegszerkesztésébe
script.noteTextEditWrite ("Saját szöveg");
```

Érdemes megnéznie az `transformTextRot13` egyéni műveletet a [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) példában.

Használhatja ezt a `noteTextEditSelectAll` vel együtt az aktuális jegyzet teljes szövegének felülírásához.

Olvassa el a kijelölt szöveget a jegyzet szövegszerkesztésében
--------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Elolvassa a kiválasztott szöveget a jegyzet szövegszerkesztésében
  *
  * @Visszatérés
  */
QString ScriptingService :: noteTextEditSelectedText ();
```

### Példa
```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

Érdemes megnéznie az `transformTextRot13` egyéni műveletet a [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) példában.

Jelölje ki az összes szöveget a jegyzet szövegszerkesztésében
-------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Kiválasztja az összes szöveget a jegyzet szövegszerkesztésében
  */
void ScriptingService::noteTextEditSelectAll();
```

### Példa
```js
script.noteTextEditSelectAll();
```

Használhatja ezt a `noteTextEditWrite` vel együtt az aktuális jegyzet teljes szövegének felülírásához.

Válassza ki az aktuális sort a jegyzet szövegszerkesztésében
---------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Kiválasztja az aktuális sort a jegyzet szövegszerkesztésében
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
  * Beállítja a kurzort a jegyzet szövegszerkesztésében egy bizonyos pozícióba
  * 0 lenne a hang eleje
  * speciális eset: -1 lenne a hang vége
  *
  * @param pozíció
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

Read the current word from the note text edit
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

Check whether platform is Linux, OS X or Windows
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

Tag the current note
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

Érdemes megnéznie a `kedvencNote` egyéni műveletet a [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml) példában.

Create or fetch a tag by its name breadcrumb list
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

Search for tags by name
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

Érdemes megnéznie az [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml) példát.

Search for notes by note text
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

Érdemes megnéznie az [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml) példát.

Add a custom stylesheet
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

Érdemes megnézni a példát [egyéni stíluslap.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

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

Reloading the scripting engine
------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Újra feltölti a parancsfájl-motor
  */
void ScriptingService :: reloadScriptingEngine ();
```

### Példa
```js
// töltse be újra a szkript motort
script.reloadScriptingEngine();
```

Jegyzet letöltése fájlneve alapján
--------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Megjegyzést a fájlneve alapján kap
  *
  * @param fileName karakterlánc a megjegyzés fájlneve (kötelező)
  * @param noteSubFolderId a jegyzet almappájának egész azonosítója
  * @return NoteApi *
  */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                int noteSubFolderId);
```

### Példa
```js
// jegyzet letöltése fájlnév szerint
script.fetchNoteByFileName ("jegyzetem.md");
```

Megjegyzés behozása az azonosítója alapján
-------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * A jegyzetet az azonosítója alapján szerzi be
  *
  * @param id a jegyzet azonosítójában
  * @return NoteApi *
  */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Példa
```js
// a jegyzet lehívása azonosító szerint
script.fetchNoteById (243);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Annak ellenőrzése, hogy létezik-e jegyzet a fájlneve alapján
------------------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Ellenőrzi, hogy a jegyzetfájl létezik-e a fájlnév alapján
  *
  * @param fileName karakterlánc a megjegyzés fájlneve (kötelező)
  * @param ignoreNoteId az ellenőrzés során figyelmen kívül hagyandó jegyzet egész azonosítója
  * @param noteSubFolderId a jegyzet almappájának egész azonosítója
  * @return bool
  */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId);
```

### Példa
```js
// ellenőrizze, hogy létezik-e jegyzet, de hagyja figyelmen kívül a "jegyzet" azonosítóját
script.noteExistsByFileName("my note.md", note.id);
```

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Szöveg másolása a vágólapra
-------------------------------

### Módszerhívás és paraméterek
```cpp
/**
  * Szöveget vagy HTML-mime adatot másol a vágólapra
  *
  * @param szöveges szöveg a vágólapra
  * @param asHtml bool, ha igaz, a szöveg html mime adatként lesz beállítva
  */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Példa
```js
// szöveg másolása a vágólapra
script.setClipboardText("text to copy");
```

You may want to take a look at the example [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Ugrás egy jegyzethez
-----------------

### Módszerhívás és paraméterek
```cpp
/**
  * Beállítja az aktuális jegyzetet, ha a jegyzet látható a jegyzetlistában
  *
  * @param note NoteApi jegyzet, ahova ugorhat
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

::: tip
You can create a new note subfolder in the current subfolder by calling [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

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
  * Fájl mentési párbeszédpanelt mutat
  *
  * @param felirat (opcionális)
  * @param dir (opcionális)
  * @param szűrő (opcionális)
  * @return QString
  */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                            QString filter);
```

### Példa
```js
// a fájl mentése párbeszédpanel megjelenítése
var fileName = script.getSaveFileName ("Please select HTML file to save", "output.html", "HTML (*.html)");
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registering script settings variables
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

### Példa
```js
// meg kell határoznia a regisztrált változókat, hogy később hozzájuk férhessen
property string myString;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string mySelection;

// regisztrálja a beállítási változókat, hogy a felhasználó beállítsa őket a szkript beállításaiban
// használja ezt a tulajdonságot, ha nincs rá szüksége
//
// sajnos a Qt-ben nincs QVariantHash, csak használni tudjuk
// QVariantMap (amelynek nincs önkényes sorrendje) vagy QVariantList (amely a
// a legkevesebb tetszőlegesen megrendelhető)
tulajdonságváltozat beállításaiVáltozók: [
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
  * Újra regisztrálja a beállítási változókat
  *
  * Használja ezt a módszert, ha kódot szeretne használni a változók felülírásához, például a beállításhoz
  * az alapértelmezett értékek az operációs rendszertől függtek.
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
  * Tartós változót tárol
  * Ezek a változók globálisan elérhetők az összes szkripten
  * Kérjük, használjon értelmes előtagot a kulcsában, például "PersistentVariablesTest / myVar"
  *
  * @param key {QString}
  * @param defaultValue {QVariant}
  */
void ScriptingService::setPersistentVariable (const QString & amp; kulcs,
                                                 const QVariant &amp);

/**
  * Tartós változót tölt be
  * Ezek a változók globálisan elérhetők az összes szkripten
  *
  * @param key {QString}
  * @param defaultValue {QVariant} visszatérési érték, ha a beállítás nem létezik (nem kötelező)
  * @Return
  */
QVariant ScriptingService::getPersistentVariable (const QString & kulcs,
                                                     const QVariant & defaultValue);
```

### Példa
```js
// perzisztens változó tárolása
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// perzisztens változó betöltése és naplózása
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
 * Törli a szkript gyorsítótár könyvtárát
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

Converting path separators to native ones
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

Converting path separators from native ones
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

Getting the native directory separator
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

Getting a list of the paths of all selected notes
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

Getting a list of the ids of all selected notes
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

Triggering a menu action
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

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Just search for the English menu title. Note that these texts can change over time.
:::

Opening an input dialog with a select box
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

Opening an input dialog with a line edit
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

Checking if a file exists
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

Reading text from a file
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
  * Szöveget ír egy fájlba
  *
  * @param filePath {QString}
  * @param adatok {QString}
  * @param createParentDirs {bool} opcionális (alapértelmezett: hamis)
  * @Visszatérés
  */
bool ScriptingService :: writeToFile (const QString & amp; filePath, const QString & amp; adatok, bool createParentDirs);
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
