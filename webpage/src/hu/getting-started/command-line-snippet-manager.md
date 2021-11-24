# Command-line Snippet Manager

Download the [QOwnNotes Command-line Snippet Manager](https://github.com/qownnotes/qc/releases) to **execute command snippets stored in notes** in QOwnNotes from the command line.

![qc](/img/qc.png)

**Speciális címkével ellátott megjegyzések** segítségével tárolhatja a **parancsrészleteket**, amelyeket **a parancssori kódrészletkezelőből hajthat végre**.

![commands](/img/commands.png)

## Configuration

![socket-server-token](/img/socket-server-token.png)

A kódrészletkezelő használata előtt engedélyeznie kell a *Web socket szervert* (2) a QOwnNotes *Böngészőbővítmény / parancsrészletek* (1) beállításainál.

Ezután meg kell mutatnia a biztonsági tokent (3), és ki kell másolnia (4).

Most nyissa meg a kódrészletkezelő konfigurációs fájlját a következővel:

```bash
# Configure the snippet manager
qc configure
```

És helyezze a biztonsági tokent a `token` attribútumba:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
A QOwnNotes beállításaiban azt is beállíthatja, hogy milyen jegyzetcímkét használjon a parancsok kereséséhez a jegyzetekben. Alapértelmezés szerint a `command` címke használatos.
:::

## A parancsrészletek szintaxisa

A parancsrészletek tárolásához használhat soron belüli kódblokkokkal rendelkező rendezetlen listákat. Minden `command` címkével rendelkező jegyzetben a rendszer parancsrészleteket keres.

Ha a soron belüli kódblokk elé egy `cmd:` karakterláncot ad hozzá, a parancs a jegyzetcímkéktől függetlenül az aktuális jegyzetben is megtalálható lesz.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3
* `echo I am also a command` I am a description #tag3 #tag4 #tag5
- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags
```

## Usage

```bash
# Search and execute command snippets
qc exec
```

```bash
# Search and print command snippets
qc search
```
