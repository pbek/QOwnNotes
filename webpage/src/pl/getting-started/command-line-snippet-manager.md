# Command-line Snippet Manager

Możesz użyć [Menedżera fragmentów wiersza polecenia QOwnNotes](https://github.com/qownnotes/qc) do **wykonywania fragmentów poleceń zapisanych w notatkach** w QOwnNotes z wiersza poleceń.

![qc](/img/qc.png)

Możesz używać **notatek ze specjalnym tagiem** do przechowywania **fragmentów poleceń**, które możesz **wykonywać z poziomu menedżera fragmentów wiersza poleceń**.

![commands](/img/commands.png)

## Instalacja

Odwiedź [latest release page](https://github.com/qownnotes/qc/releases/latest) i pobierz potrzebną wersję.

::: tip
Jeśli masz zainstalowane [jq](https://stedolan.github.io/jq), możesz również użyć tego fragmentu kodu, aby pobrać i zainstalować na przykład najnowszą aplikację Linux AMD64 AppImage do `/usr/local/bin/qc`:

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

## Zależności

[fzf](https://github.com/junegunn/fzf) (wyszukiwanie rozmyte) lub [peco](https://github.com/peco/peco) (starsze, ale z większym prawdopodobieństwem zostaną zainstalowane domyślnie) muszą być zainstalowane, aby wyszukiwać polecenia w wierszu poleceń.

::: tip
Domyślnie do wyszukiwania używane jest `fzf`, ale możesz użyć `peco`, ustawiając go za pomocą `qc configure`.
:::

## Organizować coś

![socket-server-token](/img/socket-server-token.png)

Before you are using the snippet manager you need to enable the _Web socket server_ (2) in the _Browser extension / command snippets_ (1) settings in QOwnNotes.

Następnie musisz pokazać token bezpieczeństwa (3) i skopiować go (4).

Teraz otwórz plik konfiguracyjny menedżera snippetów za pomocą:

```bash
# Skonfiguruj menedżera fragmentów
qc configure
```

I umieść token bezpieczeństwa w atrybucie `token`:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
W ustawieniach QOwnNotes możesz również ustawić, jaki znacznik notatki ma być używany do wyszukiwania poleceń w notatkach. Domyślnie używany jest tag `polecenia`.
:::

## Składnia fragmentów poleceń

Możesz używać **list nieuporządkowanych z wbudowanymi blokami kodu** do przechowywania fragmentów poleceń. Wszystkie notatki z tagiem `polecenia` są przeszukiwane pod kątem fragmentów poleceń.

Jeśli dodasz `cmd:` przed blokiem kodu w wierszu, polecenie zostanie również znalezione w **bieżącej notatce**, niezależnie od tagów notatek.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3

* `echo I am also a command` I am a description #tag3 #tag4 #tag5

- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags

<!-- Example for asking for user input -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

**`bash` or `shell` code blocks**, poprzedzone nagłówkiem 2 lub wyższym jako opis, mogą być również używane we fragmentach poleceń. Tagi są również obsługiwane, jeśli znajdują się między nagłówkiem a blokiem kodu.

    ## Zrób to z blokiem kodu "bash"

    - ten tekst zostanie zignorowany
    - ale można używać tagów: #tag1 #tag2

    ```bash
    echo zrób to
     echo zrób to
    ```


    ## Zrób coś innego z blokiem kodu „sh”

    ```sh
    echo zrób coś innego
     echo zrób coś innego
    ```

Powyższy przykład da w wyniku dwa fragmenty poleceń, pierwszy z dwoma tagami `tag1` i `tag2`.

## Stosowanie

```bash
# Wyszukaj i wykonaj fragmenty poleceń
qc exec
```

```bash
# Wyszukaj i wydrukuj fragmenty poleceń
wyszukiwanie qc
```

## Konfiguracja

Uruchom `konfigurację qc`.

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

## Zakończenie powłoki

Możesz wygenerować kod uzupełniania powłoki dla swojej powłoki za pomocą `uzupełniania qc <shell>`.

Na przykład do muszli rybnej możesz użyć:

```bash
qc zakończenie ryba > ~/.config/fish/completions/qc.fish
```
