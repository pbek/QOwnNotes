# Installa come AppImage

Puoi scaricare l'ultima AppImage dalla pagina [Releases](https://github.com/pbek/QOwnNotes/releases) di QOwnNotes. Nella pagina, si dovrebbe chiamare `QOwnNotes-x86_64.AppImage`.

::: tip
Se hai installato [jq](https://stedolan.github.io/jq/) puoi anche scaricate direttamente l'ultima AppImage:

```bash
# interroga l'ultima release Linux tramite le API QOwnNotes, analizza il JSON per cercare l'URL e lo scarica
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

Quindi puoi modificare i permessi di esecuzione del file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Dopodiché, dovresti essere in grado di eseguire l'AppImage per lanciare QOwnNotes.

::: warning
Se vuoi usare l'**aggiornamento automatico** assicurati di mettere il tuo AppImage in un punto in cui il tuo account utente ha accesso in scrittura, come per esempio da qualche parte nella tua cartella home.
:::
