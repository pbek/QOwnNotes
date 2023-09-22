# Notizen importieren

## Evernote

Es gibt einen Evernote-Importdialog, den Sie im Menü `Notiz / Importieren` erreichen können.

## Joplin

Es gibt einen Joplin-Importdialog, den Sie im Menü `Notiz / Importieren` erreichen.

## Tomboy

Sie können Ihre Tomboy Notes mit einem Skript namens [Trombone](https://github.com/samba/trombone) importieren.

Sie müssen eventuell Python 2 installieren, damit das Skript funktioniert. Es konvertiert Ihre Tomboy-Notizen in eine Evernote `.enex`-Datei, die Sie dann in QOwnNotes importieren können.

Wenn Sie dies tun möchten, stellen Sie zunächst sicher, dass Python 2 installiert ist, und installieren Sie möglicherweise auch `python-is-python2` (Sie können das später entfernen):

```bash
sudo apt install python2 python-is-python2
```

Laden Sie die Trombone-Datei von GitHub herunter, extrahieren Sie sie und springen Sie in diesen Ordner:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Navigieren Sie dann mit cd in den Ordner, in dem sich Ihre Tomboy-Notizen befinden:

```bash
 cd ~/.local/share/tomboy/
```

Führen Sie dann Folgendes aus:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Wenn Sie Unicode-Fehler zu bestimmten Notizen erhalten, entfernen Sie einfach jede Notiz und führen Sie die Konvertierung erneut aus, bis die Meldung `Speichern...` angezeigt wird. Sie erhalten eine Datei mit dem Namen `EXPORT.enex`, die dann in QOwnNotes importiert werden kann.

Während des Imports in QOwnNotes können Sie alle zu importierenden Attribute, außer vielleicht dem Erstellungs-/Änderungsdatum, deaktivieren, da Tomboy nicht über diese Funktionen verfügt.
