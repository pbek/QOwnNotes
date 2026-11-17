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

Afterward you should be able to execute the AppImage to run QOwnNotes.

Current QOwnNotes AppImages include the required FUSE userspace library and do
not require `libfuse2` to be installed separately. The system still needs kernel
FUSE support. Older QOwnNotes AppImages may still require `libfuse2`.

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

## Fcitx input on KDE Plasma Wayland

If Chinese or other input through Fcitx 5 does not work in the AppImage on
**KDE Plasma with native Wayland**, a global `QT_IM_MODULE=fcitx` setting may be
the cause. The AppImage bundles its own Qt but currently does not include the
Fcitx Qt input-method plugin. Explicitly requesting that plugin prevents Qt
from using native Wayland input.

Fully quit QOwnNotes first, including any instance in the system tray, then try:

```bash
env -u QT_IM_MODULE ./QOwnNotes-x86_64.AppImage
```

If you also have `QT_IM_MODULES` configured, unset both variables for this launch:

```bash
env -u QT_IM_MODULE -u QT_IM_MODULES ./QOwnNotes-x86_64.AppImage
```

Select **Fcitx 5** in Plasma's **System Settings → Virtual Keyboard** so that
Plasma starts it with the required Wayland connection. See
[Fcitx's Plasma/Wayland guidance](https://fcitx-im.org/wiki/Using_Fcitx_5_on_Wayland#KDE_Plasma).

These commands only change the environment for this launch. If this resolves
the problem, use the same environment override in an application-specific
launcher rather than changing your global input-method settings.

::: warning
This workaround is for native Wayland input on KDE Plasma, not X11/XWayland.
Do not unset input-method variables globally: other applications may need them.
If you need direct Fcitx Qt integration, consider the native QOwnNotes package
from your distribution instead.
:::

For the investigation and confirmed workaround, see
[issue #3849](https://github.com/pbek/QOwnNotes/issues/3849).

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
