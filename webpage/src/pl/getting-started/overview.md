# Przegląd

<ScreenshotCarousel />

<v-divider />

**Q** dla [frameworka Qt](https://www.qt.io/) używanego  
**Own**, ponieważ jesteś właścicielem własnych notatek  
**Notes** dotyczące notatek, które robisz

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) to **open source** (GPL) zwykły plik tekstowy **notatka notowań** aplikacja dla **GNU / Linux**, **macO ** i **Windows**, który (opcjonalnie) współpracuje z [**aplikacją do notatek**](https://github.com/nextcloud/notes) z [Nextcloud](https://nextcloud.com/) i [ownCloud](https://owncloud.org/).

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Zrozum pojęcie QOwnNotes](concept.md)

## Funkcje

- You can use your **existing text or markdown files**, no need for an import
- QOwnNotes jest napisany w C++ i zoptymalizowany pod kątem **niskiego zużycia zasobów** (bez aplikacji Electron, która wymaga dużej ilości procesora i pamięci)
- **Import z Evernote i Joplin**
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

## Wygląd zewnętrzny

- **Podświetlanie przecen** notatek i **tryb podglądu przecen**
- **Obsługa motywów trybu ciemnego**
- **Theming support** for the **Markdown syntax highlighting**
- Wszystkie **panele można umieścić w dowolnym miejscu**, mogą nawet **unosić się** lub **ustawiać w stos** (w pełni dokowalne)
- Obsługa **ikon motywów freedesktop**, możesz używać QOwnNotes ze swoimi **natywnymi ikonami pulpitu** oraz ze swoim ulubionym **ciemnym motywem pulpitu**
- **Tryb bez rozpraszania uwagi**, **Tryb pełnoekranowy** i **Tryb maszyny do pisania**
- **Numery wierszy**
- **Differences** between current note and externally changed note are **shown** in a dialog

## Języki

- **Dostępne w ponad 60 różnych językach**, takich jak angielski, niemiecki, francuski, polski, chiński, japoński, rosyjski, portugalski, węgierski, holenderski i hiszpański
  - [Twoja pomoc](../contributing/translation.md) jest bardzo ceniona w ulepszaniu tych tłumaczeń lub w tłumaczeniu QOwnNotes na więcej języków

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
