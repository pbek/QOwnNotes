# Importer des notes

## Evernote

Vous pouvez accéder à une boîte de dialogue d'importation Evernote dans le menu `Note / Importer` pour importer des fichiers _Enex_.

Les images, pièces-jointes, tags, le formattage basique et les métadonnées seront également importés.

## Joplin

Il existe une boîte de dialogue d'importation Joplin dans le menu `Note / Importer`.

## Obsidian

1. Installez le plugin communautaire [« Better Markdown Links »](https://github.com/mnaoumov/obsidian-better-markdown-links) pour convertir tous les _wikilink et les liens Obsidian-flavoured Mardown (OFM)_ et faire en sorte qu’Obsidian fonctionne avec les _liens markdown standards_.

2. Ouvrez les mêmes fichiers (même coffre) dans QOwnNotes. Installez le script QOwnNotes [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) et appuyez sur le bouton d’import « ObsidianImport » dans le panneau de scripts pour convertir tous les fichiers.

L’export de QOwnNotes vers Obsidian n’est pas nécessaire.

::: warning
Avertissement _Attention (!)_ : le plugin `« Better Markdown Links »` adapte silencieusement les liens quand Obsidian est ouvert ou quand des dossiers, fichiers ou titres sont renommés. Appuyez sur le bouton « ObsidianImport » dans QOwnNotes à chaque fois que vous changez des fichiers dans Obsidian, sans quoi certains liens pourraient ne plus être reconnus par QOwnNotes !
:::

## VS Codium / GitHub / Markor / Typora / Pandoc et les autres (GitHub flavoured markdown, GFM)

Le GitHub Flavoured Markdown (GFM) utilise des liens de titres où les espaces sont converties en tirets. Une référence vers un titre tel que `## Titre avec espaces` ressemblera à `[lien vers une note](β_note.md#titre-avec-espaces)`.

QOwnNotes et Obsidian (Obsidian flavoured markdown, OFM) utilisent _des références urlencoded (percentencoded)_ (les espaces sont converties en %20 et les lettres majuscules sont gardées), donc le même lien ressemblera à `[lien vers une note](%CE%B2_note.md#Titre%20avec%20espaCes)`.

Pour convertir entre du GFM et QOwnNotes/Obsidian (OFM), ouvrez simplement les fichiers dans QOwnNotes. Installez le script QOwnNotes [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) et appuyez sur le bouton d’import « GFMImport » dans le panneau de scripts pour convertir tous les fichiers

L’exportation de QOwnNotes vers GFM fonctionne pour une seule page (utile pour une conversion consécutive avec pandoc) ou pour tous les fichiers.

::: warning
Avertissement _Attention_ : une perte d’information est possible dans les liens vers des titres dans certains cas ! Imaginez que vous ayez une note `β_note.md` avec plusieurs titres identiques comme `## Avantages` dans la _même (!) note_. Le GFM autorise le référencement de chaque titre avec `[lien vers une note](%CE%B2_note.md#avantages), [lien vers une note](%CE%B2_note.md#avantages-1), [lien vers une note](%CE%B2_note.md#avantages-2), …`. _Ni QOwnNotes ni Obsidian ne l’autorisent !_ QOwnNotes et Obsidian ont chacun besoin de nom de titres uniques dans le même document pour avoir la possibilité de référencer chacun d’entre eux ! Comme cette fonctionnalité n’est pas du tout prise en charge, après conversion, ces liens pointeront tous vers la première occurrence d’un titre avec ce nom particulier : `[lien vers une note](%CE%B2_note.md#avantages)` (c’est particulièrement pertinent dans les longs documents) ! Pour contourner ce problème, ajoutez simplement du texte à chaque titre et mettez à jour les liens vers ces titres (rendez-les uniques).
:::

## Autres formats

L’outil en ligne de commande [jimmy](https://github.com/marph91/jimmy) peut convertir divers formats en Markdown (comme [Google Keep](https://marph91.github.io/jimmy/formats/google_keep/), [Synology Note Station](https://marph91.github.io/jimmy/formats/synology_note_station/) et [plus](https://marph91.github.io/jimmy/)). Pour préserver le plus possible les métadonnées, il est recommandé d’appliquer [des ajustements additionnels](https://marph91.github.io/jimmy/import_instructions/#qownnotes).

S’il y a un quelconque problème avec la conversion, n’hésitez pas à ouvrir un ticket sur [GitHub](https://github.com/marph91/jimmy/issues).

## Google Keep

> Téléchargez Keep tasks avec Google Takeout
> 
> Extrayez l’archive
> 
> Téléchargez keep_to_markdown.py dans le dossier à côté de votre dossier Keep extrait
> 
>     python keep_to_markdown.py Keep out/
>     
> 
> Remplacez Keep par le dossier contenant les notes au format JSON. Le dossier out sera créé s’il n’existe pas.

De <https://gitlab.com/-/snippets/2002921>

## Tomboy

Vous pouvez importer vos notes Tomboy avec un script nommé [Trombone](https://github.com/samba/trombone).

Vous devez installer python2 pour faire fonctionner ce script. Il convertira vos notes Tomboy vers un fichier Evernote `.enex`, que vous pourrez ensuite importer dans QOwnNotes.

Pour tous ceux qui souhaitent faire cela, assurez-vous d'abord que Python2 est installé. Vous souhaiterez peut-être également installer `python-is-python2` (vous pourrez le supprimer plus tard). Il est sûrement plus simple/sécurisé de le faire dans un chroot ou un conteneur Docker, si vous avez cette option (elle sera documentée ici plus tard).

```bash
sudo apt install python2 python-is-python2
```

Téléchargez le fichier Trombone depuis GitHub, extrayez-le et allez dans ce dossier :

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Puis allez dans le dossier où se trouvent vos notes Tomboy :

```bash
 cd ~/.local/share/tomboy/
```

Exécutez ensuite ceci :

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

Si vous obtenez des erreurs Unicode concernant des notes spécifiques, supprimez simplement chaque note et exécutez à nouveau la conversion jusqu'à obtenir un message disant `Saving…`. Vous obtiendrez un fichier appelé `EXPORT.enex` qui pourra ensuite être importé dans QOwnNotes.

Lors de l'importation dans QOwnNotes vous pouvez décocher tous les attributs à importer – sauf peut-être les dates de création/modification – car Tomboy ne possède pas ces fonctionnalités.
