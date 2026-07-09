# Antisèche Markdown

Cette antisèche Markdown est conçue comme une référence rapide et un recueil d’exemples de la syntaxe Markdown dans QOwnNotes.

## Titres de sections

Utilisez des titres ATX pour structurer vos textes.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
Le **panneau de navigation** montre la structure de vos titres de sections.
:::

Alternativement, pour H1 et H2, vous pouvez aussi utiliser les titres Setex :

```markdown
Variante H1
======

Variante H2
------
```

::: tip
Par défaut, QOwnNotes crée le **nom de fichier d'une note** à partir du **titre de section 1** (h1).
:::

## Accentuation

```markdown
Accentuation (i.e. italique) avec des *astérisques* ou des _tirets bas_.

Accentuation forte (i.e. gras) avec des **astérisques** ou des __tirets bas__.

Accentuation combinée avec **astérisques et _tirets bas_**.
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + B</kbd> pour mettre le texte en gras et <kbd>Ctrl + I</kbd> pour le mettre en italique.
:::

## Souligné

Il existe également un paramètre facultatif pour activer le rendu souligné dans les _Préférences d’aperçu_.

```markdown
_souligner_
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + U</kbd> pour souligner du texte.
:::

## Rayer

```markdown
~~rayer~~
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Alt + Maj + S</kbd> pour rayer un texte.
:::

## Listes

Il existe de nombreuses façons de créer des listes.

```markdown
1. Premier élément de liste ordonnée
2. Un autre élément

- Sous liste non ordonnée.

1. Les nombres n’ont pas d’importance, il faut juste que ce soit un nombre
1. Et un autre élément.

* Les listes non-ordonnées peuvent utiliser des astérisques

- Des tirets

