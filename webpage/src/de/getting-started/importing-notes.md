# Notizen importieren

## Evernote

Es gibt einen Evernote-Importdialog, den Sie über die Menüpunkte `Notiz / Import` erreichen können, um *Enex*-Dateien zu importieren.

Bilder, Anhänge, Schlagworte, grundlegende Formatierungen und Metadaten werden ebenfalls importiert.

## Joplin

Es gibt einen Joplin-Importdialog, den Sie über die Menüpunkte `Notiz / Import` erreichen können.

## Tomboy

Sie können Ihre Tomboy-Notizen importieren, indem Sie ein Skript namens [Trombone](https://github.com/samba/trombone) verwenden.

Möglicherweise müssen sie Python 3 installieren, damit das Skript funktioniert. Es konvertiert Ihre Tomboy-Notizen zu einer Evernote `.enex`-Datei, die Sie dann nach QOwnNotes importieren können.

Falls Sie das machen möchten, gehen Sie zunächst sicher, dass Sie Python 2 installiert haben. Vielleicht möchten Sie auch `python-is-python2` installieren (Sie können es später wieder entfernen):

```bash
sudo apt install python2 python-is-python2
```

Laden Sie die Trombone-Datei von GitHub herunter, extrahieren Sie sie und springen Sie in den Ordner:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Navigieren Sie dann mit cd in den Ordner, wo Ihre Tomboy-Notizen sind:

```bash
 cd ~/.local/share/tomboy/
```

Führen Sie dann folgendes aus:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Falls Sie Unicode-Fehler für gewisse Notizen erhalten, entfernen Sie einfach jede Notiz einzeln und führen die Konvertierung erneut aus, bis eine Nachricht `Saving...` angezeigt wird. Sie haben dann eine Datei namens `EXPORT.enex`, die dann nach QOwnNotes importiert werden kann.

Währen der Importierung nach QOwnNotes sollten Sie eventuell den Import der Attribute deaktivieren, außer vielleicht dem Erstellungs-/Modifizierungsdatum, da Tomboy diese Funktionen nicht unterstützt.
