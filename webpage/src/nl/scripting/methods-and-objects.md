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
Als je hebt uitgeschakeld dat de kop van de notitie de bestandsnaam van de notitie bepaalt, moet je de naam van je notitiebestand achteraf zelf wijzigen, als volgt:

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
  * Retourneert de inhoud van het klembord als tekst of html
  *
  * @param asHtml geeft de inhoud van het klembord terug als html in plaats van tekst
  */
QString ScriptingService :: clipboard (bool asHtml);
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

Misschien wilt u de aangepaste actie `transformTextRot13` in voorbeeld [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) bekijken.

U kunt dit samen met `noteTextEditSelectAll` gebruiken om de hele tekst van de huidige notitie te overschrijven.

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

Misschien wilt u de aangepaste actie `transformTextRot13` in voorbeeld [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) bekijken.

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

U kunt dit samen met `noteTextEditWrite` gebruiken om de hele tekst van de huidige notitie te overschrijven.

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
  * Stelt de momenteel geselecteerde tekst in de notitietekst in
  *
  * @param start
  * @param einde
  */
void ScriptingService :: noteTextEditSetSelection (int start, int end);
```

### Voorbeeld
```js
// breidt de huidige selectie uit met één teken
script.noteTextEditSetSelection (
     script.noteTextEditSelectionStart () - 1,
     script.noteTextEditSelectionEnd () + 1);
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
  * Zet de tekstcursor in de tekstbewerking van de notitie op een bepaalde positie
  * 0 zou het begin van de notitie zijn
  * speciaal geval: -1 zou het einde van de notitie zijn
  *
  * @param positie
  */
void ScriptingService :: noteTextEditSetCursorPosition (int positie);
```

### Voorbeeld
```js
// spring naar het 11e teken in de notitie
script.noteTextEditSetCursorPosition(10);

// spring naar het einde van de notitie
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
// lees het huidige woord in de tekst van de notitie
var text = script.noteTextEditCurrentWord();
```

Tag de huidige notitie
--------------------

### Methodeaanroep en parameters
```cpp
/**
  * Label de huidige notitie met een tag met de naam tagName
  *
  * @param tagName
  */
void ScriptingService :: tagCurrentNote (QString tagnaam);
```

### Voorbeeld
```js
// voeg een "favoriete" tag toe aan de huidige notitie
script.tagCurrentNote ("favoriet");
```

Misschien wilt u de aangepaste actie `favoriteNote` in het voorbeeld [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml) bekijken.

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
// maakt alle tags tot het 3e niveau en retourneert het tag-object voor
// tag "level3", die er zo uitziet in de tagboom:
// level1 & gt; level2 & gt; niveau 3
var tag = script.getTagByNameBreadcrumbList (["level1", "level2", "level3"]);
```

Zoek naar tags op naam
-----------------------

### Methodeaanroep en parameters
```cpp
/**
  * Haalt alle tags op door een substring-zoekopdracht uit te voeren in het naamveld
  *
  * @param naam {QString} naam om naar te zoeken
  * @return {QStringList} lijst met tagnamen
  */
QStringList ScriptingService :: searchTagsByName (QString-naam);
```

### Voorbeeld
```js
// zoekt naar alle tags met het woordspel erin
var tags = script.searchTagsByName("game");
```

