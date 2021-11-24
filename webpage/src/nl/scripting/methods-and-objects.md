# Methoden en objecten die QOwnNotes biedt

Het starten van een extern programma op de achtergrond
----------------------------------------------


### Methodeaanroep en parameters
```cpp
/**
 * QML wrapper to start a detached process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function (optional)
 * @param callbackParameter an additional parameter for loops or the like (optional)
 * @param processData data written to the process if the callback is used (optional)
 * @param workingDirectory the working directory to execute the process in (optional, only works without callback)
 * @return true on success, false otherwise
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
                            QByteArray processData, QString workingDirectory);
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

Start een extern programma en wacht op de uitvoer
----------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * QML wrapper to start a synchronous process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @param workingDirectory the working directory to execute the process in (optional)
 * @return the text that was returned by the process
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### Voorbeeld
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data", "/path/to/execute/in");
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

Een bijlagebestand invoegen in de bijlagenmap
--------------------------------------------------------

### Methodeaanroep en parameters
```cpp
 * QML-wrapper om een bijlagebestand in de map `attachments` in te voegen en
  * de bijlage-URL of de afprijstekst van de bijlage retourneren
  * ten opzichte van de huidige noot
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

### Voorbeeld
```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

Het notitievoorbeeld opnieuw genereren
-----------------------------

Vernieuwt het notitievoorbeeld.

### Methodeaanroep en parameters
```cpp
/**
  * Regenereert het notitievoorbeeld
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

::: tip
U kunt ook lokale en algemene snelkoppelingen aan uw aangepaste acties toewijzen in de *Shortcuts settings*.
:::

::: warning
Houd er rekening mee dat [freedesktop-themapictogrammen](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) meestal alleen beschikbaar zijn onder Linux. Dus als je echt een pictogram onder macOS of Windows wilt gebruiken, moet je er een bij je script voegen. Om het pad van uw script te krijgen om een juist pad voor uw pictogram in te stellen, kunt u de [scriptDirPath-eigenschap](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script) gebruiken.
:::

### Voorbeeld

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

Zie [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) voor nog meer voorbeelden.

::: tip
U kunt ook een aangepaste actie activeren nadat de toepassing is gestart met de parameter `--action customAction_<identifier>`. Kijk voor meer informatie op [Activeer menuacties na opstarten](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

Een label registreren
-------------------

### Methodeaanroep en parameters
```cpp
/**
  * Registreert een label om naar te schrijven
  *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
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
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
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
Als je hebt uitgeschakeld dat je notitiekop de naam van het notitiebestand bepaalt, moet je je notitiebestand daarna zelf hernoemen, zoals dit:

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
  * Schrijft tekst naar de huidige cursorpositie in de tekstbewerking van de notitie
  *
  * @param tekst
  */
void ScriptingService :: noteTextEditWrite (QString-tekst);
```

### Voorbeeld
```js
// schrijf tekst naar de notitie tekst bewerken
script.noteTextEditWrite ("Mijn aangepaste tekst");
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

You can use this together with `noteTextEditSelectAll` to overwrite the whole text of the current note.

Lees de geselecteerde tekst in de tekstbewerking van de notitie
--------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Leest de geselecteerde tekst in de tekstbewerking van de notitie
  *
  * @return
  */
QString ScriptingService :: noteTextEditSelectedText ();
```

### Voorbeeld
```js
// lees de geselecteerde tekst uit de notitietekst bewerk 
var text = script.noteTextEditSelectedText ();
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Selecteer alle tekst in de tekstbewerking van de notitie
-------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Selecteert alle tekst in de tekstbewerking van de notitie
  */
leegte ScriptingService :: noteTextEditSelectAll ();
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
  * Selecteert de huidige regel in de notitietekst
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
  * Selecteert de huidige regel in de tekstbewerking van de notitie
  */
void ScriptingService :: noteTextEditSelectCurrentWord ();
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
  * Geeft als resultaat de startpositie van de huidige selectie in de notitietekst
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
  * Zet de tekstcursor in de notitietekstbewerking op een bepaalde positie
  * 0 zou het begin van de notitie zijn
  * speciaal geval: -1 zou het einde van de notitie zijn
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
  * Geeft de huidige positie van de tekstcursor terug in de notitietekst
  * 0 zou het begin van de notitie zijn
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
  * Leest het huidige woord in de notitietekst
  *
  * @param withPreviousCharacters krijgen in het begin ook meer tekens
  * om tekens als "@" te krijgen die dat niet zijn
  * woordtekens
  * @return
  */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### Voorbeeld
```js
// lees het huidige woord in de tekst van de notitie
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
  // Wordt alleen uitgevoerd onder Linux
}
```

Tag de huidige notitie
--------------------

