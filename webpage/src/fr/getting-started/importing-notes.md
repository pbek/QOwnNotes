# Importer des notes

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

Images, attachments, tags, basic formatting and metadata will be imported as well.

## Joplin

There is a Joplin import dialog you can reach in the `Note / Import` menu.

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
