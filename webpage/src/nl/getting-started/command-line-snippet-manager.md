# Command-line Snippet Manager

Download the [QOwnNotes Command-line Snippet Manager](https://github.com/qownnotes/qc/releases) to **execute command snippets stored in notes** in QOwnNotes from the command line.

![qc](/img/qc.png)

U kunt **notities met een speciale tag** gebruiken om **command snippets** op te slaan, die u **uit kunt voeren vanuit de command-line snippet manager**.

![commands](/img/commands.png)

## Configuration

![socket-server-token](/img/socket-server-token.png)

Voordat u de snippetmanager gebruikt, moet u de *Websocketserver* (2) inschakelen in de instellingen voor *Browserextensie / opdrachtfragmenten* (1) in QOwnNotes.

Dan moet je het security token (3) laten zien en kopiëren (4).

Open nu het configuratiebestand van de snippetmanager met:

```bash
# Configure the snippet manager
qc configure
```

En plaats het security token in het `token` attribuut:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
In de QOwnNotes-instellingen kunt u ook instellen welke notitietag moet worden gebruikt om naar opdrachten in notities te zoeken. Standaard wordt de tag `opdracht` gebruikt.
:::

## Syntaxis van opdrachtfragmenten

U kunt ongeordende lijsten met in-line codeblokken gebruiken om opdrachtfragmenten op te slaan. Alle notities met de tag `opdracht` worden doorzocht op opdrachtfragmenten.

Als je een `cmd:` toevoegt voor het in-line codeblok, zal de opdracht ook in de huidige notitie worden gevonden, ongeacht notitietags.

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
