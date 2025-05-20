# Install as AppImage

You can download the latest AppImage from the [QOwnNotes releases page](https://github.com/pbek/QOwnNotes/releases).
It should be named like `QOwnNotes-x86_64.AppImage` on that page.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed, you can also
download the latest AppImage directly:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

Then you can change the execute-permissions on the file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
AppImages need `libfuse2` to run!
For example under Ubuntu you can install the library with `sudo apt install libfuse2`.
:::

Afterward you should be able to execute the AppImage to run QOwnNotes.

::: warning
If you want to use the **automatic updater** please make sure to put your
AppImage in a place where your user account has write-access to, like somewhere
in your home directory.
:::

::: tip
If you have troubles running the AppImage, because your glibc version is too old
you can try the [AppImage built on OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage),
it should be built with glibc 2.16.
:::
