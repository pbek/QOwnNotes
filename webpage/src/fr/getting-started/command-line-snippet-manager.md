# Gestionnaire de snippets en ligne de commande

Vous pouvez utiliser le [Gestionnaire de snippets en ligne de commande de QOwnNotes](https://github.com/qownnotes/qc) pour **exécuter des snippets stockés dans des notes** se trouvant dans QownNotes depuis la ligne de commande.

![qc](/img/qc.png)

Vous pouvez utiliser des **notes avec un tag spécifique** pour y stocker **des snippets de commandes** que vous pouvez **exécuter depuis le Gestionnaire de snippets en ligne de commande**.

![commands](/img/commands.png)

## Installation

Visitez la [page de la dernière version](https://github.com/qownnotes/qc/releases/latest) et téléchargez la version dont vous avez besoin.

::: tip
Si vous avez installé [jq](https://stedolan.github.io/jq) vous pouvez également utiliser ce snippet pour, par exemple, télécharger et installer la dernière AppImage Linux AMD64 dans `/usr/local/bin/qc`:

```bash
curl https://api.github.com/repos/qownnotes/qc/releases/latest | \
jq '.assets[] | select(.browser_download_url | endswith("_linux_amd64.tar.gz")) | .browser_download_url' | \
xargs curl -Lo /tmp/qc.tar.gz && \
tar xfz /tmp/qc.tar.gz -C /tmp && \
rm /tmp/qc.tar.gz && \
sudo mv /tmp/qc /usr/local/bin/qc && \
/usr/local/bin/qc --version
```

:::

## Dépendances

[fzf](https://github.com/junegunn/fzf) (fuzzy search) ou [peco](https://github.com/peco/peco) (plus ancien mais plus probablement installé par défaut) doit être installé pour rechercher des commandes depuis la ligne de commande.

::: tip
`fzf` est utilisé par défaut pour les recherches, mais vous pouvez utiliser `peco` en le paramétrant à l'aide de `qc configure`.
:::

## Configuration

![socket-server-token](/img/socket-server-token.png)

Avant d'utiliser le gestionnaire de snippets de commandes, vous devez activer le _serveur de socket Web_ (2) dans la section _Extension de navigateur / Snippets de commandes_ (1) dans les préférences de QOwnNotes.

Vous devez ensuite afficher le jeton de sécurité (3) et le copier (4).

Ouvrez maintenant le fichier de configuration du gestionnaire de snippets avec :

```bash
# Configurer le gestionnaire de snippets
qc configure
```

Et placez le jeton de sécurité dans l'attribut `token` :

```toml
[QOwnNotes]
token = "votre_jeton_ici"
```

::: tip
Dans les préférences de QOwnNotes vous pouvez également définir quel tag devrait être utilisé pour rechercher des commandes dans les notes. Par défaut le tag `commands` est utilisé.
:::

## Syntaxe des snippets

Vous pouvez utiliser des **listes non-ordonnées avec des blocs de code en ligne** pour stocker des snippets de commandes. Toutes les notes avec le tag `commands` sont lues pour y trouver d'éventuels snippets de commandes.

Si vous ajoutez un `cmd:` avant le bloc de code en ligne, la commande sera aussi trouvée dans la **note courante** quels que soient les tags de cette dernière.

```markdown
- `echo Je suis une commande` Je suis une description #tag1 #tag2 #tag3

* `echo Je suis aussi une commande` Je suis une description #tag3 #tag4 #tag5

- cmd: `echo Je serai trouvée dans la note courante` Cette commande sera trouvée dans la note courante quels que soient ses tags

<!-- Exemple pour demander une entrée utilisateur -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Demande l’identifiant d’une requête de fusion et la checkout
```

Des **blocs de code `bash` ou `shell`** précédés par un en-tête niveau 2 ou plus élevé peuvent également être utilisés pour des snippets de commandes. Les tags sont également pris en charge s’ils sont placés entre un en-tête et un bloc de code.

    ## Faire ceci avec un bloc de code « bash »

    - ce texte sera du texte ignoré
    - mais des tags peuvent être utilisés : #tag1 #tag2

    ```bash
    echo fais ceci
    echo fais cela
    ```


    ## Faire autre chose avec un bloc de code « sh »

    ```sh
    echo fais autre chose
    echo fais encore autre chose
    ```

Les exemples ci-dessus résulteront en deux snippets de commandes, le premier avec les deux tags `tag1` et `tag2`.

## Utilisation

```bash
# Rechercher et exécuter des snippets de commandes
qc exec
```

```bash
# Rechercher et afficher des snippets de commandes
qc search
```

## Configuration

Exécutez `qc configure`.

```toml
[General]
  editor = "vim"            # votre éditeur de texte favori
  column = 40               # taille de colonne pour la commande list
  selectcmd = "fzf"         # sélecteur de commande pour éditer une commande (fzf ou peco)
  sortby = ""               # spécifie comment les extraits de commandes sont triés (recency (par défaut), -recency, description, -description, command, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # le jeton de l'API QOwnNotes
  websocket_port = 22222    # le port websocket dans QOwnNotes
```

## Complétion dans le Shell

Vous pouvez générer du code de complétion pour votre shell avec `qc completion <shell>`.

Pour le shell Fish vous pouvez par exemple utiliser :

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
