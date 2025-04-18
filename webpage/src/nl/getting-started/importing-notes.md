# Notities importeren

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

Afbeeldingen, bijlagen, labels, basisopmaak en metagegevens worden ook geïmporteerd.

## Joplin

Er is een Joplin-importdialoog welke u kunt bereiken in het menu `Notitie / Importeren `.

## Verschillende formaten

De opdrachtregel [jimmy](https://github.com/marph91/jimmy) kan verschillende formaten converteren naar Markdown (zoals [Google Keep](https://marph91.github.io/jimmy/formats/google_keep/), [Synology Note Station](https://marph91.github.io/jimmy/formats/synology_note_station/) en [meer](https://marph91.github.io/jimmy/)). Om zoveel mogelijk metadata te behouden, is het raadzaam om [extra aanpassingen](https://marph91.github.io/jimmy/import_instructions/#qownnotes) toe te passen.

Als er problemen zijn met de conversie, open dan gerust een ticket op [GitHub](https://github.com/marph91/jimmy/issues).

## Google Keep

> Download Keep-taken met Google Takeout
> 
> Archief uitpakken
> 
> Download keep_to_markdown.py naar map naast map met uitgepakte Keep-taken
> 
>     python keep_to_markdown.py Keep out/
>     
> 
> Vervang Keep door de map met notities in JSON-formaat. De out-map wordt aangemaakt als deze niet bestaat.

Van <https://gitlab.com/-/snippets/2002921>

## Tomboy

U kunt uw Tomboy-notities importeren met een script genaamd [Trombone](https://github.com/samba/trombone).

U kunt python2 installeren om het script te laten werken. Het converteert uw Tomboy-notities naar een Evernote `.enex` -bestand die u vervolgens kunt importeren in QOwnNotes.

Voor iedereen die dit wil doen, zorg er eerst voor dat Python2 is geïnstalleerd en u zou ook `python-is-python2` kunnen installeren (dit kan later verwijderd worden). Dit kan gemakkelijker/veiliger zijn in een chroot- of Docker-container, als u die mogelijkheid hebt (wordt hier later gedocumenteerd).

```bash
sudo apt install python2 python-is-python2
```

Download het trombone-bestand van GitHub, pak het uit en navigeer naar die map:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

cd vervolgens naar de map waar zich de tomboy-notities bevinden:

```bash
 cd ~/.local/share/tomboy/
```

Voer dan dit uit:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Treden er unicode-fouten op bij specifieke notities? Verwijder dan gewoon elke notitie en voer de conversie opnieuw uit totdat deze het bericht geeft `Opslaan...` . Dit levert een bestand met de naam `EXPORT.enex` dat vervolgens in QOwnNotes kan worden geïmporteerd.

Tijdens de import in QOwnNotes kunt u alle te importeren attributen uitschakelen, behalve misschien de aanmaak-/wijzigingsdatum, aangezien Tomboy die kenmerken niet ondersteunt.
