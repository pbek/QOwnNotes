# Notizen importieren

## Evernote

Es gibt einen Evernote-Importdialog, den Sie im Menü ` Hinweis / Import ` erreichen können.

## Tomboy

Sie können Ihre Tomboy Notes mit einem Skript namens [Trombone](https://github.com/samba/trombone) importieren.

Sie können python2 installieren, damit das Skript funktioniert. Es konvertiert Ihre Tomboy-Notizen in eine Evernote ` .enex ` -Datei, die Sie dann in QOwnNotes importieren können.

Wenn Sie dies tun möchten, stellen Sie zunächst sicher, dass Python2 installiert ist, und installieren Sie möglicherweise auch ` python-is-python2 ` (das können Sie später entfernen):

```bash
sudo apt install python2 python-is-python2
```

Download the trombone file from GitHub, extract and jump into that folder:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Dann cd in den Ordner, in dem sich Ihre Wildfangnotizen befinden:

```bash
 cd ~/.local/share/tomboy/
```

Führen Sie dann Folgendes aus:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Wenn Sie Unicode-Fehler zu bestimmten Notizen erhalten, entfernen Sie einfach jede Notiz und führen Sie die Konvertierung erneut aus, bis die Meldung ` Speichern ... ` angezeigt wird. Sie haben eine Datei mit dem Namen ` EXPORT.enex `, die dann in QOwnNotes importiert werden kann.

Während des Imports in QOwnNotes können Sie alle zu importierenden Attribute außer dem Erstellungs- / Änderungsdatum deaktivieren, da Tomboy über diese Funktionen nicht verfügt.
