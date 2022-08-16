# Hooks

onNoteStored
------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót akkor hívják fel, amikor egy jegyzet lemezre kerül
  * Nem lehet módosítani a tárolt jegyzeteket, az már rendetlenség lenne
  * nagy valószínűséggel egyszerre kézzel szerkeszti őket
  *
  * @param {NoteApi} megjegyzés - a tárolt jegyzet jegyzetobjektuma
  */
function onNoteStored(note);
```

Érdemes megnézni a példát [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteOpenedHook
--------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót egy jegyzet megnyitása után hívják meg
  *
  * @param {NoteApi} megjegyzés - a megnyitott jegyzetobjektum
  */
function noteOpenedHook(note);
```

Érdemes megnézni a példát [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteDoubleClickedHook
---------------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót akkor hívják meg, ha egy jegyzetre duplán kattintottak
  *
  * @param {NoteApi} jegyzet - a jegyzetre kattintott objektum
  */
function noteDoubleClickedHook (jegyzet);
```

Érdemes megnézni ezt a példát: [ external-note-open.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml).

insertMediaHook
---------------

Ezt a funkciót akkor hívják meg, amikor médiafájlt illesztenek az aktuális jegyzetbe.

Ha ezt a függvényt több szkript definiálja, akkor az első olyan szkript nyer, amely nem üres karakterláncot ad vissza.

### Módszerhívás és paraméterek
```js
/**
  * @param fileName karaktersorozat a forrás média fájl elérési útját, mielőtt átmásolták volna a média mappába
  * @param markdownText karaktersorozat a médiafájl jelölési szövege, pl.![my-image](media/my-image-4101461585.jpg)
  * @return karakterlánc a médiafájl új jelölési szövege
  */
function insertMediaHook(fileName, markdownText);
```

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertAttachmentHook
--------------------

Ezt a funkciót akkor hívják meg, ha egy mellékletfájlt illesztenek be az aktuális jegyzetbe.

Ha ezt a függvényt több szkript definiálja, akkor az első olyan szkript nyer, amely nem üres karakterláncot ad vissza.

### Módszerhívás és paraméterek
```js
/**
  * @param fileName karakterlánc a forrás melléklet fájljának elérési útja, mielőtt átmásolták volna a melléklet mappájába
  * @param markdownText karaktersorozat a mellékletfájl jelölési szövege, pl. [my-file.txt] (mellékletek / my-file-4245650967.txt)
  * @return karaktersorozat a mellékletfájl új jelölési szövege
  */
function insertAttachmentHook(fileName, markdownText);
```

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertingFromMimeDataHook
-------------------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót akkor hívjuk meg, ha html vagy médiafájl be van illesztve egy jegyzetbe a „Ctrl + Shift + V” billentyűvel.
  *
  * @param szöveges szöveg a QMimeData objektumból
  * @param html html a QMimeData objektumból
  * @ visszaküldi azt a karakterláncot, amelyet be kell illeszteni a QMimeData objektum szövege helyett
  */
function insertingFromMimeDataHook(text, html);
```

Érdemes megnézni a példát [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) vagy [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-text-from-5pm-mail.qml).

handleNoteTextFileNameHook
--------------------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót akkor hívják meg, ha egy jegyzet lemezre kerül, ha
  * A "A jegyzetfájl neve eltérhet a címsortől" engedélyezve van
  * a beállításokban
  *
  * Lehetővé teszi a jegyzetfájl nevének módosítását
  * Ne feledje, hogy önnek is törődnie kell az ismétlődő nevekkel!
 *
  * Adjon meg egy üres karakterláncot, ha a megjegyzés fájlnevének meg kell felelnie
  * nem módosítható
  *
  * @param {NoteApi} megjegyzés - a tárolt jegyzet jegyzetobjektuma
  * @return {string} a jegyzet fájlneve
  */
function handleNoteTextFileNameHook(note);
```

Érdemes megnézni a példát [példa.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) vagy [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

handleNoteNameHook
------------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót akkor hívják meg, ha a jegyzethez meghatározzák a jegyzet nevét
  *
  * Lehetővé teszi a megtekintett jegyzet nevének módosítását
  *
  * Ha egy megjegyzés nevét nem szabad módosítani, küldjön vissza egy üres karakterláncot
  *
  * @param {NoteApi} megjegyzés - a tárolt jegyzet jegyzetobjektuma
  * @return {string} a jegyzet neve
  */
function handleNoteNameHook(note);
```

Érdemes megnézni a példát [példa.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

Lehet, hogy nem jó ötlet ezt a kampót használni, ha a fájlnév jegyzetnévként történő használata aktív.

handleNewNoteHeadlineHook
-------------------------

### Módszerhívás és paraméterek
```js
/**
 * Ez a függvény a jegyzet létrehozása előtt meghívásra kerül
 *
 * Lehetővé teszi a jegyzet címsorának módosítását a létrehozás előtt
 * Vegye figyelembe, hogy különben ügyelnie kell az egyedi jegyzetnévre
 * az új jegyzet nem jön létre, csak megjelenik a jegyzetlistában
 *
 * Ezt a funkciót jegyzetsablonok létrehozására használhatja
 *
 * @param headline text that would be used to create the headline
 * @return {string} the headline of the note
 */
function handleNewNoteHeadlineHook(headline);
```

Érdemes megnézni a példát [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml).

preNoteToMarkdownHtmlHook
-------------------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a függvényt a rendszer a jegyzet markdown html-jének generálása előtt hívja meg
  *
  * Lehetővé teszi, hogy módosítsa, mi kerül át a html konverterhez
  *
  * A függvény például több szkriptben is használható kód megjelenítésére (például LaTeX matematikai vagy sellő)
  * a grafikus megjelenítéséhez az előnézethez
  *
  * A jegyzet ebben a folyamatban nem módosul
  *
  * @param {NoteApi} note – a jegyzetobjektum
  * @param {string} leértékelés – az a leértékelés, amelyet hamarosan html-vé alakítanak
  * @param {bool} forExport - igaz, ha a html-t az exportáláshoz használja, hamis az előnézethez
  * @return {string} a módosított jelölést vagy üres karakterláncot, ha semmit sem kell módosítani
  */
függvény preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Érdemes megnézni a példát [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

noteToMarkdownHtmlHook
----------------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a függvényt akkor hívják meg, amikor egy jegyzet leíró html-jét generálják
  *
  * Lehetővé teszi a html módosítását
  * Ezt például korábban a jegyzet előnézete hívja meg
  *
  * A funkció több szkriptben is használható az előnézet html-jének módosítására
  *
  * @param {NoteApi} note – a jegyzetobjektum
  * @param {karakterlánc} html - a html, amely hamarosan megjelenik
  * @param {bool} forExport - igaz, ha a html-t az exportáláshoz használja, hamis az előnézethez
  * @return {string} a módosított html-t vagy egy üres karakterláncot, ha semmit sem kell módosítani
  */
függvény noteToMarkdownHtmlHook(note, html, forExport);
```

Érdemes megnézni a példát [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) vagy [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

Kérjük, olvassa el a [Támogatott HTML részhalmaz](http://doc.qt.io/qt-5/richtext-html-subset.html) dokumentációt az összes támogatott css stílus felsorolásához.

encryptionHook
--------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót akkor hívjuk meg, ha a szöveget titkosítani vagy visszafejteni kell
  *
  * @param text karaktersorozat a titkosításhoz vagy visszafejtéshez
  * @param jelszó írja be a jelszót
  * @param dekódolja a boolot, ha hamis titkosítást igényel, ha valódi dekódolást követel meg
  * @ visszaküldi a titkosított visszafejtett szöveget
  */
függvény encryptionHook (szöveg, jelszó, visszafejtés);
```

Érdemes megnézni a példákat [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml) vagy [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml).

noteTaggingHook
---------------

Megvalósíthatja saját jegyzetcímkézési mechanizmusát, például a jegyzet speciális szövegével, például `@ tag1`, `@ tag2`, `@ tag3`.

### Módszerhívás és paraméterek
```js
/**
  * Kezeli a jegyzet címkézését
  *
  * Ezt a funkciót akkor hívják meg, amikor címkéket adnak hozzá, eltávolítanak belőlük vagy átnevezik
  * egy jegyzetet vagy egy jegyzet címkéit fel kell tüntetni
  *
  * @param jegyzet
  * A @param művelet lehet "hozzáadás", "eltávolítás", "átnevezés" vagy "lista"
  * @param tagName címke neve hozzáadni, eltávolítani vagy átnevezni
  * @param newTagName címke neve átnevezésre, ha action = "átnevezés"
  * @return note szöveges karaktersorozat vagy a címkék nevének string-listája (ha action = "list")
  */
```

-   amint egy szkript aktiválódik, amely végrehajtja az új függvényt, a `noteTaggingHook` jegyzet címkézést ez a függvény kezeli.
-   a következő funkcióknak a QOwnNotes felhasználói felületén keresztül kell működniük
    -   kezdetben olyan címkéket importál a jegyzeteiből, mint a `@tag`, és felülírja az aktuális címkekiosztást
        -   nem veszíti el a címkék fáját, csak a jegyzetekhez való korábbi hozzárendelést
        -   továbbra is áthelyezhet címkéket más címkékbe
        -   ha egynél több címkének ugyanaz a neve a címkefájában, akkor az első találatot hozzárendelik
    -   címke hozzáadása egy jegyzethez hozzáadja a címkét a jegyzet szövegéhez
    -   címke eltávolítása egy jegyzetből eltávolítja a címkét a jegyzet szövegéből
    -   a címkék eltávolításával a címkékből eltávolítja ezeket a címkéket a jegyzeteiből
    -   a címkék listáján szereplő címkék átnevezése átnevezi a jegyzetekben szereplő címkéket
    -   A jegyzetek listájában a jegyzetek tömeges címkézése hozzáadja ezeket a címkéket a jegyzeteihez
    -   A címkék tömeges eltávolítása a jegyzetlista jegyzeteiből eltávolítja ezeket a címkéket a jegyzeteiből
    -   az alkalmazás elindít egy sor `add` és `remove` műveletet az összes kijelölt címkéhez és gyermekéhez az összes jegyzethez, ha a címkéket áthelyezi a címke panelre

Érdemes megnézni a példát [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml) saját címkézési mechanizmusának megvalósításához.

::: warning
Győződjön meg arról, hogy a ` list ` művelete valóban gyors, mert minden jegyzetnél végrehajtásra kerül, minden alkalommal, amikor a jegyzetmappa újratöltésre kerül!
:::

noteTaggingByObjectHook
----------------------

A [noteTaggingHook](#notetagginghook) hez hasonlóan megvalósíthatja saját jegyzetcímkézési mechanizmusát, de a címkefa gyökerében nem kötelező a címkék neve. Így a teljes címkefát használhatja fel, nem csak címkézési lista helyett.

A `noteTaggingByObjectHook` paranccsal egy `TagApi` objektumot kapunk paraméterként címke neve helyett. A `list` művelet eredményeként meg kell adnia a címkeazonosítók listáját.

Ez azt is jelenti, hogy magának kell létrehoznia hiányzó címkéket, hogy meg tudja adni a már meglévő címkeazonosítók listáját a `list` művelethez.

### Módszerhívás és paraméterek
```js
/**
  * Kezeli a jegyzet címkézését
  *
  * Ezt a funkciót akkor hívják meg, amikor címkéket adnak hozzá, eltávolítanak belőlük vagy átnevezik
  * egy jegyzetet vagy egy jegyzet címkéit fel kell tüntetni
  *
  * @param jegyzet
  * A @param művelet lehet "hozzáadás", "eltávolítás", "átnevezés" vagy "lista"
  * Hozzáadni, eltávolítani vagy átnevezni a @param címkét
  * @param newTagName címke neve átnevezésre, ha action = "átnevezés"
  * @return note szöveges karakterlánc vagy a tag-azonosítók string-listája (ha action = "list")
  */
function noteTaggingByObjectHook(note, action, tag, newTagName);
```

Érdemes megnézni a példát [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml) saját címkézési mechanizmusának megvalósításához.

autocompletionHook
------------------

Visszaadhatja az automatikus kiegészítés listájához hozzáadandó karaktersorozatok listáját, amikor meghívják az automatikus kiegészítést (például a <kbd>Ctrl + szóköz</kbd> megnyomásával).

### Módszerhívás és paraméterek
```js
/**
  * Meghívja az autocompletionHook függvényt az összes szkriptösszetevőhöz
  * Ezt a funkciót akkor hívják meg, amikor az automatikus kiegészítést meghívják egy jegyzetben
  *
  * @return QStringList szöveg az automatikus kiegészítés listájához
  */
függvény callAutocompletionHook ();
```

Érdemes megnéznie az [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml) példát.

websocketRawDataHook
--------------------

Ezt a horgot akkor hívják meg, amikor az adatokat a QOwnNotes Web Companion böngészőbővítményből küldi a webböngésző helyi menüjén keresztül.

### Módszerhívás és paraméterek
```js
/**
  * @param requestType lehet "oldal" vagy "kiválasztás"
  * @param pageUrl annak a weboldalnak az URL-jét, ahová a kérelmet benyújtották
  * @param pageCímezze meg annak a weboldalnak az oldal címét, ahol a kérést benyújtották
  * @param rawData a továbbított adatok, html a requestType "page" vagy sima szöveg esetén a requestType "selection"
  * @param screenshotDataUrl a képernyőkép adat URL-jét, ha az a weboldal, ahol a kérelmet benyújtották
  * @return true, ha az adatokat horog kezelte
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

Érdemes megnézni a példákat [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-new-note.qml) és [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

onDetachedProcessCallback
-------------------------

Ezt a horgot akkor hívják meg, amikor a [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) parancsfájl szála végrehajtásra kerül.

### Módszerhívás és paraméterek
```js
/**
 * Ezt a függvényt akkor hívjuk meg, amikor a parancsfájl szála végrehajtásra kerül.
 * Hint: thread[1]==0 segít meghatározni, hogy az indított folyamatok nagy része elkészült-e egy adott azonosítóhoz.
 *
  * @param {QString} callbackIdentifier - a megadott azonosító a startDetachedProcess () meghívásakor
  * @param {QString} resultSet - a folyamat eredménye
  * @param {QVariantList} cmd - a teljes parancstömb [0-végrehajthatóPath, 1-paraméterek, 2-exitCode]
  * @param {QVariantList} szál - a szál információs tömbje [0-átadott visszahívási paraméter, 1 hátralévő szál ehhez az azonosítóhoz]
  */
függvény onDetachedProcessCallback (callbackIdentifier, resultSet, cmd, thread);
```

Érdemes megnézni a példát [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml).

windowStateChangedHook
--------------

### Módszerhívás és paraméterek
```js
/**
  * Ezt a funkciót egy WindowStateChange esemény elindítása után hívják meg
  *
  * @param {QString} windowState - az új ablakállapot, a paraméter értéke "minimalizálható", "maximalizálható", "teljes képernyős", "aktív" vagy "nosztatikus"
  */
függvény windowStateChangedHook (windowState);
```

Érdemes megnézni a példát [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/window-state-changed.qml).

workspaceSwitchedHook
----------------------

This hook is called when workspaces are switched.

### Módszerhívás és paraméterek
```js
/**
 * This function is called when workspaces are switched
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

Érdemes lehet egy pillantást vetni a példára [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/workspaces.qml).
