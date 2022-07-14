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

[fzf](https://github.com/junegunn/fzf) (fuzzy search) or [peco](https://github.com/peco/peco) (più vecchio, ma più probabile che venga installato per impostazione predefinita) deve essere installato per cercare i comandi sulla riga di comando.

::: tip
Per impostazione predefinita viene utilizzato `fzf` per la ricerca, ma puoi utilizzare `peco` impostandolo con `qc configure`.
:::

## Preparazione

![socket-server-token](/img/socket-server-token.png)

Prima di utilizzare lo snippet manager è necessario abilitare il *server Web socket* (2) nelle impostazioni *Estensione del browser / frammenti di comando* (1) in QOwnNotes.

Quindi devi mostrare il token di sicurezza (3) e copiarlo (4).

Ora apri il file di configurazione dello snippet manager con:

```bash
# Configura il gestore degli snippet
qc configure
```

E inserisci il token di sicurezza nell'attributo `token`:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
Nelle impostazioni di QOwnNotes puoi anche impostare quale tag nota deve essere utilizzato per cercare i comandi nelle note. Per impostazione predefinita, viene utilizzato il tag `commands`.
:::

## Sintassi dei frammenti di comando

Puoi utilizzare **elenchi non ordinati con blocchi di codice in linea** per memorizzare frammenti di comando. Tutte le note con il tag `comandi` vengono cercate per frammenti di comando.

Se aggiungi un `cmd:` prima del blocco di codice in linea, il comando si troverà anche nella **nota corrente** indipendentemente dai tag della nota.

```markdown
- `echo sono un comando` sono una descrizione #tag1 #tag2 #tag3
* `echo sono anche un comando` Sono una descrizione #tag3 #tag4 #tag5
- cmd: `echo sarò trovato nella nota corrente` Questo comando sarà trovato nella nota corrente indipendentemente dai tag della nota

<!-- Esempio di richiesta di input da parte dell'utente -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Richiedi l'ID della richiesta pull e la richiesta pull checkout
```

I blocchi di codice **`bash` o `shell`**, preceduti da un'intestazione 2 o superiore come descrizione, possono essere utilizzati anche per frammenti di comando. I tag sono supportati anche se si trovano tra l'intestazione e il blocco di codice.

    ## Fallo con un blocco di codice "bash".

    - questo testo verrà ignorato
    - ma si possono usare i tag: #tag1 #tag2

    ```bash
    eco fai questo
     eco fallo
    ```


    ## Fai qualcos'altro con un blocco di codice "sh".

    ```sh
    eco fare qualcos'altro
     eco fai qualcos'altro
    ```

L'esempio precedente risulterà in due frammenti di comando, il primo con i due tag `tag1` e `tag2`.

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
