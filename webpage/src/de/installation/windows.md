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

## Windows XP

Qt hat die Unterstützung für Windows XP mit Version 5.8 eingestellt, aber QOwnNotes wurde jetzt auch mit Qt 5.7 erstellt, damit Windows XP-Benutzer es weiterhin verwenden können.

Sie müssen die ZIP-Datei von [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) selbst herunterladen und in einen Ordner Ihrer Wahl entpacken.

Sie können dann `QOwnNotes.exe` direkt aus diesem Ordner ausführen, es ist keine Installation erforderlich.

::: tip
Info
Der automatische Update-Mechanismus funktioniert nicht mit dem AppVeyor-Build für Windows XP!
Sie müssen neue Versionen selbst herunterladen.
:::

## Chocolatey

Unter [Chocolatey](https://chocolatey.org/packages/qownnotes/) gibt es ein von der Community gepflegtes Paket mit QOwnNotes.

Sie können es installieren mit:

```shell
choco install qownnotes
```

## Scoop

Es gibt ein [von der Community gepflegtes Paket von QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) bei [Scoop](https://scoop.sh/). Wenn Sie den Extras-Bucket hinzufügen, können Sie ihn verwenden, um QOwnNotes im portablen Modus zu installieren.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
