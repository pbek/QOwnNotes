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
Dołącz do naszej [grupy telegramów](https://t.me/QOwnNotes), aby dyskutować, uczyć się i łączyć ze społecznością QOwnNotes.
:::

[Zrozum pojęcie QOwnNotes](concept.md)

## Funkcje
- Możesz użyć **istniejących plików tekstowych lub znaczników**, przez większość czasu nie ma potrzeby importowania
- QOwnNotes jest napisany w C++ i zoptymalizowany pod kątem **niskiego zużycia zasobów** (bez aplikacji Electron, która wymaga dużej ilości procesora i pamięci)
- **Import z Evernote i Joplin**
- **Kompatybilny** z aplikacją Notes **Nextcloud / ownCloud** i **mobile** oraz Nextcloud Text
- Obsługa **udostępniania notatek** na serwerze **ownCloud** / **Nextcloud**
- Zarządzaj swoimi **listami rzeczy do zrobienia w Nextcloud / ownCloud** (**Zadania Nextcloud** lub **Tasks Plus** / **Calendar Plus**) lub użyj innego <0 >Serwer CalDAV</strong> do synchronizacji zadań z
- Starsze **wersje** Twoich notatek można **przywrócić** z serwera **Nextcloud / ownCloud**
- **Usunięte notatki** można **przywrócić** z serwera **Nextcloud / ownCloud**
- **Zewnętrzne zmiany** plików notatek są **obserwowane** (notatki lub lista notatek są ponownie ładowane)
- Obsługa **hierarchicznego tagowania notatek** i **podfolderów notatek**
- Opcjonalne **szyfrowanie** notatek, wbudowane jest AES-256 lub możesz użyć niestandardowych metod szyfrowania, takich jak **[Keybase.io](https://keybase.io/)** (
encryption-keybase.qml<//2>) lub **PGP** ([szyfrowanie-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml))</li> 
  
  - **folder notatek** można **swobodnie wybrać** (można użyć wielu folderów notatek)
- **Wyszukiwanie części słów** w notatkach jest możliwe, a wyniki wyszukiwania są podświetlone w notatkach
- [**Rozszerzenie przeglądarki**](browser-extension.md) do **dodawania notatek z zaznaczonego tekstu**, **tworzenia zrzutów ekranu** lub **zarządzania zakładkami<//1></li> 
  
  - Obsługa [**sprawdzania pisowni**](../editor/spellchecking.md)
- **Tryb przenośny** do przenoszenia QOwnNotes na pamięciach USB
- **Obsługa skryptów** i [**repozytorium skryptów online**](https://github.com/qownnotes/scripts), w którym można instalować skrypty wewnątrz aplikacji
- **Tryb Vima**
- [Aplikacja internetowa](web-app.md) do **wstawiania zdjęć z telefonu komórkowego** do bieżącej notatki w QOwnNotes na komputerze</ul> 




## Wygląd zewnętrzny

- **Podświetlanie przecen** notatek i **tryb podglądu przecen**
- **Obsługa motywów trybu ciemnego**
- **Obsługa motywów** dla **podświetlania składni przecen**
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
