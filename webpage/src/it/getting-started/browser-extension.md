---
image: /img/bookmarks.png
---

# Estensione del browser QOwnNotes Web Companion

![bookmarks](/img/bookmarks.png)

Visita il [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) o la [pagina dei componenti aggiuntivi di Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) per installare l '[** estensione del browser QOwnNotes Web Companion **](https://github.com/qownnotes/web-companion/).

Puoi trovare l'estensione anche su [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes deve essere in esecuzione affinché l'estensione del browser Web Companion funzioni. The browser extensions works **offline**, no internet connection needed.
:::

## Web clipper

Fai clic con il pulsante destro del mouse su una pagina web o sul testo selezionato per utilizzare la funzionalità **web-clipper**. Lì puoi anche creare una nuova nota con uno **screenshot** della pagina web corrente.

::: tip
Il web clipper è anche scriptabile! Dai un'occhiata a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) se desideri controllare ciò che ritagli dalle pagine web.
:::

## Segnalibri

If you want full control over your browser bookmarks and **use them across different browsers and operating systems** then the QOwnNotes browser extension is for you.

By default the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. These links will get a tag `current`.

You can also **manage your bookmarks in notes** with the note tag `bookmarks` (changeable in the settings). These links can also have tags and a description that will be shown in the browser extension.

New bookmarks are stored in a note called `Bookmarks` (also changeable in the settings).

::: tip
You can also import your browser bookmarks into QOwnNotes with the web companion browser extension!
:::

### Sintassi dei collegamenti segnalibri

```markdown
- [Nome pagina web](https://www.example.com)
- [Nome pagina web](https://www.example.com) #etichetta1 #etichetta2
- [Nome pagina web](https://www.example.com) solo descrizione
- [Nome pagina web](https://www.example.com) #etichetta1 #etichetta2 descrizione e etichette
* [Nome pagina web](https://www.example.com) funziona anche il carattere lista alternativo
```

You are able to search for name, url tags or description in the browser extension.
