# Jegyzetek importálása

## Evernote

Van egy Evernote importáló párbeszédpanel, amelyet elérhet a `Jegyzet / Import` menüben.

## Tomboy

A Tomboy Notes programot a [Trombone](https://github.com/samba/trombone) nevű szkript segítségével importálhatja.

Lehetséges, hogy telepíti a python2-t a szkript működéséhez. Konvertálja a Tomboy-jegyzeteit egy Evernote `.enex` fájlba, amelyet aztán importálhat a QOwnNotes fájlba.

Aki ezt meg akarja tenni, először ellenőrizze, hogy telepítve van-e a Python2, és érdemes lehet a `python-is-python2` -t is telepíteni (ezt később eltávolíthatja):

```bash
sudo apt install python2 python-is-python2
```

Töltse le a trombone fájlt a GitHubról, bontsa ki és ugorjon abba a mappába:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Ezután CD-t abba a mappába, ahol a jegyzetei vannak:

```bash
 cd ~/.local/share/tomboy/
```

Ezután futtassa ezt:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Ha unicode hibákat tapasztal egyes megjegyzésekkel kapcsolatban, egyszerűen távolítsa el az egyes jegyzeteket, és futtassa újra az átalakítást, amíg a `Mentés ...` üzenetet nem kap. Lesz egy `EXPORT.enex` nevű fájlja, amelyet aztán importálhat a QOwnNotes fájlba.

A QOwnNotes programba történő importálás során törölheti az összes importálandó attribútum jelölését, kivéve talán a létrehozás / módosítás dátumát, mivel a Tomboy nem rendelkezik ilyen tulajdonságokkal.
