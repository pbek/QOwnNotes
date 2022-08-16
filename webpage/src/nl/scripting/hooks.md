# Hooks

onNoteStored
------------

### Methodeaanroep en parameters
```js
/ **
  * Deze functie wordt aangeroepen wanneer een notitie op schijf wordt opgeslagen
  * Je kunt opgeslagen notities niet wijzigen, dat zou een puinhoop zijn sindsdien
  * u bewerkt ze waarschijnlijk tegelijkertijd met de hand
  *
  * @param {NoteApi} notitie - het notitieobject van de opgeslagen notitie
  * /
functie onNoteStored (Notizie);
```

Misschien wilt u het voorbeeld eens bekijken [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteOpenedHook
--------------

### Methodeaanroep en parameters
```js
/ **
  * Deze functie wordt aangeroepen nadat een notitie is geopend
  *
  * @param {NoteApi} note - het notitieobject dat werd geopend
  * /
functie noteOpenedHook (note);
```

Misschien wilt u het voorbeeld eens bekijken [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteDoubleClickedHook
---------------------

### Methodeaanroep en parameters
```js
/ **
    * Deze functie wordt aangeroepen nadat een notitie is geopend
    *
    * @param {NoteApi} note - het notitieobject dat werd geopend
    * /
functie noteOpenedHook (note);
```

Misschien wilt u het voorbeeld eens bekijken [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml).

insertMediaHook
---------------

Deze functie wordt aangeroepen wanneer een mediabestand in de huidige notitie wordt ingevoegd.

Als deze functie in meerdere scripts is gedefinieerd, wint het eerste script dat een niet-lege tekenreeks retourneert.

### Methodeaanroep en parameters
```js
// **
  * @param fileName string het bestandspad van het bronmediabestand voordat het naar de mediamap werd gekopieerd
  * @param markdownText string de markdown-tekst van het mediabestand, bijv. ![mijn-afbeelding] ![my-image](media/my-image-4101461585.jpg)
  * @return string de nieuwe markdown-tekst van het mediabestand
  * /
functie insertMediaHook (bestandsnaam, markdownText);
```

Misschien wilt u het voorbeeld eens bekijken [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertAttachmentHook
--------------------

Deze functie wordt aangeroepen wanneer een mediabestand in de huidige notitie wordt ingevoegd.

Als deze functie in meerdere scripts is gedefinieerd, wint het eerste script dat een niet-lege tekenreeks retourneert.

### Methodeaanroep en parameters
```js
/**
  * @param fileName string het bestandspad van het bronbijlagebestand voordat het naar de bijlagemap werd gekopieerd
  * @param markdownText string de markdown-tekst van het bijlagebestand, bijv. [mijn-bestand.txt] (bijlagen / mijn-bestand-4245650967.txt)
  * @return string de nieuwe markdown-tekst van het bijlagebestand
  */
functie insertAttachmentHook (bestandsnaam, markdownText);
```

Misschien wilt u het voorbeeld eens bekijken [ voorbeeld.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertingFromMimeDataHook
-------------------------

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen wanneer html of een mediabestand in een notitie wordt geplakt met `Ctrl + Shift + V`
  *
  * @ parametertekst tekst van het QMimeData-object
  * @param html html van het QMimeData-object
  * @ retourneert de tekenreeks die moet worden ingevoegd in plaats van de tekst uit het QMimeData-object
  */
functie insertingFromMimeDataHook (tekst, html);
```

Misschien wilt u het voorbeeld [ example.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml), [ insert-headline-with-link-from-github-url.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) of [ note-text bekijken -vanaf-17.00 uur-mail.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-text-from-5pm-mail.qml).

handleNoteTextFileNameHook
--------------------------

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen wanneer een notitie op schijf wordt opgeslagen als
  * "Toestaan dat de naam van het notitiebestand anders is dan de kop" is ingeschakeld
  * in de instellingen
  *
  * Hiermee kunt u de naam van het notitiebestand wijzigen
  * Houd er rekening mee dat u zelf voor dubbele namen moet zorgen!
 *
  * Retourneer een lege tekenreeks als de bestandsnaam van de notitie zou moeten
  * niet worden gewijzigd
  *
  * @param {NoteApi} note - het notitieobject van de opgeslagen notitie
  * @return {string} de bestandsnaam van de notitie
  */
functie handleNoteTextFileNameHook (opmerking);
```

Misschien wilt u het voorbeeld eens bekijken [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) of [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

handleNoteNameHook
------------------

### Methodeaanroep en parameters
```js
/ **
  * Deze functie wordt aangeroepen wanneer de nootnaam voor een noot wordt bepaald
  *
  * Hiermee kunt u de naam wijzigen van de notitie die wordt bekeken
  *
  * Retourneer een lege tekenreeks als de naam van de notitie niet moet worden gewijzigd
  *
  * @param {NoteApi} note - het notitieobject van de opgeslagen notitie
  * @return {string} de naam van de notitie
  * /
functie handleNoteNameHook (opmerking);
```

Misschien wilt u het voorbeeld eens bekijken [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

Het is misschien geen goed idee om deze hook te gebruiken als de instelling om de bestandsnaam als nootnaam te gebruiken actief is.

handleNewNoteHeadlineHook
-------------------------

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen voordat een noot wordt gemaakt
  *
  * Hiermee kunt u de kop van de notitie wijzigen voordat deze wordt gemaakt
  * Houd er rekening mee dat u anders moet zorgen voor een unieke nootnaam
  * de nieuwe notitie wordt niet gemaakt, deze wordt alleen in de notitielijst gevonden
  *
  * U kunt deze functie gebruiken voor het maken van notitiesjablonen
  *
  * @param koptekst die zou worden gebruikt om de kop te maken
  * @return {string} de kop van de notitie
  */
function handleNewNoteHeadlineHook(headline);
```

Misschien wil je het voorbeeld [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml) bekijken.

preNoteToMarkdownHtmlHook
-------------------------

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen voordat de markdown-html van een notitie wordt gegenereerd
  *
  * Hiermee kunt u wijzigen wat wordt doorgegeven aan de markdown naar html-converter
  *
  * De functie kan bijvoorbeeld in meerdere scripts worden gebruikt om code weer te geven (zoals LaTeX wiskunde of zeemeermin)
  * naar de grafische weergave voor de preview
  *
  * De notitie wordt tijdens dit proces niet gewijzigd
  *
  * @param {NoteApi} notitie - het notitieobject
  * @param {string} markdown - de markdown die op het punt staat te worden geconverteerd naar html
  * @param {bool} forExport - waar als de html wordt gebruikt voor een export, onwaar voor het voorbeeld
  * @return {string} de gewijzigde afwaardering of een lege tekenreeks als er niets moet worden gewijzigd
  */
functie preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Misschien wilt u het voorbeeld eens bekijken [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

noteToMarkdownHtmlHook
----------------------

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen wanneer de markdown-html van een notitie wordt gegenereerd
  *
  * Hiermee kunt u deze html wijzigen
  * Dit wordt bijvoorbeeld eerder aangeroepen door de notitievoorbeeld
  *
  * De functie kan in meerdere scripts worden gebruikt om de html van de preview te wijzigen
  *
  * @param {NoteApi} notitie - het notitieobject
  * @param {string} html - de html die op het punt staat te worden weergegeven
  * @param {bool} forExport - waar als de html wordt gebruikt voor een export, onwaar voor het voorbeeld
  * @return {string} de gewijzigde html of een lege string als er niets gewijzigd moet worden
  */
functie noteToMarkdownHtmlHook(note, html, forExport);
```

Misschien wilt u het voorbeeld eens bekijken [voorbeeld.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) of [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

Raadpleeg de [ondersteunde HTML Subset](http://doc.qt.io/qt-5/richtext-html-subset.html) documentatie voor een lijst met alle ondersteunde css-stijlen.

encryptionHook
--------------

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen wanneer tekst moet worden gecodeerd of gedecodeerd
  *
  * @param text string de tekst die moet worden versleuteld of ontsleuteld
  * @param wachtwoord tekenreeks het wachtwoord
  * @param decrypt bool als valse codering vereist is, als echte decodering vereist is
  * @return de versleutelde ontsleutelde tekst
  */
functie encryptionHook (tekst, wachtwoord, decoderen);
```

Misschien wilt u het voorbeeld eens bekijken [encryptie-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml), [encryptie-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml) of [encryptie-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml).

noteTaggingHook
---------------

U kunt uw eigen tagging-mechanisme voor notities implementeren met bijvoorbeeld speciale tekst in uw notitie, zoals `@tag1`, `@tag2`, `@tag3`.

### Methodeaanroep en parameters
```js
/ **
  * Verwerkt notitie-tagging voor een notitie
  *
  * Deze functie wordt aangeroepen wanneer tags worden toegevoegd aan, verwijderd uit of hernoemd in
  * een notitie of de tags van een notitie moeten worden vermeld
  *
  * @param opmerking
  * @param-actie kan zijn "toevoegen", "verwijderen", "hernoemen" of "lijst"
  * @param tagName tag naam die moet worden toegevoegd, verwijderd of hernoemd
  * @param newTagName tagnaam die moet worden hernoemd als action = "hernoemen"
  * @return opmerking tekstreeks of string-lijst met tagnamen (if action = "list")
  * /
functie noteTaggingHook (note, action, tagName, newTagName);
```

-   zodra een script is geactiveerd dat de nieuwe functie `noteTaggingHook` implementeert, wordt het taggen van opmerkingen door die functie afgehandeld
-   zodra een script is geactiveerd dat de nieuwe functie <0>noteTaggingHook</0> implementeert, wordt het taggen van opmerkingen door die afgehandeld
    -   in eerste instantie tags zoals `@tag` uit je notities importeren en je huidige tag-toewijzing overschrijven
        -   je raakt je tags-boom niet kwijt, alleen de vorige toewijzing aan notities
        -   u kunt nog steeds tags naar andere tags verplaatsen
        -   als meer dan één tag dezelfde naam heeft in uw tagboom, wordt de eerste treffer toegewezen
    -   door een tag aan een notitie toe te voegen, wordt de tag aan de notitietekst toegevoegd
    -   door een tag aan een notitie toe te voegen, wordt de tag aan de notitietekst toegevoegd
    -   als u tags in de taglijst verwijdert, worden die tags uit uw notities verwijderd
    -   het hernoemen van tags in de tag-lijst zal de naam van die tags in uw notities
    -   bulksgewijs taggen van notities in de notitielijst zal die tags aan uw notities
    -   het bulksgewijs verwijderen van tags uit notities in de notitielijst zal die tags uit uw notities verwijderen
    -   de applicatie activeert een reeks `toevoegen` en `verwijderen` acties voor alle geselecteerde tags en hun kinderen op alle notities als tags worden verplaatst in het tagpaneel

Vielleicht möchten Sie sich das Beispiel ansehen [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml) um Ihren eigenen Tagging-Mechanismus zu implementieren.

::: warning
Zorg ervoor dat uw `lijst` -actie echt snel is, want deze wordt voor elke notitie uitgevoerd telkens wanneer de notitiemap opnieuw wordt geladen!
:::

noteTaggingByObjectHook
----------------------

Net als bij [noteTaggingHook](#notetagginghook) kunt u uw eigen mechanisme voor het taggen van notities implementeren, maar u bent niet gebonden aan het taggen van namen in de root van de tagboom. Op deze manier kunt u gebruik maken van de hele tagboom in plaats van alleen een taglijst.

Met `noteTaggingByObjectHook` krijg je een `TagApi` -object als parameter, in plaats van een tagnaam. En als resultaat voor de `list` -actie moet u een lijst met tag-id's.

Dit betekent ook dat u zelf ontbrekende tags moet maken om een lijst met reeds bestaande tag-ID's voor de `list` -actie te kunnen leveren.

### Methodeaanroep en parameters
```js
/ **
  * Verwerkt notitie-tagging voor een notitie
  *
  * Deze functie wordt aangeroepen wanneer tags worden toegevoegd aan, verwijderd uit of hernoemd in
  * een notitie of de tags van een notitie moeten worden vermeld
  *
  * @param opmerking
  * @param-actie kan zijn "toevoegen", "verwijderen", "hernoemen" of "lijst"
  * @param-tag die moet worden toegevoegd, verwijderd of hernoemd
  * @param newTagName tagnaam die moet worden hernoemd als action = "hernoemen"
  * @return notitie tekstreeks of string-lijst met tag-ID's (if action = "list")
  * /
functie noteTaggingByObjectHook (opmerking, actie, tag, newTagName);
```

Misschien wil je het voorbeeld [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml) bekijken om je eigen tagging-mechanisme te implementeren.

autocompletionHook
------------------

U kunt een lijst met tekenreeksen retourneren die aan de autocomplete-lijst moeten worden toegevoegd wanneer ze automatisch worden aangeroepen (bijvoorbeeld door op <kbd>Ctrl + Space</kbd> te drukken).

### Methodeaanroep en parameters
```js
/**
  * Roept de functie autocompletionHook aan voor alle scriptcomponenten
  * Deze functie wordt aangeroepen wanneer automatische aanvulling wordt aangeroepen in een notitie
  *
  * @return QStringLijst met tekst voor de autocomplete lijst
  */
functie callAutocompletionHook ();
```

Misschien wilt u het voorbeeld eens bekijken [automatisch aanvullen.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

websocketRawDataHook
--------------------

Deze hook wordt aangeroepen wanneer gegevens worden verzonden vanuit de QOwnNotes Web Companion-browserextensie via het contextmenu van de webbrowser.

### Methodeaanroep en parameters
```js
/**
  * @param requestType kan "pagina" of "selectie" zijn
  * @param pageUrl de url van de webpagina waar het verzoek is gedaan
  * @param page Titel de paginatitel van de webpagina waar het verzoek is gedaan
  * @param rawData de gegevens die zijn verzonden, html voor requestType "page" of platte tekst voor requestType "selection"
  * @param screenshotDataUrl de data-url van de screenshot als de webpagina waar het verzoek is gedaan
  * @return waar als gegevens werden afgehandeld door een haak
  */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

Misschien wilt u de voorbeelden eens bekijken [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-new-note.qml) en [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

onDetachedProcessCallback
-------------------------

Deze hook wordt aangeroepen wanneer een scriptthread van [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) klaar is met uitvoeren.

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen wanneer een scriptthread klaar is met uitvoeren.
 * Hint: thread [1]==0 helpt om te bepalen of een groot deel van de gestarte processen voor een bepaalde identifier is voltooid.
 *
  * @param {QString} callbackIdentifier - de opgegeven id bij het aanroepen van startDetachedProcess ()
  * @param {QString} resultSet - het resultaat van het proces
  * @param {QVariantList} cmd - de volledige commando-array [0-executablePath, 1-parameters, 2-exitCode]
  * @param {QVariantList} thread - de thread-informatie-array [0-doorgegeven callbackParameter, 1-resterende threads voor deze identifier]
  * /
functie onDetachedProcessCallback (callbackIdentifier, resultSet, cmd, thread);
```

Misschien wilt u het voorbeeld eens bekijken [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml).

windowStateChangedHook
--------------

### Methodeaanroep en parameters
```js
/**
  * Deze functie wordt aangeroepen nadat een WindowStateChange-gebeurtenis is geactiveerd
  *
  * @param {QString} windowState - de nieuwe vensterstatus, parameterwaarde kan "geminimaliseerd", "gemaximaliseerd", "volledig scherm", "actief" of "nostate" zijn
  */
functie windowStateChangedHook (windowState);
```

Misschien wilt u het voorbeeld eens bekijken [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/window-state-changed.qml).

workspaceSwitchedHook
----------------------

This hook is called when workspaces are switched.

### Methodeaanroep en parameters
```js
/**
 * This function is called when workspaces are switched
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

Misschien wil je het voorbeeld bekijken [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/workspaces.qml).
