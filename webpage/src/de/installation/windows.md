# Installieren unter Microsoft Windows™

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like. Es ist keine Installation erforderlich!

::: warning
Wenn Sie den **automatischen Updater** verwenden möchten, stellen Sie sicher, dass Sie ihn an einen Ort entpacken, auf den Ihr Benutzerkonto Schreibzugriff hat. By default, your user account most likely **doesn't have write access** to places like `C:\Program Files (x86)` or `C:\Program Files`.
:::

Sie können dann `QOwnNotes.exe` direkt in Ihrem `QOwnNotes`-Ordner ausführen. Eine Installation ist nicht erforderlich.

### Portabler Modus

Verwenden Sie `QOwnNotesPortable.bat`, um QOwnNotes im **portablen Modus** auszuführen, in dem alles (einschließlich Ihrer Notizen) nur in Ihrem `QOwnNotes`-Ordner gespeichert wird.

::: tip
Sie benötigen den portablen Modus nicht, wenn Sie lediglich keine Administratorrechte für Ihren Computer haben. QOwnNotes muss nicht installiert werden!
:::

## Windows 8.1 and older

Qt dropped support for Windows XP with version 5.8, but QOwnNotes is now also built with Qt 5.7 to allow Windows XP users to still use it. Windows 8.1 has issues with the regular QOwnNotes build, because it was built on a newer Windows SDK.

Sie müssen die ZIP-Datei von [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) selbst herunterladen und in einen Ordner Ihrer Wahl entpacken.

Sie können dann `QOwnNotes.exe` direkt aus diesem Ordner ausführen, es ist keine Installation erforderlich.

::: tip
Info
Der automatische Update-Mechanismus funktioniert nicht mit dem AppVeyor-Build für Windows XP!
Sie müssen neue Versionen selbst herunterladen.
:::

## Chocolatey

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

Sie können es installieren mit:

```shell
choco install qownnotes
```

## Scoop

There is a [community-maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket, you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

There is a [community-maintained package of QOwnNotes](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) for [WinGet](https://github.com/microsoft/winget-cli).

You can install it with:

```shell
winget install qownnotes
```

## MSYS2

There is a [community-maintained package of QOwnNotes](https://packages.msys2.org/base/mingw-w64-qownnotes) for [MSYS2](hhttps://www.msys2.org/).

You can install it with:

```shell
pacman -S mingw-w64-qownnotes
```
