# Command-line Snippet Manager

Download the [QOwnNotes Command-line Snippet Manager](https://github.com/qownnotes/qc/releases) to **execute command snippets stored in notes** in QOwnNotes from the command line.

![qc](/img/qc.png)

Sie können **Notizen mit einem speziellen Tag** verwenden, um **Befehls-Snippets** zu speichern, die Sie **über den Befehlszeilen-Snippet-Manager** ausführen können.

![commands](/img/commands.png)

## Aufbau

![socket-server-token](/img/socket-server-token.png)

Bevor Sie den Snippet-Manager verwenden, müssen Sie den *Web-Socket-Server* (2) in den Einstellungen *Browsererweiterung / Befehlsschnipsel* (1) in QOwnNotes aktivieren.

Dann müssen Sie das Sicherheitstoken vorzeigen (3) und kopieren (4).

Öffnen Sie nun die Konfigurationsdatei des Snippet-Managers mit:

```bash
# Configure the snippet manager
qc configure
```

Und fügen Sie das Sicherheitstoken in das Attribut `token` ein:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
In den QOwnNotes-Einstellungen können Sie auch festlegen, welches Notiz-Tag verwendet werden soll, um in Notizen nach Befehlen zu suchen. Standardmäßig wird das Tag `command` verwendet.
:::

## Syntax von Befehlsschnipseln

Sie können ungeordnete Listen mit Inline-Codeblöcken verwenden, um Befehlsausschnitte zu speichern. Alle Notizen mit dem Tag `command` werden nach Befehlsschnipseln durchsucht.

Wenn Sie vor dem Inline-Codeblock ein `cmd:` hinzufügen, wird der Befehl auch in der aktuellen Notiz gefunden, unabhängig von Notiz-Tags.

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
