---
image: /img/bookmarks.png
---

# Estensione del browser QOwnNotes Web Companion

Consente di ritagliare da una pagina del browser e gestire i segnalibri del browser su diversi browser e sistemi operativi.

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
2. Aggiungi il token di sicurezza per configurare l'estensione.
    - La prima volta che clicchi sull'icona dell'estensione del browser di QOwnNotes, vedrai una finestra di dialogo col token di sicurezza. Copialo.
    - Vai alla gestione delle tue estensioni del browser. Clicca sui dettagli dell'estensione QOwnNotes.
    - Incolla il token nel campo del token di sicurezza.

## Strumento di ritaglio web

![strumento di ritaglio web](/img/web-clipper.png)

Fai clic con il pulsante destro del mouse su una pagina web o sul testo selezionato per usare la funzionalità **web-clipper**. Lì puoi anche creare una nuova nota con uno **screenshot** della pagina web corrente.

::: warning
Info If the web clipper doesn't work, please check if the QOwnNotes desktop application is running, and the Web Companion browser extension is configured with the security token.

Also, make sure you accepted the consent dialog, when you first clicked the extension icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional permission `Access your data for all websites` in the *Permission* extension settings, otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
Il web clipper è anche scriptabile! Dai un'occhiata a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) se desideri controllare ciò che ritagli dalle pagine web.
:::

## Segnalibri

![segnalibri](/img/bookmarks.png)

Se vuoi il pieno controllo dei tuoi segnalibri del browser e **usarli tra diversi browser e sistemi operativi**, allora l'estensione del browser di QOwnNotes fa per te.

By default, the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. Questi collegamenti avranno l'etichetta `corrente`.

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