Misschien wilt u het voorbeeld eens bekijken [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Zoek naar notities op notitietekst
-----------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Retourneert een lijst met notitie-id's van alle notities met een bepaalde tekst in de notitietekst
  *
  * Helaas is er geen gemakkelijke manier om een QList <NoteApi*> in QML te gebruiken, dus wij
  * kan alleen de notitie-ID's overdragen
  *
  * @return {QList <int>} lijst met notitie-id's
  */
QList <int> ScriptingService :: fetchNoteIdsByNoteTextPart (QString-tekst);
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

U kunt de objectnamen uit de `*.ui` -bestanden halen, bijvoorbeeld [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Bekijk [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) voor een referentie van welke stijlen beschikbaar zijn.

Als u stijlen in html-voorbeeld wilt injecteren om de manier waarop notities worden weergegeven te wijzigen, kijk dan naar [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

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
  * @param id int de id van de notitie
  * @return OpmerkingApi *
  */
NoteApi * ScriptingService :: fetchNoteById (int id);
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
/ **
  * Kopieert tekst naar het klembord als platte tekst of html mime-gegevens
  *
  * @param tekstreeks tekst om op het klembord te plaatsen
  * @param asHtml bool indien true wordt de tekst ingesteld als html mime-gegevens
  * /
void ScriptingService::setClipboardText(QString text, bool asHtml);
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

Springen naar een submap van een notitie
---------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Springt naar een submap voor notities
  *
  * @param noteSubFolderPath {QString} pad van de submap, relatief aan de notitiemap
  * @param scheidingsteken {QString} scheidingsteken tussen delen van het pad, standaard "/"
  * @return waar als de sprong succesvol was
  */
bool ScriptingService :: jumpToNoteSubFolder (const QString & amp; noteSubFolderPath,
                                             QString-scheidingsteken);
```

### Voorbeeld
```js
// spring naar de submap notitie "een submap"
script.jumpToNoteSubFolder ("een submap");

// spring naar de submap "sub" van de notitie in "een submap"
script.jumpToNoteSubFolder ("een submap / sub");
```

::: tip
U kunt een nieuwe submap voor notities maken in de huidige submap door [`mainWindow.createNewNoteSubFolder`](classes.html#example-2) aan te roepen.
:::

Een informatieberichtvenster weergeven
----------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Toont een informatieberichtvenster
  *
  * @param tekst
  * @param title (optioneel)
  */
void ScriptingService :: informationMessageBox (QString-tekst, QString-titel);
```

### Voorbeeld
```js
// toon een informatieberichtvenster
script.informationMessageBox ("De tekst die ik wil laten zien", "Een optionele titel");
```

Er wordt een berichtvenster met een vraag weergegeven
------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Toont een vraagberichtvenster
  *
  * Zie voor informatie over knoppen:
  * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
  *
  * @param tekst
  * @param title (optioneel)
  * @param buttons knoppen die getoond moeten worden (optioneel)
  * @param defaultButton standaardknop die wordt geselecteerd (optioneel)
  * @return id van ingedrukte knop
 */
int ScriptingService::questionMessageBox(
        QString text, QString title, int buttons, int defaultButton);
```

### Voorbeeld
```js
// toon een vraagberichtvenster met een sollicitatie- en een helpknop
// zie: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var resultaat = script.questionMessageBox (
    "De tekst die ik wil laten zien", "Een optionele titel", 0x01000000 | 0x02000000, 0x02000000);
script.log (resultaat);
```

Zie voor informatie over knoppen [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Zie voor informatie over knoppen [StandardButton](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Een open bestandsdialoog weergeven
---------------------------

### Methodeaanroep en parameters
```cpp
**
  * Toont een open bestandsdialoog
  *
  * @param caption (optioneel)
  * @param dir (optioneel)
  * @param filter (optioneel)
  * @return QString
  * /
QString ScriptingService::getOpenFileName(QString caption, QString dir,
                                            QString filter);
```

### Voorbeeld
```js
// toon een open bestandsdialoog
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Een zekere bestandsdialoog weergeven
--------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Toont een dialoogvenster voor het opslaan van bestanden
  *
  * @param caption (optioneel)
  * @param dir (optioneel)
  * @param filter (optioneel)
  * @return QString
  */
QString ScriptingService::getSaveFileName (QString caption, QString dir,
                                             QString-filter);
```

### Voorbeeld
```js
// toon een dialoogvenster voor het opslaan van bestanden
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Variabelen voor scriptinstellingen registreren
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

De gebruiker kan deze eigenschappen vervolgens instellen in de scriptinstellingen.

### Voorbeeld
```js
// u moet uw geregistreerde variabelen definiëren zodat u ze later kunt openen
property string myString;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string mySelection;

// registreer uw instellingsvariabelen zodat de gebruiker ze kan instellen in de scriptinstellingen
// gebruik deze eigenschap als je dat niet nodig hebt

// // helaas is er geen QVariantHash in Qt, we kunnen alleen
// QVariantMap (dat geen willekeurige volgorde heeft) of QVariantList
// (die op zijn minst willekeurig kan worden besteld)

eigenschapsvariantinstellingen Variabelen: [
     {
         "identifier": "myString",
         "name": "Ik ben een lijnbewerker",
         "description": "Voer een geldige string in:",
         "type": "string",
         "default": "Mijn standaardwaarde",
     },
     {
         "identifier": "myBoolean",
         "name": "Ik ben een selectievakje",
         "description": "Een beschrijving",
         "text": "Vink dit selectievakje aan",
         "type": "boolean",
         "default": true,
     },
     {
         "identifier": "myText",
         "name": "Ik ben tekstvak",
         "description": "Voer uw tekst in:",
         "typ Text",
         "default": "Dit kan een hele lange tekst zijn \ nmet meerdere regels.",
     },
     {
         "identifier": "myInt",
         "name": "Ik ben een nummerkiezer",
         "description": "Voer een getal in:",
         "type": "geheel getal",
         "standaard": 42,
     },
     {
         "identifier": "myFile",
         "name": "Ik ben een bestandskiezer",
         "description": "Selecteer het bestand:",
         "type": "bestand",
         "default": "pandoc",
     },
     {
         "identifier": "mySelection",
         "name": "Ik ben een itemkiezer",
         "description": "Selecteer een item:",
         "type": "selectie",
         "default": "option2",
         "items": {"option1": "Tekst voor optie 1", "option2": "Tekst voor optie 2", "option3": "Tekst voor optie 3"},
     }
];
```

Bovendien kun je de `settingsVariables` overschrijven met een speciale functie `registerSettingsVariables()` zoals deze:

### Voorbeeld
```js
/**
  * Registreert de instellingsvariabelen opnieuw
  *
  * Gebruik deze methode als u code wilt gebruiken om uw variabelen te overschrijven, zoals instelling
  * standaardwaarden zijn afhankelijk van het besturingssysteem.
 */
function registerSettingsVariables() {
    if (script.platformIsWindows()) {
        // override the myFile default value
        settingsVariables[3].default = "pandoc.exe"
    }
}
```

Misschien wilt u het voorbeeld eens bekijken [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

Persistente variabelen opslaan en laden
----------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Slaat een permanente variabele op
  * Deze variabelen zijn wereldwijd toegankelijk via alle scripts
  * Gebruik een betekenisvol voorvoegsel in uw sleutel, zoals "PersistentVariablesTest / myVar"
  *
  * @param sleutel {QString}
  * @param waarde {QVariant}
  */
void ScriptingService :: setPersistentVariable (const QString & amp; key,
                                                 const QVariant & waarde);

/**
  * Laadt een permanente variabele
  * Deze variabelen zijn wereldwijd toegankelijk via alle scripts
  *
  * @param sleutel {QString}
  * @param defaultValue {QVariant} retourwaarde als de instelling niet bestaat (optioneel)
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

Zorg ervoor dat u een betekenisvol voorvoegsel in uw sleutel gebruikt, zoals `PersistentVariablesTest/myVar`, omdat de variabelen toegankelijk zijn vanuit alle scripts.

Misschien wilt u het voorbeeld eens bekijken [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Variabelen voor applicatie-instellingen laden
--------------------------------------

### Methodeaanroep en parameters
```cpp
/**
  * Laadt een variabele voor toepassingsinstellingen
  *
  * @param sleutel {QString}
  * @param defaultValue {QVariant} retourwaarde als de instelling niet bestaat (optioneel)
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

Houd er rekening mee dat instellingen eigenlijk leeg kunnen zijn, daar moet u zelf voor zorgen. `defaultValue` wordt alleen gebruikt als de instelling helemaal niet bestaat.

Een cachemap maken
--------------------------

U kunt bestanden cachen op de standaard cachelocatie van uw systeem.

### Methodeaanroep en parameters
```cpp
/**
  * Retourneert een cachemap voor een script
  *
  * @param {QString} subDir de submap die moet worden gemaakt en gebruikt
  * @return {QString} het pad naar de cache-map
  */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Voorbeeld
```js
// maak de cachemap voor my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Een cachemap wissen
--------------------------

U kunt de cachebestanden van uw script wissen door de naam door te geven aan clearCacheDir().

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
importeer QtQml 2.0
importeer QOwnNotesTypes 1.0

Script {
     // het pad naar de directory van het script wordt hier ingesteld
     eigenschap string scriptDirPath;

     functie init () {
         script.log (scriptDirPath);
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
// retourneert "c:\winnt\system32" op Windows
script.log (script.toNativeDirSeparators ("c:/winnt/system32"));
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
// will return "\" on Windows
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
  * Activeert een menuactie
  *
  * @param objectName {QString} objectnaam van de actie die moet worden geactiveerd
  * @param aangevinkt {QString} activeert alleen de actie als aangevinkt-status is
  * anders dan deze parameter (optioneel, kan 0 of 1 zijn)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);

```

### Voorbeeld
```js
// schakel de alleen-lezen modus in
script.triggerMenuAction("actionAllow_note_editing");

// schakel de alleen-lezen modus uit
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Misschien wilt u het voorbeeld eens bekijken [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

U kunt de objectnamen van de menuactie ophalen uit [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

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
  * Lees tekst uit een bestand
  *
  * @param filePath {QString} pad van het te laden bestand
  * @param codec {QString} bestandscodering (standaard: UTF-8)
  * @return de bestandsgegevens of null als het bestand niet bestaat
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
  * Schrijft een tekst naar een bestand
  *
  * @param filePath {QString}
  * @param-gegevens {QString}
  * @param createParentDirs {bool} optioneel (standaard: false)
  * @return
  */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### Voorbeeld
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Werken met websockets
-----------------------

U kunt QOwnNotes op afstand bedienen met `WebSocketServer`.

Bekijk het voorbeeld eens [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). U kunt de socketserver testen door er verbinding mee te maken op [Websocket-test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

U kunt ook naar sockets luisteren met `WebSocket`. Kijk alstublieft naar de voorbeeld [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Onthoud dat u Qt's QML `websocket`-bibliotheek moet hebben geïnstalleerd om dit te gebruiken. U kunt bijvoorbeeld onder Ubuntu Linux installeren `qml-module-qtwebsockets`.
