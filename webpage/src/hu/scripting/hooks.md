# Hooks

## onNoteStored

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

You may want to take a look at the example [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteOpenedHook

### Módszerhívás és paraméterek

```js
/**
  * Ezt a funkciót egy jegyzet megnyitása után hívják meg
  *
  * @param {NoteApi} megjegyzés - a megnyitott jegyzetobjektum
  */
function noteOpenedHook(note);
```

You may want to take a look at the example [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteDoubleClickedHook

### Módszerhívás és paraméterek

```js
/**
  * Ezt a funkciót akkor hívják meg, ha egy jegyzetre duplán kattintottak
  *
  * @param {NoteApi} jegyzet - a jegyzetre kattintott objektum
  */
function noteDoubleClickedHook (jegyzet);
```

You may want to take a look at the example [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-open.qml).

## insertMediaHook

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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertAttachmentHook

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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertingFromMimeDataHook

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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) or [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-text-from-5pm-mail.qml).

## handleNoteTextFileNameHook

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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) or [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## handleNoteNameHook

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

The note name is mainly used when showing the note in the lists.

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

Lehet, hogy nem jó ötlet ezt a kampót használni, ha a fájlnév jegyzetnévként történő használata aktív.

## handleNewNoteHeadlineHook

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

You may want to take a look at the example [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-new-note-headline.qml).

## preNoteToMarkdownHtmlHook

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

You may want to take a look at the example [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

## noteToMarkdownHtmlHook

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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) or [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

Kérjük, olvassa el a [Támogatott HTML részhalmaz](http://doc.qt.io/qt-5/richtext-html-subset.html) dokumentációt az összes támogatott css stílus felsorolásához.

## encryptionHook

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

You may want to take a look at the example [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml) or [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-rot13.qml).

## noteTaggingHook

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

- as soon as a script is activated that implements the new function `noteTaggingHook` note tagging will be handled by that function
- a következő funkcióknak a QOwnNotes felhasználói felületén keresztül kell működniük
  - initially importing tags like `@tag` from your notes and overwriting your current tag assignment
    - you will not lose your tags tree, just the former assignment to notes
    - továbbra is áthelyezhet címkéket más címkékbe
    - if more than one tag has the same name in your tag tree the first hit will be assigned
  - címke hozzáadása egy jegyzethez hozzáadja a címkét a jegyzet szövegéhez
  - címke eltávolítása egy jegyzetből eltávolítja a címkét a jegyzet szövegéből
  - removing of tags in the tag list will remove those tags from your notes
  - renaming of tags in the tag list will rename those tags in your notes
  - bulk tagging of notes in the note list will add those tags to your notes
  - bulk removing of tags from notes in the note list will remove those tags from your notes
  - the application will trigger a series of `add` and `remove` actions for all selected tags and their children on all notes if tags are moved in the tag panel

You may want to take a look at the example [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging.qml) to implement your own tagging mechanism.

::: warning
Make sure your `list` action is really fast, because it will be executed for every note every time the note folder or the tag tree is reloaded!

That is every time you find `reloadTagTree` in [mainwindow.cpp](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.cpp).
:::

## noteTaggingByObjectHook

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

You may want to take a look at the example [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) to implement your own tagging mechanism.

## autocompletionHook

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

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## websocketRawDataHook

Ezt a horgot akkor hívják meg, amikor az adatokat a QOwnNotes Web Companion böngészőbővítményből küldi a webböngésző helyi menüjén keresztül.

### Módszerhívás és paraméterek

```js
/**
 * @param requestType can be "page" or "selection"
 * @param pageUrl the url of the webpage where the request was made
 * @param pageTitle the page title of the webpage where the request was made
 * @param rawData the data that was transmitted, html for requestType "page" or plain text for requestType "selection"
 * @param screenshotDataUrl the data url of the screenshot of the webpage where the request was made
 * @return true if data was handled by a hook
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

You may want to take a look at the examples [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-new-note.qml) and [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

## onDetachedProcessCallback

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

You may want to take a look at the example [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml).

## windowStateChangedHook

### Módszerhívás és paraméterek

```js
/**
  * Ezt a funkciót egy WindowStateChange esemény elindítása után hívják meg
  *
  * @param {QString} windowState - az új ablakállapot, a paraméter értéke "minimalizálható", "maximalizálható", "teljes képernyős", "aktív" vagy "nosztatikus"
  */
függvény windowStateChangedHook (windowState);
```

You may want to take a look at the example [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/window-state-changed.qml).

## workspaceSwitchedHook

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

You may want to take a look at the example [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/workspaces.qml).

## openAiBackendsHook

This hook is called, when the OpenAI service config is reloaded. For example, this is also done when the script engine is reloaded.

You can use it to provide config for custom OpenAI backends, like your own OpenAI API compatible LLMs, for example ChatGPT, Claude or systems like Ollama.

### Method call and parameters

```js
/**
 * This function is called when the OpenAI service config is reloaded
 * It returns a list of objects with config parameters for new OpenAI backends
 */
function openAiBackendsHook() {
  return [
    {
      id: "my-custom-ai",
      name: "My Custom AI",
      baseUrl: "http://localhost:5000",
      apiKey: "kDFJkjk3asdm",
      models: ["gpt-3.5-turbo", "gpt-4.0-turbo"],
    },
    {
      id: "my-custom-ai2",
      name: "My Custom AI 2",
      baseUrl: "http://localhost:5001",
      apiKey: "lOikf7eNdb9",
      models: ["gpt-3.5-turbo2", "gpt-4.0-turbo2"],
    },
  ];
}
```

::: tip
You can use [Registering script settings variables](./methods-and-objects.md#registering-script-settings-variables) to store the OpenAI backend settings in the script settings.
:::

You may want to take a look at the example [custom-openai-backends.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-openai-backends.qml).
