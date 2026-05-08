# Gestor de fragmentos de comandos de terminal

Puede utilizar el [Gestor de fragmentos de comandos de terminal de QOwnNotes](https://github.com/qownnotes/qc) para **ejecutar comandos de órdenes almacenados en notas** en QOwnNotes desde los comandos de terminal.

![qc](/img/qc.png)

Puede utilizar **notas con una etiqueta especial** para almacenar **fragmentos de comandos**, que puede **ejecutar desde el gestor de fragmentos de comandos de terminal**.

![commands](/img/commands.png)

## Instalación

Visite la [página del último lanzamiento](https://github.com/qownnotes/qc/releases/latest) y descargue la versión que necesite.

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

Se necesita instalar [fzf](https://github.com/junegunn/fzf) (fuzzy search) o [peco](https://github.com/peco/peco) (más antiguo, pero más probable de estar instalado de forma predeterminada) para buscar comandos en las órdenes de terminal.

::: tip
De forma predeterminada, `fzf` se usa para buscar, pero puede usar `peco` configurándolo con `qc configure`.
:::

## Configuración

![socket-server-token](/img/socket-server-token.png)

Antes de usar el gestor de fragmentos, debe habilitar el _servidor de zócalo web_ (2) en la configuración de _extensión del navegador/fragmentos de comandos_ (1) en QOwnNotes.

Luego, debe mostrar la ficha de seguridad (3) y copiarla (4).

Ahora abra el archivo de configuración del gestor de fragmentos con:

```bash
# Configurar el gestor de fragmentos
qc configure
```

Y coloque la ficha de seguridad en el atributo `token`:

```toml
[QOwnNotes]
token = "su_ficha_aquí"
```

::: tip
En la configuración de QOwnNotes, también puede establecer qué etiqueta de nota se debe usar para buscar comandos en las notas. De forma predeterminada, se utiliza la etiqueta `commands`.
:::

## Sintaxis de fragmentos de comandos

Puede usar **listas desordenadas con bloques de código en terminal** para almacenar fragmentos de comandos. Todas las notas con la etiqueta `commands` se buscan para encontrar fragmentos de comandos.

Si agrega un `cmd:` antes del bloque de código en terminal, el comando también se encontrará en la **nota actual** independientemente de las etiquetas de la nota.

```markdown
- `echo Soy un comando` Soy una descripción #etiqueta1 #etiqueta2 #etiqueta3

* `echo Tambien soy un comando` Soy una descripcion #etiqueta3 #etiqueta4 #etiqueta5

- cmd: `echo Seré encontrado en la nota actual` Este comando se encontrará en la nota actual independientemente de las etiquetas de nota

<!-- Ejemplo para solicitar la entrada del usuario -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Preguntar por el ID de la incorporación de cambios (pull request) y realizar checkout a la incorporación de cambios
```

Los bloques de código de **`bash` o `shell`**, precedidos por un encabezado 2 o superior como descripción, también pueden ser usados para fragmentos de comandos. Las etiquetas también son compatibles si se encuentran entre el encabezado y el bloque de código.

    ## Haz esto con un bloque de código "bash"

    - este texto será ignorado
    - pero se pueden usar etiquetas: #etiqueta1 #etiqueta2

    ```bash
    echo haz esto
     echo haz eso
    ```


    ## Realizar otra cosa con un bloque de código "sh"

    ```sh
    echo haz otra cosa
     echo haz algo más
    ```

El ejemplo anterior dará como resultado dos fragmentos de comandos, el primero con las dos etiquetas `etiqueta1` y `etiqueta2`.

## Uso

```bash
# Buscar y ejecutar fragmentos de comandos
qc exec
```

```bash
# Buscar e imprimir fragmentos de comandos
qc search
```

## Configuración

Ejecute `qc configure`.

```toml
[General]
  editor = "vim"            # su editor de texto favorito
  column = 40               # tamaño de columna para comandos de lista
  selectcmd = "fzf"         # orden de selector para orden de edición (fzf o peco)
  sortby = ""               # especifica cómo se ordenan los fragmentos (más reciente (predeterminado), -recency, descripción, -description, orden, -command, salida, -output)

[QOwnNotes]
  token = "MvTagHXF"        # su ficha de la API de QOwnNotes
  websocket_port = 22222    # puerto del zócalo web en QOwnNotes
```

## Finalización de la consola

Puede generar un código de finalización de su consola con la `qc completion <shell>`.

Por ejemplo, para la consola Fish puede usar:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
