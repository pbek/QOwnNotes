# Importar notas

## Evernote

Dispone de un cuadro de diálogo de importación desde Evernote que puede abrir mediante el menú `Nota/Importar` para importar archivos _Enex_.

Se importarán asimismo las imágenes, los archivos adjuntos, las etiquetas, el formato básico y los metadatos.

## Joplin

Dispone de un diálogo de importación de Joplin al que puede acceder mediante el menú `Nota▸Importar`.

## Obsidian

1. Instale el complemento de la comunidad ["Better Markdown Links"](https://github.com/mnaoumov/obsidian-better-markdown-links) para convertir todos los _enlaces de Markdown con formato wikilink y Obsidian (OFM)_ y hacer que Obsidian funcione con _enlaces de Markdown estándar_.

2. Abra los mismos archivos (misma bóveda) en QOwnNotes. Instale el script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) de QOwnNotes y presione el botón de importación "ObsidianImport" en el panel de script para convertir todos los archivos.

La exportación de QOwnNotes a Obsidian no es necesaria.

::: warning
Warning _¡Precaución!_: El complemento `"Better Markdown Links"` adapta silenciosamente los enlaces cuando se abre Obsidian o cuando se renombran carpetas, archivos o encabezados. Presione el botón "ObsidianImport" en QOwnNotes cada vez que haya cambiado archivos en Obsidian. De lo contrario, ¡es posible que QOwnNotes ya no reconozca algunos enlaces!
:::

## VS Codium / Github / Markor / Typora / Pandoc y otros (Markdown con estilo o GFM)

Markdown con estilo GitHub (GFM) utiliza encabezados de enlaces con estilo de guiones (espacios convertidos en guiones). Una referencia a un encabezado como `## Encabezado con espacios`, por ejemplo, se verá como `[notelink](β_nota.md#encabezado-con-espacios)`.

QOwnNotes y Obsidian (Markdown con estilo de Obsidian u OFM) usan _fragmentos de encabezado codificados en URL (codificados por porcentaje)_ (espacios convertidos a %20 y manteniendo las letras mayúsculas), por lo que el mismo enlace se verá diferente `[notelink](%CE%B2_note.md#Encabezado%20con%20espacios)`.

Para convertir entre GFM y QOwnNotes/Obsidian (OFM), simplemente abra los archivos en QOwnNotes. Instale el script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) de QOwnNotes y presione el botón de importación "GFMImport" en el panel de scripts para convertir todos los archivos.

La exportación de QOwnNotes a GFM funciona para una sola página (útil para una conversión consecutiva con pandoc) o para todos los archivos.

::: warning
Advertencia _Precaución_: ¡Posible pérdida de información en los enlaces de encabezado en casos especiales! Supongamos que tiene una nota `β_nota.md` con varios encabezados idénticos como `## Ventajas` dentro de la _(¡)misma(!) nota_. GFM permite referenciar cada encabezado con `[notelink](%CE%B2_nota.md#ventajas), [notelink](%CE%B2_nota.md#ventajas-1), [notelink](%CE%B2_nota.md#ventajas-2), ...`. _¡Ni QOwnNotes ni Obsidian permiten esto!_ ¡Tanto QOwnnotes como Obsidian necesitan nombres de encabezado únicos dentro del mismo documento para poder hacer referencia a cada uno de ellos! Como esta función no está soportada en lo absoluto, después de la conversión todos esos enlaces apuntarán al primer encabezado que aparezca con ese nombre particular `[notelink](%CE%B2_note.md#ventajas)` (¡esto es especialmente relevante en documentos largos!). Como solución alternativa, simplemente agregue texto a los encabezados y actualice primero los enlaces a los encabezados (hágalos únicos).
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
