# Parámetros de la interfaz de las órdenes de terminal

Puede utilizar estos parámetros en la interfaz de las órdenes de terminal (CLI) para controlar el comportamiento de la aplicación:

| Parámetro                    | Descripción                                                                                                                              |
| ---------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Muestra la pantalla de ayuda                                                                                                             |
| `--version`                  | Imprime el número de versión de la aplicación                                                                                            |
| `--portable`                 | Ejecuta la aplicación en modo portable                                                                                                   |
| `--clear-settings`           | Elimina los ajustes y ejecuta la aplicación                                                                                              |
| `--dump-settings`            | Imprime un volcado de la configuración y otra información sobre la aplicación y el entorno de GitHub en markdown y sale de la aplicación |
| `--session <name>`     | Ejecuta la aplicación en un contexto diferente para la configuración y archivos internos                                                 |
| `--allow-multiple-instances` | Permite iniciar varias instancias de QOwnNotes, incluso si no está permitido en la configuración                                         |
| `--action <name>`      | Activa una acción de menú después de que se inicie la aplicación (vea abajo)                                                             |
| `--completion <shell>` | Genera un código de finalización de consola. Soporta `fish` y `bash`.                                                                    |

::: tip
Si tiene problemas con la instalación de QOwnNotes, es posible que desee iniciar la aplicación con una configuración nueva sin perder la configuración actual mediante el parámetro `--session`.

```bash
QOwnNotes --session test
```

:::

Puede ejecutar la aplicación en la interfaz de órdenes de terminal de manera diferente en diferentes sistemas operativos:

| Sistema Operativo | Orden                                                  |
| ----------------- | ------------------------------------------------------ |
| Linux             | `QOwnNotes` (o `qownnotes` si se instaló como snap)    |
| macOS             | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes` |
| Windows           | `QOwnNotes.exe`                                        |

::: tip
Para copiar directamente el volcado de configuración en el portapapeles para luego publicarlo en una [incidencia de GitHub](https://github.com/pbek/QOwnNotes/issues), puede utilizar `xclip` o `xsel` en Linux, `pbcopy` en macOS y `clip` en Windows:

```bash
# Copiar el volcado de configuración en el portapapeles con xclip en Linux
QOwnNotes --dump-settings | xclip -selection clipboard

# Copiar el volcado de configuración en el portapapeles con xsel en Linux
QOwnNotes --dump-settings | xsel --clipboard

# Copiar el volcado de configuración en el portapapeles con pbcopy en macOS
QOwnNotes --dump-settings | pbcopy

# Copiar el volcado de configuración en el portapapeles con clip en LinuxWindows
QOwnNotes --dump-settings | clip
```

:::

## Activar acciones del menú después del inicio

Con el parámetro ` --action <name> ` puede activar acciones de menú después de que se inicie la aplicación.

Por ejemplo, para abrir el cuadro de diálogo de la lista de tareas pendientes después del inicio utilice:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Puede obtener los nombres de objeto de la acción del menú desde [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Simplemente busque el título del menú en inglés. Tenga en cuenta que estos textos pueden cambiar con el tiempo.
:::

Para activar [acciones de secuencia de órdenes](../scripting/methods-and-objects.md#registering-a-custom-action), utilice `customAction_` seguido del identificador de la acción personalizada. El identificador de la acción personalizada es el primer parámetro de la llamada `script.registerCustomAction` en el script.

Por ejemplo, para ejecutar la acción personalizada `myAction`, inicie QOwnNotes así:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Si ejecuta QOwnNotes por segunda vez con el parámetro de acción y solo ha permitido una instancia de aplicación, la acción del menú se activará en la primera instancia.
:::
