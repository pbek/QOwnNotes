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

::: tip
Si tiene problemas con la instalación de QOwnNotes, es posible que desee iniciar la aplicación con una configuración nueva sin perder la configuración actual utilizando el parámetro `--session`.

```bash
QOwnNotes --session test
```
:::

Puede ejecutar la aplicación en la interfaz de línea de comandos de manera diferente en diferentes sistemas operativos:

| Sistema Operativo | Comando                                                |
| ----------------- | ------------------------------------------------------ |
| Linux             | `QOwnNotes` (or `qownnotes` if installed as snap)      |
| macOS             | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes` |
| Windows           | `QOwnNotes.exe`                                        |
