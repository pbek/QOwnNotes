# Importare le note

## Evernote

C'è una finestra di dialogo di importazione Evernote che puoi raggiungere nel menu `Nota / Importa`.

## Joplin

C'è una finestra di dialogo di importazione Joplin che puoi raggiungere nel menu `Nota / Importa`.

## Tomboy

Puoi importare le tue note Tomboy utilizzando uno script chiamato [Trombone](https://github.com/samba/trombone).

Potresti dover installare python2 per far funzionare lo script. Convertirà le tue note Tomboy in un file Evernote `.enex`, che potrai importare in QOwnNotes.

Per chiunque desideri farlo, assicurati prima di aver installato Python2 e potresti voler installare anche `python-is-python2` (puoi rimuoverlo in seguito):

```bash
sudo apt install python2 python-is-python2
```

Scarica il file trombone da GitHub, estrailo e vai in quella cartella:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Quindi entra nella cartella in cui sono le tue note Tomboy:

```bash
 cd ~/.local/share/tomboy/
```

Quindi esegui questo:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Se ricevi errori Unicode su note specifiche, rimuovi ognuna di queste note ed esegui di nuovo la conversione finché non viene visualizzato un messaggio che dice `Salvataggio in corso ...`. Avrai un file chiamato `EXPORT.enex` che può essere importato in QOwnNotes.

Durante l'importazione in QOwnNotes dovresti deselezionare tutti gli attributi da importare tranne forse la data di creazione / modifica, poiché Tomboy non ha queste caratteristiche.
