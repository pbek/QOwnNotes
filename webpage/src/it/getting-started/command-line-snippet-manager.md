# Gestore dei frammenti di comandi

Puoi usare il [Gestore dei frammenti di comandi QOwnNotes](https://github.com/qownnotes/qc) per **eseguire frammenti di comandi memorizzati nelle note** in QOwnNotes tramite la linea di comando.

![qc](/img/qc.png)

Puoi usare **note con etichette speciali** per memorizzare **frammenti di comandi** che puoi **eseguire dal gestore dei frammenti di comandi**.

![commands](/img/commands.png)

## Installazione

Visita la [pagina dell'ultimo rilascio](https://github.com/qownnotes/qc/releases/latest) e scarica la versione di cui hai bisogno.

::: tip
Se hai installato [jq](https://stedolan.github.io/jq) puoi usarlo per scaricare e installare, per esempio l'ultima AppImage di Linux AMD64 in `/usr/local/bin/qc`:

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

## Dipendenze

[fzf](https://github.com/junegunn/fzf) (fuzzy search) or [peco](https://github.com/peco/peco) (older, but more likely to be installed by default) need to be installed to search for commands on the command-line.

::: tip
By default `fzf` is used for searching, but you can use `peco` by setting it with `qc configure`.
:::

## Preparazione

![socket-server-token](/img/socket-server-token.png)

Before you are using the snippet manager you need to enable the *Web socket server* (2) in the *Browser extension / command snippets* (1) settings in QOwnNotes.

Then you need to show the security token (3) and copy it (4).

Now open the configuration file of the snippet manager with:

```bash
# Configure the snippet manager
qc configure
```

And put the security token in the `token` attribute:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
In the QOwnNotes settings you can also set what note tag should be used to search for commands in notes. By default, the tag `commands` is used.
:::

## Syntax of command snippets

You can use **unordered lists with in-line code blocks** to store command snippets. All notes with the tag `commands` are searched for command snippets.

If you add a `cmd:` before the in-line code block, the command will also be found in the **current note** regardless of note tags.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3
* `echo I am also a command` I am a description #tag3 #tag4 #tag5
- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags
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

## Come si usa

```bash
# Cerca ed esegui frammenti di comando
qc exec
```

```bash
# Cerca e visualizza frammenti di comando
qc search
```

## Configurazione

Esegui `qc configure`.

```toml
[General]
  editor = "vim"            # il tuo editor di testo preferito
  column = 40               # numero di colonne per la lista comandi
  selectcmd = "fzf"         # selettore per modificare il comando (fzf o peco)
  sortby = ""               # specifica come i frammenti saranno ordinati (recency (predefinito), -recency, description, -description, command, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # il tuo token API QOwnNotes
  websocket_port = 22222    # porta websocket di QOwnNotes
```

## Completamento console

Puoi generare il codice per il completamento da console per la tua shell con `qc completion <shell>`.

Per esempio, per la Fish shell puoi usare:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
