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
| `--action <name>`            | Triggers a menu action after the application was started (see below)                                                                       |

::: tip
If you run into troubles with your installation of QOwnNotes you may want start
the application with fresh settings without losing your current settings by using
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

## Trigger menu actions after startup

With the parameter `--action <name>` you can trigger menu actions after the application was started.

For example to open the todo list dialog after startup use:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
You can get the object names of the menu action from
[mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).
Just search for the English menu title. Note that these texts can change over time.
:::

To trigger [scripting actions](../scripting/methods-and-objects.md#registering-a-custom-action)
use `customAction_` followed by the identifier of the custom action.
The identifier of the custom action is the first parameter of the `script.registerCustomAction`
call in the script.

For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
If you run QOwnNotes a second time with the action parameter and only have allowed
one application instance, then the menu action will be triggered in the first instance.
:::
