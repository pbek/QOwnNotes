# Importowanie notatek

## Evernote

W menu `Notatka / Importuj` znajduje się okno dialogowe importowania Evernote.

## Joplin

Istnieje okno dialogowe importu Joplin, do którego można przejść w menu `Notatka / Importuj`.

## Tomboy

Możesz zaimportować swoje notatki Tomboy za pomocą skryptu o nazwie [Trombone](https://github.com/samba/trombone).

Możesz zainstalować python2, aby skrypt działał. Konwertuje twoje notatki Tomboy do pliku Evernote `.enex`, który następnie możesz zaimportować do QOwnNotes.

Dla każdego, kto chce to zrobić, najpierw upewnij się, że masz zainstalowany Python2 i możesz również zainstalować `python-is-python2` (możesz to później usunąć):

```bash
sudo apt install python2 python-is-python2
```

Pobierz plik puzonu z GitHub, rozpakuj i wskocz do tego folderu:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Następnie przejdź do folderu, w którym znajdują się twoje notatki chłopca:

```bash
 cd ~/.local/share/tomboy/
```

Następnie uruchom to:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Jeśli otrzymujesz błędy Unicode dotyczące określonych notatek, po prostu usuń każdą notatkę i ponownie uruchom konwersję, aż pojawi się komunikat `Zapisywanie...`. Będziesz mieć plik o nazwie `EXPORT.enex`, który można następnie zaimportować do QOwnNotes.

Podczas importu do QOwnNotes możesz odznaczyć wszystkie atrybuty do zaimportowania, z wyjątkiem być może daty utworzenia / modyfikacji, ponieważ Tomboy nie ma tych funkcji.
