# Importare le note

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

Saranno automaticamente importati anche immagini, allegati, etichette, formattazione standard e i metadati.

## Joplin

C'è una voce per importare le note di Joplin che puoi raggiungere nel menù in ` Nota / Importa`.

## Various Formats

The command line tool [jimmy](https://github.com/marph91/jimmy) can convert various formats to Markdown (such as [Google Keep](https://marph91.github.io/jimmy/formats/google_keep/), [Synology Note Station](https://marph91.github.io/jimmy/formats/synology_note_station/) and [more](https://marph91.github.io/jimmy/)). To preserve as much metadata as possible, it's recommended to apply [additional tweaks](https://marph91.github.io/jimmy/import_instructions/#qownnotes).

If there are any issues with the conversion, feel free to open a ticket at [GitHub](https://github.com/marph91/jimmy/issues).

## Google Keep

> Scarica i Task di Keep usando Google Takeout
> 
> Estrai un archivio
> 
> Scarica keep_to_markdown.py nella cartella dov'è stata estratta la cartella di Keep
> 
>     python keep_to_markdown.py Keep out/
>     
> 
> Sostituisci Keep con la cartella contenente i file nel formato JSON. La cartella di output verrà creata se non esistente.

From <https://gitlab.com/-/snippets/2002921>

## Tomboy

Puoi importare le note di Tomboy tramite uno script chiamato [Trombone](https://github.com/samba/trombone).

Potresti dover installare python2 per far funzionare lo script. Convertirà le tue note di Tomboy in un file `Enex` di Evernote, che potrai importare su QOwnNotes.

Per chiunque desideri farlo, assicurati prima di aver installato Python2, inoltre potresti dover installare anche `python-is-python2` (puoi rimuoverlo in seguito). Potrebbe essere più facile e sicuro farlo all'interno dell'ambiente chroot o in un container Docker, se ne hai possibilità (sarà descritto qui prossimamente).

```bash
sudo apt install python2 python-is-python2
```

Scarica il file trombone da GitHub, estrailo e entra in quella cartella:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Dunque fai il comando cd all'interno della cartella in cui si trovano le tue note di Tomboy:

```bash
 cd ~/.local/share/tomboy/
```

Quindi esegui:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Se ricevi errori Unicode su note specifiche, rimuovi ognuna di queste note ed esegui di nuovo la conversione finché non viene visualizzato un messaggio che dice `Salvataggio in corso ...`. Avrai un file chiamato `EXPORT.enex` che può essere importato in QOwnNotes.

Durante l'importazione in QOwnNotes potresti dover deselezionare tutti gli attributi da importare tranne forse la data di creazione/modifica, poiché Tomboy non comprende questi attributi.
