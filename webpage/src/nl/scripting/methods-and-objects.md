# Methoden en objecten die QOwnNotes biedt

## Het starten van een extern programma op de achtergrond

### Methodeaanroep en parameters

```cpp
/**
 * QML wrapper om een losstaand proces te starten
 *
 * @param executablePath het pad van het uitvoerbare
 * @param parameters een lijst met parameterreeksen
 * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function (optional)
 * @param callbackParameter een extra parameter voor loops of iets dergelijks (optioneel)
 * @param processData gegevens die naar het proces worden geschreven als de callback wordt gebruikt (optioneel)
 * @param workingDirectory de werkdirectory om het proces in uit te voeren (optioneel, werkt alleen zonder callback)
 * @return true op succes, anders vals
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

Misschien wilt u het voorbeeld bekijken [custom-actions.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml), [callback.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) of [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml).

Misschien wil je ook eens kijken naar de [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) hook.

## Start een extern programma en wacht op de uitvoer

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
var result = script.startSynchronousProcess(
  "/path/to/my/program",
  ["my parameter"],
  "data",
  "/path/to/execute/in",
);
```

Misschien wilt u het voorbeeld bekijken [encryption-keybase.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml).

## Het pad ophalen van de huidige notitiemap

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

Misschien wilt u het voorbeeld bekijken [absolute-media-links.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml).

## De huidige notitie ophalen

### Methodeaanroep en parameters

```cpp
/**
  * QML-wrapper om de huidige notitie te krijgen
  *
  * @returns {NoteApi} het huidige notitieobject
  */
NoteApi currentNote();
```

### Voorbeeld

```js
var note = script.currentNote();
```

