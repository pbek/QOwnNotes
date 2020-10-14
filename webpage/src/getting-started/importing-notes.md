# Importing notes

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu.

## Tomboy

You can import your Tomboy Notes using a script called [Trombone](https://github.com/samba/trombone).

You might install python2 to make the script work. It will convert your Tomboy notes to an Evernote `.enex` file,
which then you are able to import to QOwnNotes.

For anyone wanting to do this, first make sure you have Python2 installed and you might want to also install
`python-is-python2` (you can remove that later):

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

If you get unicode errors about specific notes, just remove each note and run the conversion again until it gives a message
saying `Saving...` . You will have a file called `EXPORT.enex` which can then be imported into QOwnNotes.

During the import into QOwnNotes you might uncheck all of the attributes to import except maybe the creation/modification date,
since Tomboy does not have those features.
