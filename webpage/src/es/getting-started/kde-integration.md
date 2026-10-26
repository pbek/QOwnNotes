# KDE integration

QOwnNotes can be built with optional [KDE Frameworks](https://develop.kde.org/products/frameworks/)
integration. The KDE build is the same application with the same settings, notes and
interface, but it integrates more closely with the KDE Plasma desktop.

::: tip
The KDE build is currently only available for Linux and Qt6. All features check whether the
needed desktop services are available, so the KDE build also works on other desktops.
:::

## Features

### Plasma global shortcuts

All actions from the _Shortcuts settings_ are registered in Plasma's shortcut settings
(_System Settings → Keyboard → Shortcuts → QOwnNotes_). You can assign shortcuts in
either place, and changes are synchronized in both directions. Actions are shown with
their menu path (e.g. `Note → New note`).

If the Plasma shortcut service isn't running, the usual global shortcut implementation is
used instead.

### Desktop notifications

Notifications are sent as native KDE notifications, which you can configure in
_System Settings → Notifications → Applications → QOwnNotes_.

- Scripts can show notifications with `script.showNotification(text, title)`.
- You get a notification if the current note was changed outside QOwnNotes while
  QOwnNotes is in the background.

Clicking a notification brings QOwnNotes to the front.

### Crash reporting

If QOwnNotes crashes, the KDE crash handler (DrKonqi) can help you to create a useful
bug report, if it is installed. Reports are never submitted automatically.

### Plasma activities

The note folder and workspace links to Plasma activities use the PlasmaActivities library,
which reacts faster to activity changes.

### Color schemes

In the _Interface settings_ you can select one of the installed KDE color schemes for the
QOwnNotes interface, independently of the color scheme of your desktop. The default,
_System default_, keeps using the colors of your desktop.

::: tip
The colors of the note editor still come from the editor color schema, so you may want to
choose a matching one. The dark mode theme takes precedence over the selected color scheme.
:::

### Window activation

On Wayland, QOwnNotes is brought to the front when you start it again while it is already
running, use a global shortcut of the desktop portal or click on a notification. On X11
the KDE build also passes the startup information to the window manager, so the launch
feedback ends properly.

### Sending notes

The _Note_ menu has a _Send note to_ submenu, and the context menu of the note list has a
_Send notes to_ submenu. They let you send note files with [KDE Connect](https://kdeconnect.kde.org/)
to your phone, by email or Bluetooth, or to other available targets.

Notes are sent as they are stored on disk, so encrypted notes stay encrypted.

### KRunner search

You can search your notes in KRunner and Plasma's application launcher. Type at least
three characters to see matching notes from the current note folder of every running
QOwnNotes instance. Selecting a result opens the note in QOwnNotes.

QOwnNotes needs to be running to show results. You can turn the search off in
_System Settings → Search → Plasma Search_.

::: tip
The KRunner search is part of all Linux builds of QOwnNotes, but it is only active if the
KRunner plugin file `org.qownnotes.QOwnNotes.desktop` is installed in
`share/krunner/dbusplugins`, which the KDE build does automatically.
:::

## Building the KDE version

To build the KDE version, you need Qt6 and the KDE Frameworks 6 development packages
(Extra CMake Modules, KColorScheme, KCoreAddons, KCrash, KGlobalAccel, KNotifications,
KWindowSystem and Purpose), as well as PlasmaActivities. Turn on the integration with the
CMake option `QON_KDE_INTEGRATION`:

```bash
cmake -S src -B build-QOwnNotes-kde \
  -DQON_QT6_BUILD=ON -DQON_KDE_INTEGRATION=ON \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-QOwnNotes-kde --parallel
```

With [Nix](../installation/nix.md) you can also build the `qownnotes-qt6-kde` package of the
QOwnNotes flake: `nix build '.?submodules=1#qownnotes-qt6-kde'`.
For more information take a look at [Building QOwnNotes](../installation/building.md).