Misschien wilt u het voorbeeld bekijken [custom-actions.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Inloggen op de log-widget

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

## Het downloaden van een url naar een string

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

Misschien wilt u het voorbeeld bekijken [insert-headline-with-link-from-github-url.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

## Een url downloaden naar de mediamap

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
var markdown = script.downloadUrlToMedia(
  "http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}",
);
```

Misschien wilt u het voorbeeld bekijken [paste-latex-image.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml).

## Een mediabestand invoegen in de mediamap

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

Misschien wilt u het voorbeeld bekijken [scribble.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## Een bijlagebestand invoegen in de bijlagenmap

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

## Het notitievoorbeeld opnieuw genereren

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

Misschien wilt u het voorbeeld bekijken [scribble.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## Een aangepaste actie registreren

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
You can also assign local and global shortcuts to your custom actions in the _Shortcuts settings_.
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

Zie voor meer voorbeelden [custom-actions.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
U kunt ook een aangepaste actie activeren nadat de toepassing is gestart met de parameter `--action customAction_<identifier>`. Kijk voor meer informatie op [Activeer menuacties na opstarten](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

## Een label registreren

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
script.registerLabel(
  "html-label",
  "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.",
);

script.registerLabel(
  "long-label",
  "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap",
);

script.registerLabel("counter-label");
```

The labels will be visible in the _Scripting panel_, which you need to enable in the _Window / Panels_ menu.

U kunt zowel platte tekst als html in de labels gebruiken. De tekst kan worden geselecteerd en er kan op links worden geklikt.

Misschien wilt u dan het voorbeeldscript bekijken [script-label-demo.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

## De tekst van een geregistreerd label instellen

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

Misschien wilt u dan het voorbeeldscript bekijken [script-label-demo.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

::: tip
Het scriptlabel ondersteunt ook **externe links**, evenals **interne links** naar notities, taken, kaarten en meer. U kunt een idee krijgen van wat wordt ondersteund door een kijkje te nemen in [URL-behandeling](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

## Een nieuwe notitie maken

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

Misschien wilt u het voorbeeld bekijken [custom-actions.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
Als u hebt uitgeschakeld dat de kop van de notitie de bestandsnaam van de notitie bepaalt, dan bent moet uw notitiebestand daarna zelf hernoemen, zoals:

```js
var note = script.currentNote();
note.renameNoteFile("your-filename");
```

:::

## Toegang tot het klembord

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

Misschien wilt u het voorbeeld bekijken [custom-actions.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Schrijf tekst naar de tekst van de notitie

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

Misschien wilt u kijken naar de aangepaste actie `transformTextRot13 ` in voorbeeld [custom-actions.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

U kunt dit samen met `noteTextEditSelectAll` gebruiken om de hele tekst van de huidige notitie te overschrijven.

## Lees de geselecteerde tekst in de tekstbewerking van de notitie

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

Misschien wilt u kijken naar de aangepaste actie `transformTextRot13 ` in voorbeeld [custom-actions.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Selecteer alle tekst in de tekstbewerking van de notitie

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

## Selecteer de huidige regel in de tekstbewerking van de notitie

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

## Selecteer de huidige woord in de tekstbewerking van de notitie

### Methodeaanroep en parameters

```cpp
/**
  * Selecteert het huidige woord in de notitietekstbewerking
  */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Voorbeeld

```js
script.noteTextEditSelectCurrentWord();
```

## Stel de momenteel geselecteerde tekst in bij het bewerken van de notitietekst

### Methodeaanroep en parameters

```cpp
/**
 * Stelt de momenteel geselecteerde tekst in de notitietekstbewerking in
 *
 * @param start
 * @param einde
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Voorbeeld

```js
// expands the current selection by one character
script.noteTextEditSetSelection(
  script.noteTextEditSelectionStart() - 1,
  script.noteTextEditSelectionEnd() + 1,
);
```

## Verkrijg de startpositie van de huidige selectie in de notitietekst

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

## Haal de eindpositie van de huidige selectie op in de tekstbewerking van de notitie

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

## Plaats de tekstcursor op een bepaalde positie in de notitie-bewerking

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
// spring naar het 11e teken in de notitie
script.noteTextEditSetCursorPosition(10);

// spring naar het einde van de notitie
script.noteTextEditSetCursorPosition(-1);
```

## Haal de huidige positie van de tekstcursor op in de tekstbewerking van de notitie

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

## Lees het huidige woord uit de tekst van de notitie

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

Misschien wilt u het voorbeeld bekijken [autocompletion.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Lees het huidige blok van de notitietekstbewerking

### Methodeaanroep en parameters

```cpp
/**
 * Leest het huidige blok in de notitietekstbewerking
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### Voorbeeld

```js
// Lees het huidige blok in de notitie tekstbewerking
var text = script.noteTextEditCurrentBlock();
```

Bestudeer het voorbeeld eens [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Gebruik een voltooiingsprompt op het geselecteerde AI-model

De AI-voltooiingsprompt is een functie waarmee je een voltooiingsprompt kunt gebruiken op het momenteel geselecteerde AI-model, zoals ChatGPT, Claude of systemen zoals Ollama.

Om dit te laten werken moet het AI-systeem worden ingeschakeld in de AI-werkbalk of het hoofdmenu.

### Methodeaanroep en parameters

```cpp
/**
 * QML-wrapper om de AI-completering te gebruiken
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### Voorbeeld

```js
// Vraag het geselecteerde AI-model om de prompt te voltooien
var text = script.aiComplete("Tell me how do you feel today?");
```

Bestudeer het voorbeeld eens [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Controleer of het platform Linux, OS X of Windows is

### Methodeaanroep en parameters

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Voorbeeld

```js
if (script.platformIsLinux()) {
  // Will be executed only if under Linux
}
```

## Tag de huidige notitie

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

Misschien wilt u kijken naar de aangepaste actie `favoriteNote ` in voorbeeld [favorite-note.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

## Maak of haal een tag op met de naam van de broodkruimellijst

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

## Zoek naar tags op naam

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

Misschien wilt u het voorbeeld bekijken [autocompletion.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Zoek naar notities op notitietekst

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

noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // do something with the note
});
```

Misschien wilt u het voorbeeld bekijken [unique-note-id.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml).

## Voeg een aangepast stylesheet toe

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

Misschien wilt u het voorbeeld bekijken [custom-stylesheet.qml ](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml).

U kunt de widgetnamen ophalen uit de ` *.ui ` -bestanden, bijvoorbeeld het hoofdvenster [mainwindow.ui ](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui).

De Qt-documentatie (bijvoorbeeld [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) kan u helpen te zien hoe de widgets aan elkaar gerelateerd zijn (zoek naar `Erfenissen` op de pagina's).

De basiswidget voor bijna alles is [QWidget](https://doc.qt.io/qt-5/qwidget.html). Dus gewoon `QWidget` stylen met bijvoorbeeld `QWidget {background-color: black; kleur: wit;}` zou betekenen dat alles een zwarte achtergrondkleur en een witte voorgrondkleur heeft.

::: tip
CSS [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) van [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) kan ook een goede referentie zijn voor stijlen die u kunt wijzigen.
:::

Bekijk [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) voor een referentie van welke stijlen beschikbaar zijn.

Als je stijlen in html-preview wilt invoegen om de manier waarop notities worden weergegeven te wijzigen, kijk dan naar [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
Als je echt wilt zien hoe de dialogen eruitzien en hoe ze heten, kun je [Qt Creator](https://www.qt.io/product/development-tools) downloaden en de `*.ui`-bestanden erin openen.
:::

## Herladen van de script-engine

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

## Een notitie ophalen op basis van de bestandsnaam

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

## Een notitie ophalen op basis van zijn ID

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

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Controleren of een notitie bestaat op basis van de bestandsnaam

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

Misschien wilt u het voorbeeld eens bekijken [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## Tekst naar het klembord kopiëren

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

Misschien wilt u het voorbeeld eens bekijken [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml).

## Naar een notitie springen

### Methodeaanroep en parameters

```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 * @param asTab bool if true the note will be opened in a new tab (if not already open)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Voorbeeld

```js
// jump to the note
script.setCurrentNote(note);

// open note in new tab (if not already open)
script.setCurrentNote(note, true);
```

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml).

## Springen naar een submap van een notitie

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

## Een informatieberichtvenster weergeven

### Methodeaanroep en parameters

```cpp
/**
 * Toont een informatieberichtvenster
 *
 * @param tekst
 * @param title (optioneel)
 */
void ScriptingService:: informationMessageBox (QString-tekst, QString-titel);
```

### Voorbeeld

```js
// toon een informatieberichtvenster
script.informationMessageBox ("De tekst die ik wil laten zien", "Een optionele titel");
```

## Er wordt een berichtvenster met een vraag weergegeven

### Methodeaanroep en parameters

```cpp
/**
  * Toont een vraagberichtvenster
  *
  * Voor informatie over knoppen zie:
  * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
  *
  * @param tekst
  * @param titel (optioneel)
  * @param knoppen knoppen die getoond moeten worden (optioneel)
  * @param defaultButton standaardknop die wordt geselecteerd (optioneel)
  * @return id van ingedrukte knop
 */
int ScriptingService::questionMessageBox(
        QString text, QString title, int buttons, int defaultButton);
```

### Voorbeeld

```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
  "The text I want to show",
  "Some optional title",
  0x01000000 | 0x02000000,
  0x02000000,
);
script.log(result);
```

Zie voor informatie over knoppen [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Zie voor informatie over knoppen [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Een open bestandsdialoog weergeven

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
// show an open file dialog
var fileName = script.getOpenFileName(
  "Please select an image",
  "/home/user/images",
  "Images (*.png *.xpm *.jpg)",
);
```

## Een zekere bestandsdialoog weergeven

### Methodeaanroep en parameters

```cpp
/**
 * Toont een dialoogvenster voor het opslaan van bestanden
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
var fileName = script.getSaveFileName(
  "Please select HTML file to save",
  "output.html",
  "HTML (*.html)",
);
```

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Variabelen voor scriptinstellingen registreren

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

De gebruiker kan deze eigenschappen vervolgens instellen in de scriptinstellingen.

### Voorbeeld

```js
// you have to define your registered variables so you can access them later
property string myString;
property string myStringSecret;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string myDirectory;
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
        "identifier": "myStringSecret",
        "name": "I am a password field",
        "description": "Please enter a valid string:",
        "type": "string-secret",
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
        "identifier": "myDirectory",
        "name": "I am a directory selector",
        "description": "Please select the directory:",
        "type": "directory",
        "default": "/home",
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

Daarnaast kunt u de `settingsVariables` als volgt overschrijven met een speciale functie `registerSettingsVariables()`:

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
    settingsVariables[3].default = "pandoc.exe";
  }
}
```

Misschien wilt u het voorbeeld eens bekijken [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

## Persistente variabelen opslaan en laden

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
script.log(
  script.getPersistentVariable(
    "PersistentVariablesTest/myVar",
    "nothing here yet",
  ),
);
```

Zorg ervoor dat u een betekenisvol voorvoegsel in uw sleutel gebruikt, zoals `PersistentVariablesTest/myVar`, omdat de variabelen toegankelijk zijn vanuit alle scripts.

Misschien wilt u het voorbeeld eens bekijken [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

## Variabelen voor applicatie-instellingen laden

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

Houd er rekening mee dat instellingen eigenlijk leeg kunnen zijn, daar moet u zelf voor zorgen. `defaultValue` wordt alleen gebruikt als de instelling helemaal niet bestaat.

## Een cachemap maken

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

## Een cachemap wissen

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

## Het pad naar de directory van uw script lezen

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

## Padscheidingstekens omzetten in native

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

## Padscheidingstekens omzetten van native

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

## Het native directoryscheidingsteken ophalen

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

## Een lijst ophalen met de paden van alle geselecteerde notities

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

Misschien wilt u het voorbeeld eens bekijken [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml).

## Een lijst ophalen met de id's van alle geselecteerde notities

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

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Een menuactie activeren

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
// schakel de alleen-lezen modus in
script.triggerMenuAction("actionAllow_note_editing");

// de alleen-lezen modus uitschakelen
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Misschien wilt u het voorbeeld eens bekijken [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
U kunt de objectnamen van de menuactie ophalen van [hoofdvenster.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Zoek gewoon naar de Engelse menutitel. Houd er rekening mee dat deze teksten in de loop van de tijd kunnen veranderen.
:::

## Een invoerdialoog openen met een selectievak

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

Er wordt een lege tekenreeks geretourneerd als op `Annuleren` is geklikt of `Escape` is getoetst.

### Voorbeeld

```js
var result = script.inputDialogGetItem("combo box", "Please select an item", [
  "Item 1",
  "Item 2",
  "Item 3",
]);
script.log(result);
```

Misschien wilt u het voorbeeld eens bekijken [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Een invoerdialoog openen met een regelbewerking

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

Er wordt een lege tekenreeks geretourneerd als op `Annuleren` is geklikt of `Escape` is getoetst.

### Voorbeeld

```js
var result = script.inputDialogGetText(
  "line edit",
  "Please enter a name",
  "current text",
);
script.log(result);
```

## Een invoerdialoog openen met een tekstbewerking met meerdere regels

### Methodeaanroep en parameters

```cpp
/**
 * Opens an input dialog with a multi-line text edit
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetMultiLineText(
        const QString &title, const QString &label, const QString &text);
```

Er wordt een lege tekenreeks geretourneerd als op `Annuleren` is geklikt of `Escape` is getoetst.

### Voorbeeld

```js
var result = script.inputDialogGetMultiLineText(
  "multi-line edit",
  "Please enter a text",
  "current text",
);
script.log(result);
```

## Een dialoogvenster openen om de verschillen tussen twee teksten weer te geven

### Methodeaanroep en parameters

```cpp
/**
* Opent een dialoogvenster om de verschillen tussen twee teksten weer te geven en laat de gebruiker het resultaat bewerken
*
* @param titel {QString} titel van het dialoogvenster
* @param label {QString} label tekst van het dialoogvenster
* @param tekst1 {QString} eerste tekst
* @param tekst2 {QString} tweede tekst
* @return
  */
  QString ScriptingService::textDiffDialog(const QString &title, const QString &label,
                                           const QString & text1, const QString &text2);
```

`tekst2` is de tekst die je in het dialoogvenster kunt bewerken. Er wordt een lege tekenreeks geretourneerd als er op `Annuleren` is geklikt of op `Escape` is gedrukt.

### Voorbeeld

```js
const text = script.noteTextEditSelectedText();
const aiPrompt = "Translate the text to English";
const aiResult = script.aiComplete(aiPrompt + ":\n\n" + text);

var result = script.textDiffDialog(
  "AI Text Tool",
  "Resulting text",
  text,
  aiResult,
);
script.log(result);
```

## Controleren of er een bestand bestaat

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

## Tekst uit een bestand lezen

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
if (script.fileExists(filePath)) {
  var data = script.readFromFile(filePath);
  script.log(data);
}
```

## Tekst naar een bestand schrijven

### Methodeaanroep en parameters

```cpp
/**
 * Schrijft een tekst naar een bestand
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

Misschien wilt u het voorbeeld eens bekijken [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Werken met websockets

U kunt QOwnNotes op afstand bedienen met `WebSocketServer`.

Bekijk het voorbeeld eens [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml). U kunt de socketserver testen door er verbinding mee te maken op [Websocket-test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

U kunt ook naar sockets luisteren met `WebSocket`. Misschien wilt u dit voorbeeld bekijken [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml).

Onthoud dat u Qt's QML `websocket`-bibliotheek moet hebben geïnstalleerd om dit te gebruiken. U kunt bijvoorbeeld onder Ubuntu Linux installeren `qml-module-qtwebsockets`.

## Een markeringsregel toevoegen voor de editor

U kunt markeringsregels rechtstreeks in de editor invoegen door reguliere expressies te definiëren en deze toe te wijzen aan een markeringsstatus.

### Methodeaanroep en parameters

```cpp
/**
  * Voegt een markeringsregel toe aan de syntaxismarkering van de editor
  *
  * @param patroon {QString} het reguliere expressiepatroon om te markeren
  * @param shouldContain {QString} een tekenreeks die in de gemarkeerde tekst moet staan om het patroon te kunnen ontleden
  * @param state {int} de staat van de te gebruiken syntax highlighter
  * @param captureGroup {int} de capture-groep voor het patroon om te gebruiken voor markering (standaard: 0)
  * @param maskedGroup {int} de vastleggroep voor het patroon om te gebruiken voor maskering (standaard: 0)
  */
void ScriptingService::addHighlightingRule(const QString &patroon,
                                            const QString &shouldContain,
                                            int staat,
                                            int captureGroup,
                                            int maskedGroup);
```

### Statussen markeren

| Naam                       | Nr. |
| -------------------------- | --- |
| NoState                    | -1  |
| Koppeling                  | 0   |
| Afbeelding                 | 3   |
| CodeBlock                  | 4   |
| CodeBlockComment           | 5   |
| Cursief                    | 7   |
| Vet                        | 8   |
| Lijst                      | 9   |
| Opmerking                  | 11  |
| H1                         | 12  |
| H2                         | 13  |
| H3                         | 14  |
| H4                         | 15  |
| H5                         | 16  |
| H6                         | 17  |
| BlockQuote                 | 18  |
| HorizontalRuler            | 21  |
| Table                      | 22  |
| InlineCodeBlock            | 23  |
| MaskedSyntax               | 24  |
| CurrentLineBackgroundColor | 25  |
| BrokenLink                 | 26  |
| FrontmatterBlock           | 27  |
| TrailingSpace              | 28  |
| CheckBoxUnChecked          | 29  |
| CheckBoxChecked            | 30  |
| StUnderline                | 31  |

### Voorbeeld

```js
// Markeer een tekstregel zoals "BLOCK: some text" als blockquote (status 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Maskeer alle tekens na 32 tekens op een regel uit (status 24)
// captureGroup 1 betekent dat de uitdrukking van het eerste deel van het patroon tussen haakjes wordt gemarkeerd
// maskedGroup -1 betekent dat er niet gemaskeerd moet worden
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

U kunt de voorbeelden ook bekijken in [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).
