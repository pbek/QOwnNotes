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

Als u unicode-fouten krijgt over specifieke opmerkingen, verwijdert u gewoon elke notitie en voert u de conversie opnieuw uit totdat er een bericht verschijnt met de melding `Saving...`. Je hebt een bestand met de naam `EXPORT.enex` dat vervolgens kan worden geïmporteerd in QOwnNotes.

Tijdens het importeren in QOwnNotes kunt u alle te importeren attributen uitschakelen, behalve misschien de aanmaak- / wijzigingsdatum, aangezien Tomboy deze functies niet heeft.
