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

Before you are using the snippet manager you need to enable the _Web socket server_ (2) in the _Browser extension / command snippets_ (1) settings in QOwnNotes.

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

Használhat **soron belüli kódblokkokkal rendelkező rendezetlen listákat** a parancsrészletek tárolására. Minden `commands` címkét tartalmazó jegyzetben a rendszer parancsrészleteket keres.

Ha a soron belüli kódblokk elé egy `cmd:` karakterláncot ad hozzá, a parancs az **aktuális jegyzetben** is megtalálható lesz, függetlenül a jegyzetcímkéktől.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3

* `echo I am also a command` I am a description #tag3 #tag4 #tag5

- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags

<!-- Example for asking for user input -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

A **`bash` vagy `shell` kódblokkok**, amelyeket leírásként 2-es vagy magasabb címsor előz meg, szintén használhatók parancsrészletekhez. A címkék is támogatottak, ha a címsor és a kódblokk között vannak.

    ## Ezt egy "bash" kódblokk segítségével teheti meg

    - ezt a szöveget figyelmen kívül hagyja
    - de használhatók a címkék: #tag1 #tag2

    ```bash
    echo csináld ezt
     echo tedd ezt
    ```


    ## Csinálj valami mást az "sh" kódblokk segítségével

    ```sh
    echo csináljon valami mást
     visszhang csinál valami mást
    ```

A fenti példa két parancsrészletet eredményez, az első a két `tag1` és `tag2` címkével.

## Használat

```bash
# Parancsrészletek keresése és végrehajtása
qc exec
```

```bash
# Parancsrészletek keresése és nyomtatása
qc keresés
```

## Konfiguráció

Futtassa a `qc configure` parancsot.

```toml
[General]
  editor = "vim"            # kedvenc szövegszerkesztőd
  column = 40               # oszlopméret a lista parancshoz
  selectcmd = "fzf"         # választó parancs a szerkesztési parancshoz (fzf vagy peco)
  sortby = ""               # adja meg a kódrészletek rendezési módját (recency (alapértelmezett), -recency, description, -description, parancs, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # your QOwnNotes API token
  websocket_port = 22222    # websocket port in QOwnNotes
```

## Shell befejezése

A shell befejezési kódját a `qc befejezés <shell>` paranccsal állíthatja elő.

Például a Fish shell-hez használhatja:

```bash
qc befejezési hal > ~/.config/fish/completions/qc.fish
```
