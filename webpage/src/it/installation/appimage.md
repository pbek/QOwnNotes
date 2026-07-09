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

## Portable mode

You can run the AppImage in **portable mode** by passing the `--portable` argument:

```bash
./QOwnNotes-x86_64.AppImage --portable
```

In portable mode all application data is stored in a `Data` folder next to the AppImage file instead of in your home directory. This includes:

- Settings (`Data/QOwnNotes.ini`)
- The notes database (`Data/QOwnNotes.sqlite`)
- Your notes (`Data/Notes/`)
- Scripts, spell-check dictionaries, and other application data

This makes it easy to carry QOwnNotes together with all your notes on a USB drive or any other portable storage device.

::: tip
Make sure the directory containing the AppImage file is writable, so that the `Data` folder can be created next to it.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on your Linux distribution and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
