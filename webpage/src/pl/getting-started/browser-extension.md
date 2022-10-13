---
image: /img/bookmarks.png
---

# Rozszerzenie przeglądarki QOwnNotes Web Companion

Umożliwia wycinanie ze strony przeglądarki i zarządzanie zakładkami przeglądarki w różnych przeglądarkach i systemach operacyjnych.

::: tip
Info
- Aby rozszerzenie przeglądarki Web Companion działało, aplikacja QOwnNotes musi być uruchomiona.
- Nie wymaga połączenia z Internetem. Rozszerzenia przeglądarki działają **w trybie offline**.
:::

## Instalacja

1. Pobierz rozszerzenie
    - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
    - [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
    - Rozszerzenie znajdziesz również w [GitHub](https://github.com/qownnotes/web-companion/).
2. Dodaj token zabezpieczający, aby skonfigurować rozszerzenie.
    - Po pierwszym kliknięciu ikony rozszerzenia przeglądarki QOwnNotes pojawi się okno dialogowe z tokenem bezpieczeństwa. Skopiuj token.
    - Przejdź do lokalizacji zarządzania rozszerzeniami przeglądarki. Kliknij w szczegóły rozszerzenia QOwnNotes.
    - Wklej token w polu Token bezpieczeństwa.

## Schowek sieciowy

![schowek sieciowy](/img/web-clipper.png)

Aby użyć funkcji **wycinania fragmentów stron (web-clipper)**, kliknij prawym przyciskiem myszy stronę sieci Web lub zaznaczony tekst. W tym miejscu możesz też utworzyć nową notatkę ze **zrzutem ekranu** bieżącej strony sieci Web.

::: tip
Nożyczki sieciowe są również skryptowalne! Spójrz na [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook), jeśli chcesz kontrolować, co wycinasz ze stron internetowych.
:::

## Zakładki

![zakładki](/img/bookmarks.png)

Jeśli chcesz mieć pełną kontrolę nad zakładkami przeglądarki i **używać ich w różnych przeglądarkach i systemach operacyjnych**, rozszerzenie przeglądarki QOwnNotes jest dla Ciebie.

Domyślnie po kliknięciu ikony QOwnNotes w przeglądarce rozszerzenie wyświetli wszystkie **linki dla bieżącej notatki** w wyskakującym okienku. Te linki zostaną otagowane jako `current` (bieżące).

Możesz też **zarządzać zakładkami w notatkach** za pomocą tagu notatki `bookmarks` (zakładki – można go zmienić w ustawieniach). Linki te również mogą być opatrzone tagami i opisem, które zostaną wyświetlone w rozszerzeniu przeglądarki.

Nowe zakładki są przechowywane w notatce o nazwie `Zakładki` (można ją również zmienić w ustawieniach).

::: Wskazówka
Możesz też zaimportować zakładki ze swojej przeglądarki do QOwnNotes za pomocą rozszerzenia Web Companion.
:::

### Składnia linków zakładek

```markdown
- [Nazwa strony](https://www.example.com)
- [Nazwa strony](https://www.example.com) #tag1 #tag2
- [Nazwa strony](https://www.example.com) tylko część opisu
- [Nazwa strony](https://www.example.com) #tag1 #tag2 część opisu i tagi
* [Nazwa strony](https://www.example.com) znak listy alternatywnej również działa
```

W rozszerzeniu przeglądarki możesz wyszukiwać nazwy, tagi URL i opisy.
