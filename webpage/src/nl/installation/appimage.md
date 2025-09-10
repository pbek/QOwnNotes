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
