# Importar notas

## Evernote

Dispone de un cuadro de diálogo de importación desde Evernote que puede abrir mediante el menú `Nota/Importar` para importar archivos _Enex_.

Se importarán asimismo las imágenes, los archivos adjuntos, las etiquetas, el formato básico y los metadatos.

## Joplin

Dispone de un diálogo de importación de Joplin al que puede acceder mediante el menú `Nota▸Importar`.

## Obsidian

1. Install the community plugin ["Better Markdown Links"](https://github.com/mnaoumov/obsidian-better-markdown-links) to convert all _wikilink and Obsidian flavoured markdown links (OFM)_ and make Obsidian operate with _standard markdown links_.

2. Open the same files (same vault) in QOwnNotes. Install the QOwnNotes script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) and press the import button "ObsidianImport" in the script panel to convert all files.

La exportación de QOwnNotes a Obsidian no es necesaria.

::: warning
Warning _Caution(!)_: The plugin `"Better Markdown Links"` silently adapts links when Obsidian is opened or when folders, files or headings are renamed. Press the "ObsidianImport" button in QOwnNotes everytime you have changed files in Obsidian, otherwise some links might not be recognised by QOwnNotes anymore!
:::

## VS Codium / Github / Markor / Typora / Pandoc and others (Github flavoured markdown GFM)

Githubflavoured markdown (GFM) uses hyphenstyle link headings (spaces converted to hyphens). A reference to a heading such as `## Heading with spaCes` for example will look like `[notelink](β_note.md#heading-with-spaces)`.

QOwnNotes and Obsidian (Obsidian flavoured markdown OFM) use _urlencoded (percentencoded) heading fragments_ (spaces converted to %20 and keeping capital letters), so the same link will look different `[notelink](%CE%B2_note.md#Heading%20with%20spaCes)`.

To convert between GFM and QOwnNotes/Obsidian (OFM), just open the files in QOwnNotes. Install the QOwnNotes script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) and press the import button "GFMImport" in the script panel to convert all files

La exportación de QOwnNotes a GFM funciona para una sola página (útil para una conversión consecutiva con pandoc) o para todos los archivos.

::: warning
Advertencia _Precaución_: ¡Posible pérdida de información en los enlaces de encabezado en casos especiales! Assume you have a note `β_note.md` with several identical headings like `## Advantages` within the _same (!) note_ . GFM allows to reference each heading with `[notelink](%CE%B2_note.md#advantages), [notelink](%CE%B2_note.md#advantages-1), [notelink](%CE%B2_note.md#advantages-2), ...`. _Neither QOwnNotes nor Obsidian allow for this!_ QOwnnotes and Obsidian both need unique heading names within the same document to be able to reference each of them! As this feature is not supported at all, after the conversion those links will all point to the first occuring heading with that particular name `[notelink](%CE%B2_note.md#advantages)` (this is especially relevant in long documents)! As a workaround just add text to the headings and update the links to the headings first (make them unique).
:::

## Formatos variados

La herramienta de órdenes de terminal [jimmy](https://github.com/marph91/jimmy) puede convertir varios formatos a Markdown (como [Google Keep](https://marph91.github.io/jimmy/formats/google_keep/), [Synology Note Station](https://marph91.github.io/jimmy/formats/synology_note_station/) y [más](https://marph91.github.io/jimmy/)). Para preservar tantos metadatos como sea posible, se recomienda aplicar [ajustes adicionales](https://marph91.github.io/jimmy/import_instructions/#qownnotes).

Si existen problemas con la conversión, siéntanse libre de abrir un boleto en [GitHub](https://github.com/marph91/jimmy/issues).

## Google Keep

> Descargue las tareas de Keep mediante Google Takeout
> 
> Extraiga el archivador
> 
> Descargue keep_to_markdown.py en el directorio junto a la carpeta de Keep extraída
> 
>     python keep_to_markdown.py Keep out/
>     
> 
> Reemplace Keep por la carpeta que contiene las notas en formato JSON. La carpeta out se creará si no existiese.

De <https://gitlab.com/-/snippets/2002921>

## Tomboy

Es posible importar sus notas de Tomboy a través de una secuencia de órdenes denominada [Trombone](https://github.com/samba/trombone).

Para que la secuencia de órdenes funcione, puede necesitar instalar python2. Esto convertirá sus notas de Tomboy en un archivo `.enex` de Evernote, el cual podrá importar en QOwnNotes.

Si desea efectuar este proceso, primero cerciórese de que ha instalado Python2 y, opcionalmente, `python-is-python2` (puede desinstalarlo posteriormente). Puede que esto sea más sencillo/seguro en un contenedor chroot o Docker si tiene esa opción (esto será documentado aquí posteriormente).

```bash
sudo apt install python2 python-is-python2
```

Descargue el archivo de trombone desde GitHub, extráigalo y entre en la carpeta resultante:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

A continuación, entre en la carpeta que contenga sus notas de Tomboy:

```bash
 cd ~/.local/share/tomboy/
```

Ahora, ejecute lo siguiente:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Si recibe errores relacionados con Unicode en ciertas notas, simplemente vaya quitándolas y vuelva a ejecutar la conversión hasta que se genere el mensaje `Saving...` («Guardando…»). Obtendrá un archivo con el nombre `EXPORT.enex`, el cual podrá importar en QOwnNotes.

Durante la importación a QOwnNotes, puede desmarcar todos los atributos de la importación excepto la fecha de creación / modificación, ya que Tomboy no contiene esas características.
