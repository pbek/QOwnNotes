# Command-line Snippet Manager

U kunt de [QOwnNotes Command-line Snippet Manager](https://github.com/qownnotes/qc) gebruiken om **opdrachtfragmenten uit te voeren die zijn opgeslagen in notities** in QOwnNotes vanaf de opdrachtregel.

![qc](/img/qc.png)

U kunt **notities met een speciale tag** gebruiken om **command snippets** op te slaan, die u **uit kunt voeren vanuit de command-line snippet manager**.

![commands](/img/commands.png)

## Installatie

Ga naar de [nieuwste release-pagina](https://github.com/qownnotes/qc/releases/latest) en download de versie die je nodig hebt.

::: tip
Als je [jq](https://stedolan.github.io/jq) hebt geïnstalleerd, kun je dit fragment ook gebruiken om bijvoorbeeld de nieuwste Linux AMD64 AppImage te downloaden en te installeren naar `/usr/local/bin/qc`:

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

## Afhankelijkheden

[fzf](https://github.com/junegunn/fzf) (fuzzy zoeken) of [peco](https://github.com/peco/peco) (ouder, maar waarschijnlijker standaard geïnstalleerd) moeten worden geïnstalleerd om naar opdrachten op de opdrachtregel te zoeken.

::: tip
Standaard wordt `fzf` gebruikt om te zoeken, maar u kunt `peco` gebruiken door dit in te stellen met `qc configure`.
:::

## Opstelling

![socket-server-token](/img/socket-server-token.png)

Before you are using the snippet manager you need to enable the _Web socket server_ (2) in the _Browser extension / command snippets_ (1) settings in QOwnNotes.

Dan moet je het security token (3) laten zien en kopiëren (4).

Open nu het configuratiebestand van de snippetmanager met:

```bash
# Configureer de snippetmanager
qc configureren
```

En plaats het security token in het `token` attribuut:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
In de QOwnNotes-instellingen kunt u ook instellen welke notitietag moet worden gebruikt om naar opdrachten in notities te zoeken. Standaard wordt de tag `opdrachten` gebruikt.
:::

## Syntaxis van opdrachtfragmenten

U kunt **ongeordende lijsten met in-line codeblokken** gebruiken om opdrachtfragmenten op te slaan. Alle notities met de tag `opdrachten` worden doorzocht op opdrachtfragmenten.

Als je een `cmd:` toevoegt voor het in-line codeblok, zal het commando ook gevonden worden in de **huidige notitie** ongeacht notitietags.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3

* `echo I am also a command` I am a description #tag3 #tag4 #tag5

- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags

<!-- Example for asking for user input -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

**`bash` of `shell` codeblokken**, voorafgegaan door een kop 2 of hoger als beschrijving, kunnen ook worden gebruikt voor opdrachtfragmenten. Tags worden ook ondersteund als ze tussen de kop en het codeblok staan.

    ## Doe dit met een "bash" codeblok

    - deze tekst wordt genegeerd tekst
    - maar tags kunnen worden gebruikt: #tag1 #tag2

    ```bash
    echo doe dit
     echo doe dat
    ```


    ## Mach etwas anderes mit einem "sh" Codeblock

    ```sh
    echo doe iets anders
     echo doe iets anders
    ```

Bovenstaand voorbeeld resulteert in twee opdrachtfragmenten, de eerste met de twee tags `tag1` en `tag2`.

## Gebruik

```bash
# Zoek en voer opdrachtfragmenten uit
qc exec
```

```bash
# Parancsrészletek keresése és nyomtatása
qc keresés
```

## Configuratie

Run `qc configure`.

```toml
[General]
  editor = "vim"            # je favoriete teksteditor
  column = 40               # kolomgrootte voor lijstopdracht
  selectcmd = "fzf"         # selector commando voor edit commando (fzf of peco)
  sortby = ""               # specificeer hoe fragmenten worden gesorteerd (recentheid (standaard), -recentie, beschrijving, -beschrijving, opdracht, -commando, uitvoer, -uitvoer)

[QOwnNotes]
  token = "MvTagHXF"        # your QOwnNotes API token
  websocket_port = 22222    # websocket port in QOwnNotes
```

## Shell voltooiing

U kunt shell-aanvullingscode voor uw shell genereren met `qc voltooiing <shell>`.

Voor de Fish shell kun je bijvoorbeeld gebruiken:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
