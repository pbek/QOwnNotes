# Notities importeren

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

Afbeeldingen, bijlagen, labels, basisopmaak en metagegevens worden ook geïmporteerd.

## Joplin

Er is een Joplin-importdialoog welke u kunt bereiken in het menu `Notitie / Importeren `.

## Obsidian

1. Install the community plugin ["Better Markdown Links"](https://github.com/mnaoumov/obsidian-better-markdown-links) to convert all _wikilink and Obsidian flavoured markdown links (OFM)_ and make Obsidian operate with _standard markdown links_.

2. Open the same files (same vault) in QOwnNotes. Install the QOwnNotes script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) and press the import button "ObsidianImport" in the script panel to convert all files.

Exporting from QOwnNotes to Obsidian is not needed.

::: warning
Warning _Caution(!)_: The plugin `"Better Markdown Links"` silently adapts links when Obsidian is opened or when folders, files or headings are renamed. Press the "ObsidianImport" button in QOwnNotes everytime you have changed files in Obsidian, otherwise some links might not be recognised by QOwnNotes anymore!
:::

## VS Codium / Github / Markor / Typora / Pandoc and others (Github flavoured markdown GFM)

Githubflavoured markdown (GFM) uses hyphenstyle link headings (spaces converted to hyphens). A reference to a heading such as `## Heading with spaces` for example will look like `[notelink](β_note.md#heading-with-spaces)`.

QOwnNotes and Obsidian (Obsidian flavoured markdown OFM) use _urlencoded (percentencoded) heading fragments_ (spaces converted to %20 and keeping capital letters), so the same link will look different `[notelink](%CE%B2_note.md#Heading%20with%20spaCes)`.

To convert between GFM and QOwnNotes/Obsidian (OFM), just open the files in QOwnNotes. Install the QOwnNotes script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) and press the import button "GFMImport" in the script panel to convert all files

Exporting from QOwnNotes to GFM works for a single page (useful for a consecutive conversion with pandoc) or for all files.

::: warning
Warning _Caution_: Possible loss of information in heading links in special cases! Assume you have a note `β_note.md` with several identical headings like `## Advantages` within the _same (!) note_ . GFM allows to reference each heading with `[notelink](%CE%B2_note.md#advantages), [notelink](%CE%B2_note.md#advantages-1), [notelink](%CE%B2_note.md#advantages-2), ...`. _Neither QOwnNotes nor Obsidian allow for this!_ QOwnnotes and Obsidian both need unique heading names within the same document to be able to reference each of them! As this feature is not supported at all, after the conversion those links will all point to the first occuring heading with that particular name `[notelink](%CE%B2_note.md#advantages)` (this is especially relevant in long documents)! As a workaround just add text to the headings and update the links to the headings first (make them unique).
:::

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
