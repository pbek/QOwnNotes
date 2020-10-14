# Importing notes

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu.

## Tomboy
I was successful importing most all of my Tomboy Notes using an script called trombone: https://github.com/samba/trombone
I had to install python2 to make it work, but it converted all of my tomboy notes to an Evernote .enex file, which then was able to import to qOwnNotes.

For anyone wanting to do this, first make sure you have Python2 installed and you might want to also install python-is-python2 (you can remove that later):
```
sudo apt install python2 python-is-python2
```

download the trombone file from git, extract and cd into that folder:
```
cd ~/Downloads/trombone-master

sudo make
sudo make install
```
Then cd into the folder where your tomboy notes are:
```
cd ~/.local/share/tomboy/
```
Then run this:
```
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```
I got a few unicode errors on a couple of my notes. I just removed each note and ran that line again and it then it worked. I ended up with a file called EXPORT.enex which then imported into qOwnNotes.

During the import into qOwnNotes I unchecked all of the attributes to import except the creation/mod date.
