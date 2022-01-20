---
image: /img/bookmarks.png
---

# Estensione del browser QOwnNotes Web Companion

Allows for clipping from a browser page and managing browser bookmarks across browsers and operating systems.

::: tip
Info
- QOwnNotes deve essere in esecuzione affinché l'estensione del browser Web Companion funzioni.
- Non è necessaria una connessione Internet. L'estensione del browser lavora **offline**.
:::

## Installazione

1. Scarica l'estensione
    - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
    - [Pagina delle Estensioni Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
    - Puoi trovare l'estensione anche su [GitHub](https://github.com/qownnotes/web-companion/).
2. Add the Security Token to configure the extension.
    - The first time you click on the QOwnNotes browser extension icon you will receive a dialog box with a security token. Copy the token.
    - Go to your browser's extension management location. Click into the QOwnNotes extension details.
    - Paste the token into the Security Token field.

## Strumento di ritaglio web

![strumento di ritaglio web](/img/web-clipper.png)

Fai clic con il pulsante destro del mouse su una pagina web o sul testo selezionato per usare la funzionalità **web-clipper**. Lì puoi anche creare una nuova nota con uno **screenshot** della pagina web corrente.

::: tip
Il web clipper è anche scriptabile! Dai un'occhiata a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) se desideri controllare ciò che ritagli dalle pagine web.
:::

## Segnalibri

![segnalibri](/img/bookmarks.png)

Se vuoi il pieno controllo dei tuoi segnalibri del browser e **usarli tra diversi browser e sistemi operativi**, allora l'estensione del browser di QOwnNotes fa per te.

Per impostazione predefinita, l'estensione del browser mostrerà tutti i **collegamenti della nota corrente** in un popup quando fai clic sull'icona QOwnNotes nel tuo browser. Questi collegamenti avranno l'etichetta `corrente`.

Puoi anche **gestire i tuoi segnalibri nelle note** con l'etichetta `segnalibri` (modificabile nelle impostazioni). Questi collegamenti possono anche avere etichette e una descrizione che verrà mostrata nell'estensione del browser.

I nuovi segnalibri sono memorizzati in una nota chiamata `Segnalibri` (anch'essa modificabile nelle impostazioni).

::: tip
Puoi anche importare i segnalibri del tuo browser in QOwnNotes con l'estensione browser web companion!
:::

### Sintassi dei collegamenti segnalibri

```markdown
- [Nome pagina web](https://www.example.com)
- [Nome pagina web](https://www.example.com) #etichetta1 #etichetta2
- [Nome pagina web](https://www.example.com) solo descrizione
- [Nome pagina web](https://www.example.com) #etichetta1 #etichetta2 descrizione e etichette
* [Nome pagina web](https://www.example.com) funziona anche il carattere lista alternativo
```

Puoi cercare nome, etichetta URL o descrizione nell'estensione del browser.
