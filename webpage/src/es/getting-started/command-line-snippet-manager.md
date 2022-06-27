# Gestor de fragmentos de órdenes de terminal

Puede utilizar el [Administrador de fragmentos de la línea de comandos de QOwnNotes](https://github.com/qownnotes/qc) para **ejecutar fragmentos de comandos almacenados en notas** en QOwnNotes desde la línea de comandos.

![qc](/img/qc.png)

Puede utilizar **notas con una etiqueta especial** para almacenar **fragmentos de código**, que puede **ejecutar desde el administrador de fragmentos de línea de comandos**.

![commands](/img/commands.png)

## Instalación

Visite la [página de la última versión](https://github.com/qownnotes/qc/releases/latest) y descargue la versión que necesita.

::: tip
Si tiene [jq](https://stedolan.github.io/jq) instalado, también puede usar este fragmento para descargar e instalar, por ejemplo, la última AppImage de Linux AMD64 en `/usr/local/bin/qc`:

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

## Dependencias

[fzf](https://github.com/junegunn/fzf) (fuzzy search) or [peco](https://github.com/peco/peco) (más antiguo, pero es más probable que se instale de forma predeterminada) debe instalarse para buscar comandos en la línea de comandos.

::: tip
De forma predeterminada, `fzf` se usa para buscar, pero puede usar `peco` configurándolo con `qc configure`.
:::

## Configuración

![socket-server-token](/img/socket-server-token.png)

Antes de usar el administrador de fragmentos, debe habilitar el *servidor de socket web* (2) en la configuración de *fragmentos de comando/extensión del navegador* (1) en QOwnNotes.

Luego, debe mostrar el token de seguridad (3) y copiarlo (4).

Ahora abra el archivo de configuración del administrador de fragmentos con:

```bash
# Configurar el administrador de fragmentos
control de calidad configurar
```

Y coloque el token de seguridad en el atributo `token`:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
En la configuración de QOwnNotes, también puede establecer qué etiqueta de nota se debe usar para buscar comandos en las notas. De forma predeterminada, se utiliza la etiqueta `comandos`.
:::

## Sintaxis de fragmentos de comando

Puede usar **listas desordenadas con bloques de código en línea** para almacenar fragmentos de comandos. Todas las notas con la etiqueta `comandos` se buscan fragmentos de comandos.

Si agrega un `cmd:` antes del bloque de código en línea, el comando también se encontrará en la **nota actual** independientemente de las etiquetas de nota.

```markdown
- `echo Soy un comando` Soy una descripción #tag1 #tag2 #tag3
* `echo yo tambien soy un comando` yo soy una descripcion #tag3 #tag4 #tag5
- cmd: `echo I se encontrará en la nota actual` Este comando se encontrará en la nota actual independientemente de las etiquetas de nota

<!-- Ejemplo para solicitar la entrada del usuario -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

Los bloques de código **`bash` o `shell`**, precedidos por un encabezado 2 o superior como descripción, también se pueden usar para fragmentos de comando. Las etiquetas también son compatibles si se encuentran entre el encabezado y el bloque de código.

    ## Haz esto con un bloque de código "bash"

    - este texto será ignorado texto
    - pero se pueden usar etiquetas: #tag1 #tag2

    ```bash
    eco haz esto
     echo haz eso
    ```


    ## Haz otra cosa con un bloque de código "sh"

    ```sh
    eco haz otra cosa
     echo hacer otra cosa
    ```

El ejemplo anterior dará como resultado dos fragmentos de comando, el primero con los dos etiquetas `etiqueta1` y `etiqueta2`.

## Uso

```bash
# Buscar y ejecutar fragmentos de comandos
ejecutivo de control de calidad
```

```bash
# Buscar ejecutar y fragmentos de comandos ejecutivos de control de calidad
```

## Configuración

Ejecute `qc configure`.

```toml
[General]
  editor = "vim"            # your favorite text editor
  column = 40               # column size for list command
  selectcmd = "fzf"         # selector command for edit command (fzf or peco)
  sortby = ""               # specify how snippets get sorted (recency (default), -recency, description, -description, command, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # your QOwnNotes API token
  websocket_port = 22222    # websocket port in QOwnNotes
```

## Terminación de caparazón

Puede generar un código de finalización de shell para su shell con `completado de qc <shell>`.

Por ejemplo, para el caparazón de pescado puedes usar:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
