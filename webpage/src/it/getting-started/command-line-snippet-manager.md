# Command-line Snippet Manager

Download the [QOwnNotes Command-line Snippet Manager](https://github.com/qownnotes/qc/releases) to **execute command snippets stored in notes** in QOwnNotes from the command line.

![qc](/img/qc.png)

You can use **notes with a special tag** to store **command snippets**, which you can **execute from the command-line snippet manager**.

![commands](/img/commands.png)

## Configuration

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
In the QOwnNotes settings you can also set what note tag should be used to search for commands in notes. By default, the tag `command` is used.
:::

## Syntax of command snippets

You can use unordered lists with in-line code blocks to store command snippets. All notes with the tag `command` are searched for command snippets.

If you add a `cmd:` before the in-line code block, the command will also be found in the current note regardless of note tags.

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
