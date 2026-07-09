# Als AppImage installieren

Sie können das neueste AppImage von der Seite [QOwnNotes-Versionen](https://github.com/pbek/QOwnNotes/releases) herunterladen. Es sollte auf dieser Seite in etwa `QOwnNotes-x86_64.AppImage` heißen.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed, you can also download the latest AppImage directly:

```bash
# fragen Sie die neueste Linux-Version von der QOwnNotes-API ab, parsen Sie die JSON nach der URL und laden Sie sie runter
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

Danach können Sie die Ausführungsberechtigungen für die Datei ändern:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
AppImages need `libfuse2` to run! For example under Ubuntu you can install the library with `sudo apt install libfuse2`.
:::

Nachher sollten Sie das AppImage ausführen können, um QOwnNotes zu starten.

::: warning
Wenn Sie den **automatischen Updater** verwenden möchten, stellen Sie bitte sicher, dass Sie Ihr AppImage an einem Ort ablegen, auf den Ihr Benutzerkonto Schreibzugriff hat, z.B. irgendwo in Ihrem Home-Verzeichnis.
:::

::: tip
Falls Sie Problem beim Starten des AppImages haben, weil Ihre glibc-Version zu alt ist, können Sie das [AppImage basierend auf OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage) ausprobieren, das mit glibc 2.16 erstellt worden sein müsste.
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
