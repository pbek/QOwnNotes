# Przegląd

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="Zrzut ekranu QOwnNotes" />
    <div class="sheet">
      Edytuj swoje notatki za pomocą podświetlania znaczników, kolorowych znaczników i podfolderów
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Widok minimalny" />
    <div class="sheet">
      Minimalny, domyślny interfejs. który może być dodatkowo uproszczony
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Widok pionowy" />
    <div class="sheet">
      Przeglądaj swoje notatki w pionowym widoku przecen, przesuwając panele
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-note-relations.png" alt="Note relations" />
    <div class="sheet">
      Note relations panel
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Tryb przenośny" />
    <div class="sheet">
      Tryb przenośny dla pamięci USB
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="Jedna kolumna" />
    <div class="sheet">
      Wszystkie panele można umieścić w dowolnym miejscu
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="screenshot darkmode" />
    <div class="sheet">
      Tryb ciemny
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="screenshot-distraction-free-mode" />
    <div class="sheet">
      Tryb nie przeszkadzać
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Szyfrowanie notatek" />
    <div class="sheet">
      Opcjonalne szyfrowanie notatek AES (również skryptowe)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Zaszyfrowana notatka" />
    <div class="sheet">
      Zaszyfrowane notatki są nadal tekstem
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
    <div class="sheet">
      Pokaż różnicę między nutami, gdy została zmieniona zewnętrznie
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="screenshot-export-print" />
    <div class="sheet">
      Uwaga eksport i drukowanie PDF
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-freedesktop-theme.png" alt="screenshot-freedesktop-theme" />
    <div class="sheet">
      Ikony za pośrednictwem motywu Freedesktop
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-other-workspace.png" alt="screenshot-other-workspace" />
    <div class="sheet">
      Możesz mieć różne obszary robocze
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="screenshot-qml" />
    <div class="sheet">
      Skryptowalny
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="screenshot-russian" />
    <div class="sheet">
      Przetłumaczone na wiele języków
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="screenshot-search-in-all-notes" />
    <div class="sheet">
      Przeszukaj wszystkie notatki
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="screenshot-search-in-current-note" />
    <div class="sheet">
      Szukaj w bieżącej notatce
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="screenshot-settings-note-folders" />
    <div class="sheet">
      Możliwość korzystania z wielu folderów notatek
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
    <div class="sheet">
      Zarządzaj listami rzeczy do zrobienia za pomocą CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      Zarządzaj usuniętymi notatkami na serwerze Nextcloud
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      Zarządzaj wersjami notatek na serwerze Nextcloud
    </div>
  </v-carousel-item>
</v-carousel>
</template>

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
- **Różnice** między bieżącą notatką a zewnętrznie zmienioną notatką są **pokazane** w oknie dialogowym

## Języki

- **Dostępne w ponad 60 różnych językach**, takich jak angielski, niemiecki, francuski, polski, chiński, japoński, rosyjski, portugalski, węgierski, holenderski i hiszpański
  - [Twoja pomoc](../contributing/translation.md) jest bardzo ceniona w ulepszaniu tych tłumaczeń lub w tłumaczeniu QOwnNotes na więcej języków

<style>
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
