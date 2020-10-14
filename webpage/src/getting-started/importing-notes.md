# Importing notes

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu.

## Tomboy
You can import your Tomboy Notes using a script called trombone: https://github.com/samba/trombone
You might install python2 to make the script work. It will convert your tomboy notes to an Evernote .enex file, which then is able to import to qOwnNotes.

For anyone wanting to do this, first make sure you have Python2 installed and you might want to also install python-is-python2 (you can remove that later):
 ```bash
sudo apt install python2 python-is-python2
```

download the trombone file from git, extract and cd into that folder:
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
If you get unicode errors about specific notes, just remove each note and run that line again until it gives a message saying "Saving...
" . You will have a file called EXPORT.enex which can then be imported into qOwnNotes.

During the import into qOwnNotes you might uncheck all of the attributes to import except maybe the creation/mod date, since Tomboy does not have those features