### Methodeaanroep en parameters
```cpp
/**
 * Labelt de huidige notitie met een tag met de naam tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Voorbeeld
```js
// voeg een "favoriete" tag toe aan de huidige notitie
script.tagCurrentNote("favorite");
```

Misschien wilt u de aangepaste actie `favoriteNote` bekijken in het voorbeeld [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Maak of haal een tag op met de naam van de broodkruimellijst
-------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Haalt of maakt een tag door zijn "broodkruimellijst" van tagnamen
  * Element nameList[0] zou het hoogst zijn in de boom (met parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} if true (default) all missing tags will be created
 * @return TagApi object van de diepste tag van de naam
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Voorbeeld
```js
// maakt alle tags tot het 3e niveau en retourneert het tag-object voor
// tag "level3", die er zo uitziet in de tagboom:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList (["level1", "level2", "level3"]);
```

Zoek naar tags op naam
-----------------------

### Methodeaanroep en parameters
```cpp
/**
 * Haalt alle tags op door een substring-zoekopdracht uit te voeren in het naamveld
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Voorbeeld
```js
// zoekt naar alle tags met het woordspel erin
var tags = script.searchTagsByName("game");
```

Misschien wilt u het voorbeeld eens bekijken [automatisch aanvullen.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Zoek naar notities op notitietekst
-----------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Retourneert een lijst met notitie-ID's van alle notities met een bepaalde tekst in de notitietekst
 *
 * Helaas is er geen gemakkelijke manier om een QList<NoteApi*> in QML te gebruiken, dus we
 * kan alleen de notitie-ID's overdragen
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

    // doe iets met de notitie
});

```

Misschien wilt u het voorbeeld eens bekijken [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

Voeg een aangepast stylesheet toe
-----------------------

### Methodeaanroep en parameters
```cpp
/**
  * Voegt een aangepast stylesheet toe aan de applicatie
  *
  * @param stylesheet
  */
void ScriptingService :: addStyleSheet (QString-stylesheet);
```

### Voorbeeld
```js
// maak de tekst in de notitielijst groter
script.addStyleSheet("QTreeWidget # noteTreeWidget {font-size: 30px;}");
```

Misschien wilt u het voorbeeld eens bekijken [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

U kunt de widgetnamen uit de `*.ui`-bestanden halen, het hoofdvenster is bijvoorbeeld [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

De Qt-documentatie (bijvoorbeeld [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) kan u helpen te zien hoe de widgets aan elkaar gerelateerd zijn (zoek naar `Erfenissen` op de pagina's).

De basiswidget voor bijna alles is [QWidget](https://doc.qt.io/qt-5/qwidget.html). Dus gewoon `QWidget` stylen met bijvoorbeeld `QWidget {background-color: black; kleur: wit;}` zou betekenen dat alles een zwarte achtergrondkleur en een witte voorgrondkleur heeft.

::: tip
De [style.qss](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle/style.qss) van [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle) kan ook een goede referentie zijn voor stijlen die u kunt wijzigen.
:::

Bekijk [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) voor een referentie van welke stijlen beschikbaar zijn.

Als je stijlen in html-preview wilt invoegen om de manier waarop notities worden weergegeven te wijzigen, kijk dan naar [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
Als je echt wilt zien hoe de dialogen eruitzien en hoe ze heten, kun je [Qt Creator](https://www.qt.io/product/development-tools) downloaden en de `*.ui`-bestanden erin openen.
:::

Herladen van de script-engine
------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Herlaadt de script-engine
  */
void ScriptingService::reloadScriptingEngine();
```

### Voorbeeld
```js
// herlaad de script-engine
script.reloadScriptingEngine();
```

Een notitie ophalen op basis van de bestandsnaam
--------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Haalt een notitie op met de bestandsnaam
  *
  * @param fileName string de bestandsnaam van de notitie (verplicht)
  * @param noteSubFolderId geheel getal-id van de notitie-submap
  * @return OpmerkingApi *
  */
NoteApi * ScriptingService::fetchNoteByFileName (QString bestandsnaam,
                                                 int noteSubFolderId);
```

### Voorbeeld
```js
// haal notitie op met bestandsnaam
script.fetchNoteByFileName("my note.md");
```

Een notitie ophalen op basis van zijn ID
-------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Haalt een notitie op met zijn id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Voorbeeld
```js
// haal notitie op met id
script.fetchNoteById(243);
```

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Controleren of een notitie bestaat op basis van de bestandsnaam
------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Controleert of een notitiebestand bestaat op basis van de bestandsnaam
  *
  * @param fileName string de bestandsnaam van de notitie (verplicht)
  * @param ignoreNoteId integer-id van een notitie die bij de controle moet worden genegeerd
  * @param noteSubFolderId geheel getal-id van de notitie-submap
  * @return bool
  */
bool ScriptingService :: noteExistsByFileName (QString bestandsnaam,
                                             int ignoreNoteId,
                                             int noteSubFolderId);
```

### Voorbeeld
```js
// controleer of opmerking bestaat, maar negeer de id van "note"
script.noteExistsByFileName("my note.md", note.id);
```

