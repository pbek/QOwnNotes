# Command line interface parameters

You can use these parameters on the command line interface:

| Parameter                    | Description                                                                                                                                |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| `--help`                     | Shows the help screen                                                                                                                      |
| `--portable`                 | Runs the application in portable mode                                                                                                      |
| `--clear-settings`           | Clears the settings and runs the application                                                                                               |
| `--dump-settings`            | Prints out a dump of the settings and other information about the application and environment in GitHub Markdown and exits the application |
| `--session <name>`           | Runs the application in a different context for settings and internal files                                                                |
| `--allow-multiple-instances` | Allows multiple instances of QOwnNotes to be started even if disallowed in the settings                                                    |

::: tip
If you run into troubles with your installation of QOwnNotes you may want start
the application with fresh settings without loosing your current settings by using
the `--session` parameter.

```bash
QOwnNotes --session test
```
:::

You may run the application on the command line interface differently on
different operating systems:

| Operating system | Command                                                |
| ---------------- | ------------------------------------------------------ |
| Linux            | `QOwnNotes` (or `qownnotes` if installed as snap)      |
| macOS            | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes` |
| Windows          | `QOwnNotes.exe`                                        |
