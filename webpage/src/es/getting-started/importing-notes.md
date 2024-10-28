# Importar notas

## Evernote

Dispone de un cuadro de diálogo de importación desde Evernote que puede abrir mediante el menú `Nota▸Importar` para importar archivos *Enex*.

Se importarán asimismo las imágenes, los archivos adjuntos, las etiquetas, el formato básico y los metadatos.

## Joplin

There is a Joplin import dialog you can reach in the `Note / Import` menu.

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

De https://gitlab.com/-/snippets/2002921

## Tomboy

Es posible importar sus notas de Tomboy a través de una secuencia de órdenes denominada [Trombone](https://github.com/samba/trombone).

Para que la secuencia de órdenes funcione, puede necesitar instalar python2. Convertirá sus notas de Tomboy en un archivo `.enex` de Evernote, el cual podrá importar en QOwnNotes.

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

During the import into QOwnNotes you might uncheck all of the attributes to import except maybe the creation/modification date, since Tomboy does not have those features.