+ Ou des plus
```

::: tip
Si vous appuyez sur <kbd>Entrée</kbd> à la fin d'une liste, un nouvel élément de liste sera créé.
:::

## Liens

Deux des utilisations les plus simples des liens sont pointer vers une page web et pointer vers d’autres notes. Il y a plusieurs façons de faire chacun de ces liens.

### Liens externes

```markdown
[Je suis un lien sur une ligne](https://www.google.com)

[Je suis un lien sur une ligne avec un titre](https://www.google.com "Page d’accueil de Google")

[Vous pouvez utiliser des nombres pour des liens ressemblant à des références][1]

Les URL brutes et les URL entre chevrons seront automatiquement transformées en lien dans l’aperçu.
http://www.example.com ou <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Liens internes

Vous pouvez lier d’autres notes ou des pièces jointes comme suit :

```markdown
[Je pointe vers la note Journal.md](Journal.md)

<Journal.md> fonctionne de manière similaire.
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + L</kbd> pour **créer des liens vers des pages Web ou d’autres notes**.

L'utilisation de <kbd>Ctrl + Maj + X</kbd> fera apparaître une boîte de dialogue qui vous aidera à **insérer des pièces jointes** dans votre note.

Vous pouvez appuyer sur <kbd>Ctrl + Espace</kbd> pendant que le curseur se trouve sur un lien dans l'édition de note pour suivre le lien.
:::

::: warning
Info Les liens internes avec un titre ne sont pas supportés :

```markdown
[Je pointe vers la note Journal.md](Journal.md "Titre non supporté")
```

:::

### Signets

Les signets utilisés par [l'extension de navigateur QOwnNotes Web Companion](./browser-extension.md) utilisent des liens dans des listes.

```markdown
- [Nom de la page Web] (https://www.example.com) #tag1 #tag2 une description et des tags
```

## Images

Les images peuvent être intégrées dans QOwnNotes. Elles seront affichées dans l'aperçu.

```markdown
![description](media/mon-image.jpg)
```

### Dimensions d’image

Vous pouvez optionnellement spécifier la `largeur` et/ou la `hauteur` d’une image avec la syntaxe d’attributs utilisant des accolades après la parenthèse fermante :

```markdown
![description](media/mon-image.jpg){ width=300 }

![description](media/mon-image.jpg){ height=200 }

![description](media/mon-image.jpg){ width=300 height=200 }
```

Les attributs de dimensions sont affichés grisés dans l’éditeur et sont appliqués dans l’aperçu, où ils prennent la priorité sur le dimensionnement automatique des images.

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + Maj + I</kbd> pour insérer une image dans une note. L'image peut également être dans le presse-papiers, la boîte de dialogue la détectera et affichera un aperçu.

Vous pouvez également coller directement une image du presse-papiers dans votre note avec <kbd>Ctrl + Maj + V</kbd>.
:::

::: warning
Info Les liens d’images avec un titre ne sont pas supportés :

```markdown
![description](media/mon-image.jpg "Titre non supporté")
```

:::

## Code en ligne et blocs de code

```markdown
Le `code` en ligne est `placé entre deux accents graves`.
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + Maj + C</kbd> sur une portion de texte en ligne sélectionné – ou juste à l'intérieur du texte – pour créer un bloc de code en ligne.
:::

Les blocs de code sont délimités par des lignes avec trois accents graves ou sont indentés avec quatre espaces.

### Délimitation avec 4 espaces

Ajoutez quatre espaces devant votre code pour le marquer comme bloc de code.

```markdown
    s = "Code intenté avec des espaces"
    print s
```

### Délimitation avec des accents graves

Vous pouvez également utiliser trois accents graves pour créer un bloc de code.
````markdown
```
Le code va ici
Le code va ici
```
````

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + Maj + C</kbd> sur plusieurs lignes de texte sélectionnées ou dans une ligne vide pour créer un bloc de code.
:::

### Délimitation par accents graves avec coloration syntaxique

Il est possible d’ajouter de la coloration syntaxique dans les blocs de code avec QOwnNotes.

````markdown
```bash
# Je suis un commentaire
cd Notes
```
````
````

Pour l’instant, les langages supportés (et les identifiants de blocs de codes) sont les suivants :

- scripts BASh, `bash`
- C, `c`
- C++, `cpp`
- C++, `cxx`
- C++, `c++`
- C#, `c#`
- CMake, `cmake`
- C#, `csharp`
- CSS, `css`
- Go, `go`
- HTML, `html`
- INI, `ini`
- Java, `java`
- JavaScript, `javascript`
- JavaScript, `js`
- JSON, `json`
- Makefile, `make`
- PHP, `php`
- Python, `py`
- Python, `python`
- QML, `qml`
- Rust, `rust`
- scripts Shell, `sh`
- SQL, `sql`
- TypeScript, `ts`
- TypeScript, `typescript`
- V, `v`
- Vex, `vex`
- XML, `xml`
- YAML, `yml`
- YAML, `yaml`

## Tableaux

Les tableaux ne font pas partie de la spécification de base de Markdown, mais QOwnNotes les supporte.

```markdown
Les deux-points peuvent être utilisés pour aligner les colonnes.

|     Les tableaux    |       C’est      |  Cool |
| ------------------- | :--------------: | ----: |
| la colonne 3 est    | alignée à droite | $1600 |
| la colonne 2 est    |     centrée      |   $12 |
| les rayures zébrées |  sont chouettes  |    $1 |

Il doit y avoir au moins 3 tirets séparant chaque cellule d’en-tête.

Vous pouvez également utiliser la syntaxe Markdown.

| Du Markdown | Moins     | Beau                |
| ----------- | --------- | ------------------- |
| _Mais qui_  | `rend`    | **quand même bien** |
| 1           | 2         | 3                   |
````

::: tip
Appuyez sur <kbd>Alt + Maj + T</kbd> pour faire apparaître une boîte de dialogue qui vous aidera à créer des tableaux. Vous pouvez également importer des fichiers CSV depuis cette boîte de dialogue.

Utilisez <kbd>Ctrl + Espace</kbd> dans un tableau Markdown pour le formater automatiquement.
:::

## Blocs de citation

```markdown
> Les blocs de citation sont très pratiques dans les courriels pour émuler le texte de réponse.
> Cette ligne fait partie de la même citation.

Pause dans la citation.

> Voici une ligne très longue qui sera tout de même affichée correctement en tant que citation lorsqu’elle sera coupée avec un retour-ligne. Ouais, continuons à écrire pour être sûrs que c’est assez long pour que tout le monde voie le retour-ligne. Oh d’ailleurs, vous pouvez _mettre_ **du Markdown** dans les blocs de citation.
```

::: tip
Vous pouvez configurer QOwnNotes de telle sorte à ce qu’il surligne entièrement les blocs de citation ou juste le caractère de blocs de citation dans les _Préférences de l'éditeur_

Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + Maj + B</kbd> pour marquer le texte comme citation.
:::

## Règle horizontale

Il existe trois façons d'obtenir une règle horizontale : traits d'union, astérisques ou tirets bas.

```markdown
Trois ou plus...

Tirets

---

Astérisques

***

Tirets bas

___
```

## Sauts de ligne

- Vous pouvez construire un paragraphe comme un enchaînement de plusieurs lignes pour en faciliter l'édition, il sera tout de même affiché comme un seul paragraphe.
- Dans un paragraphe, vous pouvez forcer un retour à la ligne en insérant deux espaces à la fin d'une ligne.
- Vous pouvez définir un paragraphe distinct en le délimitant avec des sauts de ligne.

::: tip
Vous pouvez insérer deux espaces et un retour à la ligne avec <kbd>⇧ Shift</kbd> + <kbd>Entrée</kbd>.
:::

```markdown
Voici une ligne pour commencer.

Cette ligne est séparée de celle du dessus par deux retours-ligne, ce sera donc un _paragraphe séparé_.

Cette ligne commence également un paragraphe séparé, mais...  
Cette ligne n'est séparée que par deux espaces de fin et un seul retour-ligne, c'est donc une ligne distincte dans le _même paragraphe_.
```

::: tip
Les espaces de fin sont mises en évidence par défaut dans l'éditeur.
:::

## Commentaires

Les commentaires ne sont pas affichés dans l'aperçu.

```markdown
[commentaire] : # « Ce commentaire n’apparaîtra pas dans l’aperçu »

<!-- Les commentaires HTML sont aussi masqués -->
```

::: tip
Un bloc de commentaire HTML en tête d'une note sera également ignoré dans la génération automatique du nom de fichier de la note.
:::

## Listes de cases à cocher

Vous pouvez créer des listes de tâches simples avec des listes de cases à cocher.

```markdown
- [x] fait
- [ ] à faire
```

::: tip
Vous pouvez cocher / décocher les cases à cocher dans l'aperçu.
:::

## Métadonnées

Dans QOwnNotes, vous pouvez utiliser un avant-propos (par exemple YAML) pour ajouter des métadonnées supplémentaires. Il ne sera **pas affiché dans l'aperçu** et ne perturbera **pas la génération automatique de nom de fichier de note**.

```markdown
---
title: Un nom
description: Une description
---

# Le titre de la note commence ici

Du texte
```

Le nom de fichier de cet exemple de note serait `Le titre de la note commence ici.md`.
