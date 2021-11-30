# Command-line Snippet Manager

A [QOwnNotes Command-line Snippet Manager](https://github.com/qownnotes/qc) segítségével **futtathatja a jegyzetekben tárolt parancsrészleteket** a QOwnNotes alkalmazásban a parancssorból.

![qc](/img/qc.png)

**Speciális címkével ellátott megjegyzések** segítségével tárolhatja a **parancsrészleteket**, amelyeket **a parancssori kódrészletkezelőből hajthat végre**.

![commands](/img/commands.png)

## Telepítés

Keresse fel a [legfrissebb kiadás oldalát](https://github.com/qownnotes/qc/releases/latest) és töltse le a kívánt verziót.

::: tip
Ha telepítve van a [jq](https://stedolan.github.io/jq), ezt a kódrészletet használhatja például a legújabb Linux AMD64 AppImage letöltéséhez és telepítéséhez a `/usr/local/bin/qc` címre:

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

## Függőségek

Az [fzf](https://github.com/junegunn/fzf) (fuzzy search) vagy a [peco](https://github.com/peco/peco) (régebbi, de nagyobb valószínűséggel alapértelmezés szerint telepítve) telepítése szükséges a parancsok kereséséhez a parancssorban.

::: tip
Alapértelmezés szerint az `fzf` kifejezést használja a kereséshez, de használhatja a `peco`-t, ha beállítja a `qc configure` paranccsal.
:::

## Beállítás

![socket-server-token](/img/socket-server-token.png)

A kódrészletkezelő használata előtt engedélyeznie kell a *Web socket szervert* (2) a QOwnNotes *Böngészőbővítmény / parancsrészletek* (1) beállításainál.

Ezután meg kell mutatnia a biztonsági tokent (3), és ki kell másolnia (4).

Most nyissa meg a kódrészletkezelő konfigurációs fájlját a következővel:

```bash
# Konfigurálja a kódrészletkezelőt
qc konfigurálása
```

És helyezze a biztonsági tokent a `token` attribútumba:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
A QOwnNotes beállításaiban azt is beállíthatja, hogy milyen jegyzetcímkét használjon a parancsok kereséséhez a jegyzetekben. Alapértelmezés szerint a `commands` címke használatos.
:::

## A parancsrészletek szintaxisa

You can use **unordered lists with in-line code blocks** to store command snippets. Minden `commands` címkét tartalmazó jegyzetben a rendszer parancsrészleteket keres.

If you add a `cmd:` before the in-line code block, the command will also be found in the **current note** regardless of note tags.

```markdown
- `visszhang Én vagyok a parancs` Leírás vagyok #tag1 #tag2 #tag3
* `visszhang Én is parancs vagyok` Leírás vagyok #tag3 #tag4 #tag5
- cmd: `echo I megtalálható lesz az aktuális jegyzetben` Ez a parancs megtalálható az aktuális jegyzetben, függetlenül a jegyzetcímkéktől
```

**`bash` or `shell` code blocks**, preceded by a heading 2 or higher as a description, can also be used for command snippets. Tags are also supported if they are between the heading and the code block.

    ## Do this with a "bash" code block

    - this text will be ignored text
    - but tags can be used: #tag1 #tag2

    ```bash
    echo do this
    echo do that
    ```


    ## Do something else with a "sh" code block

    ```sh
    echo do something else
    echo do something other
    ```

Above example will result in two command snippets, the first one with the two tags `tag1` and `tag2`.

## Használat

```bash
# Search and execute command snippets
qc exec
```

```bash
# Search and print command snippets
qc search
```

## Konfiguráció

Run `qc configure`.

```toml
[General]
  editor = "vim"            # your favorite text editor
  column = 40               # column size for list command
  selectcmd = "fzf"         # selector command for edit command (fzf or peco)
  sortby = ""               # specify how snippets get sorted (recency (default), -recency, description, -description, command, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # your QOwnNotes API token
  websocket_port = 22222    # websocket port in QOwnNotes
```

## Shell completion

You can generate shell completion code for your shell with `qc completion <shell>`.

For example for the Fish shell you can use:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
