# Gestionnaire de snippets en ligne de commande

Vous pouvez utiliser le [Gestionnaire d'extraits de commandes en ligne de commande de QOwnNotes](https://github.com/qownnotes/qc) pour **exécuter des extraits de commandes stockés dans des notes** se trouvant dans QownNotes depuis la ligne de commande.

![qc](/img/qc.png)

Vous pouvez utiliser des **notes avec une étiquette spécifique** pour y stocker **des extraits de commandes** que vous pouvez **exécuter depuis le Gestionnaire d'extraits de commandes en ligne de commande**.

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

Before you are using the snippet manager you need to enable the _Web socket server_ (2) in the _Browser extension / command snippets_ (1) settings in QOwnNotes.

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
Dans les paramètres de QOwnNotes vous pouvez également définir quelle étiquette devrait être utilisée pour rechercher des commandes dans les notes. Par défaut l'étiquette `commands` est utilisée.
:::

## Syntaxe des extraits de commandes

Vous pouvez utiliser des **listes non-ordonnées avec des blocs de code en ligne** pour stocker des extraits de commandes. Toutes les notes avec l'étiquette `commands` sont lues pour y trouver d'éventuels extraits de commandes.

Si vous ajoutez un `cmd:` avant le bloc de code en ligne, la commande sera aussi trouvée dans la **note courante** quelles que soient les étiquettes de cette dernière.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3

* `echo I am also a command` I am a description #tag3 #tag4 #tag5

- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags

<!-- Example for asking for user input -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

Des **blocs de code `bash` ou `shell`** précédés par un en-tête niveau 2 ou plus élevé peuvent également être utilisés pour des extraits de commandes. Les étiquettes sont également prises en charge si elles sont placées entre un en-tête et un bloc de code.

    ## Faire ceci avec un bloc de code "bash"

    - ce texte sera du texte ignoré
    - mais des étiquettes peuvent être utilisées : #étiquette1 #étiquette2

    ```bash
    echo fais ceci
    echo fais cela
    ```

    ## Faire autre chose avec un bloc de code "sh"

    ```sh
    echo fais autre chose
    echo fais encore autre chose
    ```

Les exemples ci-dessus résulteront en deux extraits de commandes, le premier avec les deux étiquettes `étiquette1` et `étiquette2`.

## Utilisation

```bash
# Rechercher et exécuter des extraits de commandes
qc exec
```

```bash
# Rechercher et afficher des extraits de commandes
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

Pour le Fish shell vous pouvez par exemple utiliser :

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
