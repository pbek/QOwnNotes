# Installieren Sie unter Microsoft Windows ™

Laden Sie das neueste **Windows-Archiv** von [QOwnNotes Releases auf GitHub](https://github.com/pbek/QOwnNotes/releases) herunter und entpacken Sie es an einen beliebigen Ort. Es ist keine Installation erforderlich!

::: warning
Wenn Sie den **automatischen Updater** verwenden möchten, stellen Sie sicher, dass Sie ihn an einen Ort entpacken, auf den Ihr Benutzerkonto Schreibzugriff hat. Standardmäßig hat Ihr Benutzerkonto höchstwahrscheinlich **keinen Schreibzugriff** auf Orte wie `C:\Programme (x86)` oder `C:\Programme`.
:::

Sie können dann `QOwnNotes.exe` direkt in Ihrem Ordner `QOwnNotes` ausführen. Eine Installation ist nicht erforderlich.

### Portabler Modus

Verwenden Sie `QOwnNotesPortable.bat`, um QOwnNotes im **portablen Modus** auszuführen, in dem alles (einschließlich Ihrer Notizen) nur in Ihrem `QOwnNotes`-Ordner gespeichert wird.

::: tip
Sie benötigen den tragbaren Modus nicht, wenn Sie nur keine Administratorrechte für Ihren Computer haben. QOwnNotes braucht nicht installiert werden!
:::

## Windows XP

Qt hat die Unterstützung für Windows XP mit Version 5.8 eingestellt, QOwnNotes wurde jetzt auch mit Qt 5.7 erstellt, damit Windows XP-Benutzer es weiterhin verwenden können.

You need to download the ZIP file from [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) yourself and unzip it to a folder of your liking.

You can then directly run `QOwnNotes.exe` from that folder, no installation is required.

::: tip Info
The automatic update mechanism does not work with the AppVeyor build for Windows XP! You will have to download new releases yourself.
:::

## Chocolatey

There is a community maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

You can install it with:

```shell
choco install qownnotes
```

## Scoop

There is a [community maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
