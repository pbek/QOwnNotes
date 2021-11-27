# Parámetros de la interfaz de línea de comandos

Puede utilizar estos parámetros en la interfaz de línea de comandos:

| Parámetro                    | Descripción                                                                                                                                |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| `--help`                     | Muestra la pantalla de ayuda                                                                                                               |
| `--version`                  | Prints out the version number of the application                                                                                           |
| `--portable`                 | Runs the application in portable mode                                                                                                      |
| `--clear-settings`           | Clears the settings and runs the application                                                                                               |
| `--dump-settings`            | Prints out a dump of the settings and other information about the application and environment in GitHub Markdown and exits the application |
| `--session <name>`     | Runs the application in a different context for settings and internal files                                                                |
| `--allow-multiple-instances` | Allows multiple instances of QOwnNotes to be started even if disallowed in the settings                                                    |
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                                       |

::: tip
Si tiene problemas con la instalación de QOwnNotes, es posible que desee iniciar la aplicación con una configuración nueva sin perder la configuración actual mediante el parámetro `--session`.

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

## Activar acciones del menú después del inicio

Con el parámetro ` --action <name> ` puede activar acciones de menú después de que se inicie la aplicación.

Por ejemplo, para abrir el cuadro de diálogo de la lista de tareas pendientes después del uso de inicio:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Puede obtener los nombres de los objetos de la acción del menú en [ventana principal.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Simplemente busque el título del menú en inglés. Tenga en cuenta que estos textos pueden cambiar con el tiempo.
:::

Para activar [acciones de secuencia de comandos](../scripting/methods-and-objects.md#registering-a-custom-action), utilice `customAction_` seguido del identificador de la acción personalizada. El identificador de la acción personalizada es el primer parámetro de la llamada `script.registerCustomAction` en el script.

Por ejemplo, para ejecutar la acción personalizada `myAction`, inicie QOwnNotes así:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Si ejecuta QOwnNotes por segunda vez con el parámetro de acción y solo ha permitido una instancia de aplicación, la acción del menú se activará en la primera instancia.
:::
