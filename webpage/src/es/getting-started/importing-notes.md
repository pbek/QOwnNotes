# Importar notas

## Evernote

Hay un cuadro de diálogo de importación de Evernote al que puede acceder en el menú `Nota / Importar`.

## Tomboy

Puede importar sus Tomboy Notes usando un script llamado [Trombone](https://github.com/samba/trombone).

Puede instalar python2 para que el script funcione. Convertirá sus notas de Tomboy en un archivo Evernote `.enex`, que luego podrá importar a QOwnNotes.

Para cualquiera que desee hacer esto, primero asegúrese de tener Python2 instalado y es posible que desee instalar también `python-is-python2` (puede eliminarlo más tarde):

```bash
sudo apt install python2 python-is-python2
```

Descargue el archivo de trombón de GitHub, extráigalo y salte a esa carpeta:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Luego cd en la carpeta donde están tus notas de tomboy:

```bash
 cd ~/.local/share/tomboy/
```

Entonces ejecuta esto:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Si obtiene errores Unicode sobre notas específicas, simplemente elimine cada nota y ejecute la conversión nuevamente hasta que aparezca un mensaje que diga `Guardando ...`. Tendrá un archivo llamado `EXPORT.enex` que luego se puede importar a QOwnNotes.

Durante la importación a QOwnNotes, puede desmarcar todos los atributos para importar excepto tal vez la fecha de creación / modificación, ya que Tomboy no tiene esas características.
