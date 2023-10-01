# Notities importeren

## Evernote

Er is een Evernote-importdialoog welke u kunt bereiken in het menu ` Notitie / Importeren ` om * Enex* -bestanden te importeren.

Afbeeldingen, bijlagen, labels, basisopmaak en metagegevens worden ook geïmporteerd.

## Joplin

Er is een Joplin-importdialoog welke u kunt bereiken in het menu `Notitie / Importeren `.

## Tomboy

U kunt uw Tomboy-notities importeren met een script genaamd [Trombone](https://github.com/samba/trombone).

U kunt python2 installeren om het script te laten werken. Het converteert uw Tomboy-notities naar een Evernote `.enex ` -bestand die u vervolgens kunt importeren in QOwnNotes.

Voor iedereen die dit wil doen, zorg er eerst voor dat Python2 is geïnstalleerd en misschien wilt u ook `python-is-python2` installeren (dat kan later verwijderd worden):

```bash
sudo apt install python2 python-is-python2
```

Download het trombone-bestand van GitHub, pak het uit en navigeer naar die map:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

CD dan in de map waar zich de tomboy-notities bevinden:

```bash
 cd ~/.local/share/tomboy/
```

Voer dan dit uit:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Indien er unicode-fouten optreden over specifieke notities? Verwijder dan gewoon elke notitie en voer de conversie opnieuw uit totdat deze het bericht geeft `Opslaan...` . Dit levert een bestand met de naam `EXPORT.enex` dat vervolgens in QOwnNotes kan worden geïmporteerd.

Tijdens de import in QOwnNotes kunt u alle te importeren attributen uitschakelen, behalve misschien de aanmaak- / wijzigingsdatum aangezien Tomboy die kenmerken niet heeft.
