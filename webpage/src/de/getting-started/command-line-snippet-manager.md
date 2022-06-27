# Command-line Snippet Manager

Sie können den [QOwnNotes-Befehlszeilen-Snippet-Manager](https://github.com/qownnotes/qc) verwenden, um **Befehlsschnipsel, die in Notizen** in QOwnNotes gespeichert sind, über die Befehlszeile auszuführen.

![qc](/img/qc.png)

Sie können **Notizen mit einem speziellen Tag** verwenden, um **Befehls-Snippets** zu speichern, die Sie **über den Befehlszeilen-Snippet-Manager** ausführen können.

![commands](/img/commands.png)

## Installation

Besuchen Sie die Seite [Neueste Version](https://github.com/qownnotes/qc/releases/latest) und laden Sie die gewünschte Version herunter.

::: tip
Wenn Sie [jq](https://stedolan.github.io/jq) installiert haben, können Sie dieses Snippet auch verwenden, um beispielsweise das neueste Linux AMD64 AppImage unter `/usr/local/bin/qc` herunterzuladen und zu installieren:

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

## Abhängigkeiten

[fzf](https://github.com/junegunn/fzf) (fuzzy search) or [peco](https://github.com/peco/peco) (älter, aber eher standardmäßig installiert) notwendig zu installieren, wenn nach Befehlen in der Befehlszeile gesucht werden soll.

::: tip
Standardmäßig wird `fzf` für die Suche verwendet, aber Sie können `peco` verwenden, indem Sie es mit `qc configure` einstellen.
:::

## Aufstellen

![socket-server-token](/img/socket-server-token.png)

Bevor Sie den Snippet-Manager verwenden, müssen Sie den *Web-Socket-Server* (2) in den Einstellungen *Browsererweiterung / Befehlsschnipsel* (1) in QOwnNotes aktivieren.

Dann müssen Sie das Sicherheitstoken vorzeigen (3) und kopieren (4).

Öffnen Sie nun die Konfigurationsdatei des Snippet-Managers mit:

```bash
# Konfigurieren Sie den Snippet-Manager
qc configure
```

Und fügen Sie das Sicherheitstoken in das Attribut `token` ein:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
In den QOwnNotes-Einstellungen können Sie auch festlegen, welches Notiz-Tag verwendet werden soll, um in Notizen nach Befehlen zu suchen. Standardmäßig wird das Tag `commands` verwendet.
:::

## Syntax von Befehlsschnipseln

Sie können **ungeordnete Listen mit Inline-Codeblöcken** verwenden, um Befehlsausschnitte zu speichern. Alle Notizen mit dem Tag `commands` werden nach Befehlsschnipseln durchsucht.

Wenn Sie vor dem Inline-Codeblock ein `cmd:` hinzufügen, wird der Befehl auch in der **aktuellen Notiz** gefunden, unabhängig von Notiz-Tags.

```markdown
- `Echo Ich bin ein Befehl` Ich bin eine Beschreibung #tag1 #tag2 #tag3
* `Echo Ich bin auch ein Befehl` Ich bin eine Beschreibung #tag3 #tag4 #tag5
- cmd: `echo I wird in der aktuellen Notiz gefunden` Dieser Befehl wird in der aktuellen Notiz unabhängig von Notiz-Tags gefunden

<!-- Beispiel für das Abfragen von Benutzereingaben -->

- `read -p "PR ID: " id && git holt den Ursprung pull/$id/head:pull-$id && git checkout pull-$id` Fragen Sie nach der Pull-Request-ID und Checkout-Pull-Request
```

**`Bash`- oder `Shell`-Codeblöcke**, denen eine Überschrift 2 oder höher als Beschreibung vorangestellt ist, können auch für Befehlsschnipsel verwendet werden. Tags werden auch unterstützt, wenn sie sich zwischen der Überschrift und dem Codeblock befinden.

    ## Tun Sie dies mit einem "Bash" -Codeblock

    - dieser Text wird ignoriert Text
    - aber Tags können verwendet werden: #tag1 #tag2

    ```bash
    Echo mach das
     Echo mach das
    ```


    ## Mach etwas anderes mit einem "sh" Codeblock

    ```sh
    Echo mach was anderes
     echo mach was anderes
    ```

Das obige Beispiel führt zu zwei Befehlsschnipseln, dem ersten mit den beiden Tags `tag1` und `tag2`.

## Verwendung

```bash
# Befehlsschnipsel suchen und ausführen
qc exec
```

```bash
# Befehlsausschnitte suchen und drucken
qc-Suche
```

## Aufbau

Führen Sie `qc configure` aus.

```toml
[General]
  editor = "vim"            # Ihr Lieblingstexteditor
  column = 40               # Spaltengröße für Listenbefehl
  selectcmd = "fzf"         # Selektorbefehl für Editierbefehl (fzf oder peco)
  sortby = ""               # Geben Sie an, wie Snippets sortiert werden (recency (default), -recency, description, -description, command, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # your QOwnNotes API token
  websocket_port = 22222    # websocket port in QOwnNotes
```

## Shell-Vervollständigung

Sie können Shell-Vervollständigungscode für Ihre Shell mit `qc-Vervollständigung <shell>` generieren.

Für die Fischschale können Sie beispielsweise verwenden:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
