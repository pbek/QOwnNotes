# Parámetros de la interfaz de línea de comandos

Puede utilizar estos parámetros en la interfaz de línea de comandos:

| Parámetro                    | Descripción                                                                                                                               |
| ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Muestra la pantalla de ayuda                                                                                                              |
| `--portable`                 | Ejecuta la aplicación en modo portable                                                                                                    |
| `--clear-settings`           | Borra los ajustes y ejecuta la aplicación                                                                                                 |
| `--dump-settings`            | Imprime un volcado de la configuración y otra información sobre la aplicación y el entorno en markdown de GitHub, y sale de la aplicación |
| `--session <name>`     | Ejecuta la aplicación en un contexto diferente para la configuración y archivos internos                                                  |
| `--allow-multiple-instances` | Permite iniciar varias instancias de QOwnNotes aunque no esté permitido en la configuración                                               |
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                                      |

::: tip
If you run into troubles with your installation of QOwnNotes you may want start the application with fresh settings without losing your current settings by using the `--session` parameter.

```bash
QOwnNotes --session test
```
:::

Puede ejecutar la aplicación en la interfaz de línea de comandos de manera diferente en diferentes sistemas operativos:

| Sistema Operativo | Comando                                                    |
| ----------------- | ---------------------------------------------------------- |
| Linux             | `QOwnNotes` (o `qownnotes` si se instala como complemento) |
| macOS             | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`     |
| Windows           | `QOwnNotes.exe`                                            |

## Trigger menu actions after startup

With the parameter `--action <name>` you can trigger menu actions after the application was started.

For example to open the todo list dialog after startup use:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Just search for the English menu title. Note that these texts can change over time.
:::

To trigger [scripting actions](../scripting/methods-and-objects.md#registering-a-custom-action) use `customAction_` followed by the identifier of the custom action. The identifier of the custom action is the first parameter of the `script.registerCustomAction` call in the script.

For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
If you run QOwnNotes a second time with the action parameter and only have allowed one application instance, then the menu action will be triggered in the first instance.
:::
