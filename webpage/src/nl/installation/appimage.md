# Installeer als AppImage

U kunt de nieuwste AppImage downloaden van de [QOwnNotes-releasepagina](https://github.com/pbek/QOwnNotes/releases). Het zou op die pagina de naam `QOwnNotes-x86_64.AppImage` moeten hebben.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed, you can also download the latest AppImage directly:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

Vervolgens kunt u de uitvoeringsrechten voor het bestand wijzigen:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
AppImages hebben `libfuse2` nodig! Onder Ubuntu kun je bijvoorbeeld de bibliotheek installeren met `sudo apt install libfuse2`.
:::

Daarna zou u de AppImage moeten kunnen uitvoeren om QOwnNotes uit te voeren.

::: warning
Als u de **automatische updater** wilt gebruiken, zorg er dan voor dat u uw AppImage op een plaats plaatst waar uw gebruikersaccount schrijftoegang heeft, bijvoorbeeld ergens in uw homedirectory.
:::

::: tip
Als u problemen heeft met het uitvoeren van de AppImage, omdat uw glibc-versie te oud is, kunt u de [AppImage gebouwd op OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage) proberen, deze moet worden gebouwd met glibc 2.16.
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
