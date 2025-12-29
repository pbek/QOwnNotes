# Áttekintés

<ScreenshotCarousel />

<v-divider />

**Q** a [Qt framework](https://www.qt.io/), amelyet használ  
**Own** mert birtokolja a jegyzeteit  
**Notes** az Ön jegyzetei végett, melyeket elkészít

<v-divider />

A [QOwnNotes](https://www.qownnotes.org/) a **nyílt forráskódú** (GPL) sima szövegű fájl **macOS** és **Windows**, amelyek (opcionálisan) együtt működnek a [Nextcloud](https://nextcloud.com/) és az [ownCloud](https://owncloud.org/) [**Notes alkalmazással**](https://github.com/nextcloud/notes).

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Értse meg a QOwnNotes fogalmát](concept.md)

## Funkciók

- You can use your **existing text or markdown files**, no need for an import
- A QOwnNotes C++ nyelven íródott, és **alacsony erőforrás-fogyasztásra** van optimalizálva (nincs CPU és memóriaéhes Electron alkalmazás)
- **Evernote és Joplin import**
- **Compatible** with the [Notes](https://apps.nextcloud.com/apps/notes) application of **Nextcloud / ownCloud** and **mobile** and Nextcloud Text
- Create **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) cards** and link to them in your notes
- Support for **sharing notes** on your **ownCloud** / **Nextcloud** server
- Manage your Nextcloud / ownCloud **todo lists** (**Nextcloud tasks** or **Tasks Plus** / **Calendar Plus**) or use another **CalDAV server** to sync your tasks to
- Older **versions** of your notes can be **restored** from your **Nextcloud / ownCloud** server
- **Trashed notes** can be **restored** from your **Nextcloud / ownCloud** server
- **External changes** of note files are **watched** (notes or note list are reloaded)
- Support for **hierarchical note tagging** and **note subfolders**
- Optional **encryption** of notes, AES-256 is built in, or you can use custom encryption methods like **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) or **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- The **notes folder** can be **freely chosen** (multiple note folders can be used)
- **Searching parts of words** in notes is possible and search results are highlighted in the notes
- [**Browser extension**](browser-extension.md) to **add notes from the selected text**, **make screenshots** or **manage your bookmarks**
- [**Spellchecking**](../editor/spellchecking.md) support
- **Portable mode** for carrying QOwnNotes around on USB sticks
- **Scripting support** and an online [**script repository**](https://github.com/qownnotes/scripts) where you can install scripts inside the application
- Implementation of the **[OpenAI completion API](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** to be used in scripts
- **Vim mode**
- **[Web application](web-app.md)** to **insert photos from your mobile phone** into the current note in QOwnNotes on your desktop

## Megjelenés

- A jegyzetek **jelölésének kiemelése** és egy **jelölés előnézeti mód**
- **Sötét módú téma támogatás**
- **Theming support** for the **Markdown syntax highlighting**
- Minden **panel tetszőlegesen elhelyezhető**, akár **lebeghet** vagy **teljesen dokkolható**
- Támogatja a **freedesktop téma ikonokat**, használhatja a QOwnNotes programot a **natív asztali ikonokkal** és a kedvenc **sötét asztali témájával**
- **Figyelemelterelés mentes mód**, **teljes képernyős mód** és **Írógép mód**
- **Sorszám**
- **Differences** between current note and externally changed note are **shown** in a dialog

## Nyelvek

- **Több mint 60 különböző nyelven érhető el**, mint például angol, német, francia, lengyel, kínai, japán, orosz, portugál, magyar, holland és spanyol
  - [Segítségét](../contributing/translation.md) nagyra értékelik ezen fordítások fejlesztése vagy a QOwnNotes több nyelvre történő lefordítása érdekében

<style>
.v-divider {
  margin: 20px;
}

.sheet {
  position: absolute;
  bottom: 50px;
  background-color: rgba(0,0,0, 0.5);
  color: white;
  text-align: center;
  display: flex;
  align-items:center;
  justify-content:center;
  height: 50px;
  width: 100%;
}

.v-window__next {
  right: 0;
}

@media (max-width: 500px) {
  .v-carousel {
    height: 400px!important;
  }
}

@media (max-width: 350px) {
  .v-carousel {
    height: 250px!important;
  }
}

@media (max-width: 200px) {
  .v-carousel {
    height: 150px!important;
  }
}
</style>
