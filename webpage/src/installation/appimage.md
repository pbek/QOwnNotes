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

## Portable mode

You can run the AppImage in **portable mode** by passing the `--portable` argument:

```bash
./QOwnNotes-x86_64.AppImage --portable
```

In portable mode all application data is stored in a `Data` folder next to the
AppImage file instead of in your home directory. This includes:

- Settings (`Data/QOwnNotes.ini`)
- The notes database (`Data/QOwnNotes.sqlite`)
- Your notes (`Data/Notes/`)
- Scripts, spell-check dictionaries, and other application data

This makes it easy to carry QOwnNotes together with all your notes on a USB
drive or any other portable storage device.

::: tip
Make sure the directory containing the AppImage file is writable, so that the
`Data` folder can be created next to it.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on your Linux distribution and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
