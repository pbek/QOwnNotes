# Installa come AppImage

Puoi scaricare l'ultima AppImage dalla pagina [Releases](https://github.com/pbek/QOwnNotes/releases) di QOwnNotes. Nella pagina, si dovrebbe chiamare `QOwnNotes-x86_64.AppImage`.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed, you can also download the latest AppImage directly:

```bash
# interroga l'ultima release Linux tramite le API QOwnNotes, analizza il JSON per cercare l'URL e lo scarica
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

Quindi puoi modificare i permessi di esecuzione del file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
AppImages need `libfuse2` to run! For example under Ubuntu you can install the library with `sudo apt install libfuse2`.
:::

Dopo di che dovresti essere in grado di eseguire il file AppImage per aprire QOwnNotes.

::: warning
Se vuoi usare l'**aggiornamento automatico** assicurati di mettere il tuo AppImage in un punto in cui il tuo account utente ha accesso in scrittura, come per esempio da qualche parte nella tua cartella home.
:::

::: tip
Se hai problemi e eseguire AppImage, perché la versione della tua glibc è troppo vecchia, puoi provare [AppImage costruita su OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage), dovrebbe essere costruita con glibc 2.16.
:::
