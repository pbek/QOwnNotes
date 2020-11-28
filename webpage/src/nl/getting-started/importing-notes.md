# Notities importeren

## Evernote

Er is een Evernote-importdialoog die u kunt bereiken in het menu `Notitie / Importeren`.

## Tomboy

U kunt uw Tomboy-notities importeren met behulp van een script genaamd [Trombone](https://github.com/samba/trombone).

U zou python2 kunnen installeren om het script te laten werken. Het converteert je Tomboy-notities naar een Evernote `.enex` -bestand, dat je vervolgens kunt importeren in QOwnNotes.

Voor iedereen die dit wil doen, zorg er eerst voor dat Python2 is geïnstalleerd en misschien wil je ook ` python-is-python2 ` installeren (je kunt dat later verwijderen):

```bash
sudo apt install python2 python-is-python2
```

Download het trombone-bestand van GitHub, pak het uit en spring naar die map:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Cd dan naar de map waar je tomboy-aantekeningen zijn:

```bash
 cd ~/.local/share/tomboy/
```

Voer dan dit uit:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

If you get unicode errors about specific notes, just remove each note and run the conversion again until it gives a message saying `Saving...` . You will have a file called `EXPORT.enex` which can then be imported into QOwnNotes.

During the import into QOwnNotes you might uncheck all of the attributes to import except maybe the creation/modification date, since Tomboy does not have those features.