Misschien wilt u het voorbeeld eens bekijken [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Tekst naar het klembord kopiëren
-------------------------------

### Methodeaanroep en parameters
```cpp
/**
 * Kopieert tekst naar het klembord als platte tekst of html mime-gegevens
 *
 * @param tekst string tekst om op het klembord te plaatsen
 * @param asHtml bool indien waar, wordt de tekst ingesteld als html mime-gegevens
 */
void ScriptingService::setClipboardText(QString-tekst, bool asHtml);
```

### Voorbeeld
```js
// kopieer tekst naar het klembord
script.setClipboardText("text to copy");
```

Misschien wilt u het voorbeeld eens bekijken [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Naar een notitie springen
-----------------

### Methodeaanroep en parameters
```cpp
/**
  * Stelt de huidige notitie in als de notitie zichtbaar is in de notitielijst
  *
  * @param note Note Api-notitie om naar te springen
  */
void ScriptingService::setCurrentNote(NoteApi *note);
```

### Voorbeeld
```js
// spring naar de notitie
script.setCurrentNote (opmerking);
```

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Jumping to a note subfolder
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

Showing an information message box
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

Showing a question message box
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

Showing an open file dialog
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

Showing a save file dialog
--------------------------

### Method call and parameters
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

Registering script settings variables
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

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
 * default values depending on the operating system.
 */
function registerSettingsVariables() {
    if (script.platformIsWindows()) {
        // override the myFile default value
        settingsVariables[3].default = "pandoc.exe"
    }
}
```

You may also want to take a look at the example [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

Storing and loading persistent variables
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

Loading application settings variables
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

Creating a cache directory
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

Clearing a cache directory
--------------------------

U kunt de cachemap van uw script wissen door de naam door te geven aan clearCacheDir().

### Methodeaanroep en parameters
```cpp
/**
  * Wist de cachemap voor een script
  *
  * @param {QString} subDir de submap die moet worden gewist
  * @return {bool} true bij succes
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

Als u het pad naar de directory waar uw script is geplaatst, nodig heeft om bijvoorbeeld andere bestanden te laden, moet u een `property string scriptDirPath;` registreren. Deze eigenschap wordt ingesteld met het pad naar de directory van het script.

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
  * Geeft pad terug met de '/' scheidingstekens geconverteerd naar scheidingstekens die zijn
  * geschikt voor het onderliggende besturingssysteem.
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
 * Geeft het pad terug met '/' als bestandseparator.
 * Op Windows, bijvoorbeeld fromNativeDirSeparators("c:\\winnt\\system32")
  * geeft "c: / winnt / system32" terug.
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
  * Retourneert het native directoryscheidingsteken "/" of "\" in Windows
  *
  * @return
  */
QString ScriptingService :: dirSeparator ();
```

### Voorbeeld
```js
// retourneert "\" in Windows
script.log(script.dirSeparator());
```

Een lijst ophalen met de paden van alle geselecteerde notities
-------------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Retourneert een lijst met de paden van alle geselecteerde notities
  *
  * @return {QStringList} lijst met geselecteerde notitiepaden
  */
QStringList ScriptingService::selectedNotesPaths();
```

### Voorbeeld
```js
// retourneert een lijst met de paden van alle geselecteerde notities
script.log(script.selectedNotesPaths());
```

Misschien wilt u het voorbeeld eens bekijken [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Een lijst ophalen met de id's van alle geselecteerde notities
-----------------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Retourneert een lijst met de id's van alle geselecteerde notities
  *
  * @return {QList <int>} lijst met geselecteerde notitie-id's
  */
QList<int> ScriptingService::selectedNotesIds();
```

### Voorbeeld
```js
// retourneert een lijst met de paden van alle geselecteerde notities
script.log(script.selectedNotesIds());
```

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

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

Misschien wilt u het voorbeeld eens bekijken [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
U kunt de objectnamen van de menuactie ophalen van [hoofdvenster.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Zoek gewoon naar de Engelse menutitel. Houd er rekening mee dat deze teksten in de loop van de tijd kunnen veranderen.
:::

Een invoerdialoog openen met een selectievak
-----------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Opent een invoerdialoog met een selectievak
  *
  * @param titel {QString} titel van het dialoogvenster
  * @param label {QString} labeltekst van het dialoogvenster
  * @param items {QStringList} lijst met te selecteren items
  * @param huidige {int} index van het item dat moet worden geselecteerd (standaard: 0)
  * @param bewerkbaar {bool} indien true kan de tekst in het dialoogvenster worden bewerkt (standaard: false)
  * @return {QString} tekst van het geselecteerde item
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

Misschien wilt u het voorbeeld eens bekijken [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Een invoerdialoog openen met een regelbewerking
----------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Opent een invoerdialoog met een lijnbewerking
  *
  * @param titel {QString} titel van het dialoogvenster
  * @param label {QString} labeltekst van het dialoogvenster
  * @param tekst {QString} tekst in het dialoogvenster (optioneel)
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
**
  * Controleer of er een bestand bestaat
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

You can remotely control QOwnNotes by using `WebSocketServer`.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). You can test the socket server by connecting to it on [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

You can also listen to sockets with `WebSocket`. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library installed to use this. For example under Ubuntu Linux you can install `qml-module-qtwebsockets`.
