# Importer des notes

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

Images, attachments, tags, basic formatting and metadata will be imported as well.

## Joplin

There is a Joplin import dialog you can reach in the `Note / Import` menu.

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

## Various Formats

The command line tool [jimmy](https://github.com/marph91/jimmy) can convert various formats to Markdown (such as [Google Keep](https://marph91.github.io/jimmy/formats/google_keep/), [Synology Note Station](https://marph91.github.io/jimmy/formats/synology_note_station/) and [more](https://marph91.github.io/jimmy/)). To preserve as much metadata as possible, it's recommended to apply [additional tweaks](https://marph91.github.io/jimmy/import_instructions/#qownnotes).

If there are any issues with the conversion, feel free to open a ticket at [GitHub](https://github.com/marph91/jimmy/issues).

## Google Keep

> Download Keep tasks using Google Takeout
> 
> Extract archive
> 
> Download keep_to_markdown.py to directory next to extracted Keep folder
> 
>     python keep_to_markdown.py Keep out/
>     
> 
> Replace Keep by the folder containing notes in JSON format. The out folder will be created if it doesn't exist.

From <https://gitlab.com/-/snippets/2002921>

## Tomboy

You can import your Tomboy Notes using a script called [Trombone](https://github.com/samba/trombone).

You might install python2 to make the script work. It will convert your Tomboy notes to an Evernote `.enex` file, which then you are able to import to QOwnNotes.

For anyone wanting to do this, first make sure you have Python2 installed and you might want to also install `python-is-python2` (you can remove that later). This may be easier/safer in a chroot or Docker container, if you have that option (will be documented here later).

```bash
sudo apt install python2 python-is-python2
```

Download the trombone file from GitHub, extract and jump into that folder:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Then cd into the folder where your tomboy notes are:

```bash
 cd ~/.local/share/tomboy/
```

Then run this:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

If you get unicode errors about specific notes, just remove each note and run the conversion again until it gives a message saying `Saving...` . You will have a file called `EXPORT.enex` which can then be imported into QOwnNotes.

During the import into QOwnNotes you might uncheck all of the attributes to import except maybe the creation/modification date, since Tomboy does not have those features.
