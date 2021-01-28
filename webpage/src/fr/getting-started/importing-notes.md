# Importer des notes

## Evernote

Vous pouvez accéder à une boîte de dialogue d'importation Evernote dans le menu `Note / Importer`.

## Tomboy

Vous pouvez importer vos notes Tomboy en utilisant un script appelé [Trombone](https://github.com/samba/trombone).

Vous pouvez installer python2 pour faire fonctionner le script. Il convertira vos notes Tomboy en un fichier Evernote `.enex`, que vous pourrez ensuite importer dans QOwnNotes.

Pour tous ceux qui souhaitent faire cela, assurez-vous d'abord que Python2 est installé et que vous souhaiterez peut-être également installer `python-is-python2` (vous pourrez le supprimer plus tard):

```bash
sudo apt install python2 python-is-python2
```

Téléchargez le fichier trombone depuis GitHub, extrayez-le et sautez dans ce dossier:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Puis cd dans le dossier où se trouvent vos notes de garçon manqué:

```bash
 cd ~/.local/share/tomboy/
```

Ensuite, exécutez ceci:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Si vous obtenez des erreurs Unicode concernant des notes spécifiques, supprimez simplement chaque note et exécutez à nouveau la conversion jusqu'à ce qu'elle donne un message disant `Sauvegarde ...`. Vous aurez un fichier appelé `EXPORT.enex` qui pourra ensuite être importé dans QOwnNotes.

Lors de l'importation dans QOwnNotes, vous pouvez décocher tous les attributs à importer sauf peut-être la date de création / modification, car Tomboy ne possède pas ces fonctionnalités.
