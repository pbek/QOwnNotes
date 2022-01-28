# Installa come AppImage

Puede descargar la última AppImage de la [página de versiones de QOwnNotes](https://github.com/pbek/QOwnNotes/releases). Debe llamarse como `QOwnNotes-x86_64.AppImage` en esa página.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed you can also download the latest AppImage directly:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

Then you can change the execute-permissions on the file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Afterwards you should be able to execute the AppImage to run QOwnNotes.

::: warning
If you want to use the **automatic updater** please make sure to put your AppImage in a place where your user account has write-access to, like somewhere in your home directory.
:::

::: tip
If you have troubles running the AppImage, because your glibc version is too old you can try the [AppImage built on OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage), it should be built with glibc 2.16.
:::
