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
3. To use the private mode, you need to allow the extension to run in private windows in the extension settings.

## Schowek sieciowy

![schowek sieciowy](/img/web-clipper.png)

Aby użyć funkcji **wycinania fragmentów stron (web-clipper)**, kliknij prawym przyciskiem myszy stronę sieci Web lub zaznaczony tekst. W tym miejscu możesz też utworzyć nową notatkę ze **zrzutem ekranu** bieżącej strony sieci Web.

::: warning
Info If the web clipper doesn't work, please check if the QOwnNotes desktop application is running, and the Web Companion browser extension is configured with the security token.

Also, make sure you accept the consent dialog when you first click the extension icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional permission `Access your data for all websites` in the _Permission_ extension settings, otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
Nożyczki sieciowe są również skryptowalne! Spójrz na [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook), jeśli chcesz kontrolować, co wycinasz ze stron internetowych.
:::

## Zakładki

![zakładki](/img/bookmarks.png)

Jeśli chcesz mieć pełną kontrolę nad zakładkami przeglądarki i **używać ich w różnych przeglądarkach i systemach operacyjnych**, rozszerzenie przeglądarki QOwnNotes jest dla Ciebie.

By default, the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. Te linki zostaną otagowane jako `current` (bieżące).

Możesz też **zarządzać zakładkami w notatkach** za pomocą tagu notatki `bookmarks` (zakładki – można go zmienić w ustawieniach). Linki te również mogą być opatrzone tagami i opisem, które zostaną wyświetlone w rozszerzeniu przeglądarki.

Nowe zakładki są przechowywane w notatce o nazwie `Zakładki` (można ją również zmienić w ustawieniach).

::: Wskazówka
Możesz też zaimportować zakładki ze swojej przeglądarki do QOwnNotes za pomocą rozszerzenia Web Companion.
:::

### Składnia linków zakładek

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags

* [Webpage name](https://www.example.com) the alternative list character also works
```

You are able to search for name, url tags or description in the browser extension. In the current note all other links inside of text are parsed, but without tags or description.
