# Markdown

Esta hoja de referencia de Markdown está pensada como una referencia rápida y una muestra de la sintaxis de Markdown en QOwnNotes.

## Encabezados

Usa títulos para estructurar tus textos.

```markdown
# H1
## H2
### H3
#### H4
##### H5
###### H6
```

::: tip
El **panel de navegación** muestra la estructura de sus títulos.
:::

Alternativamente, para H1 y H2, un estilo de subrayado:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
Por defecto, QOwnNotes crea el **nombre de archivo de una nota** desde el **encabezado1** (h1).
:::

## Énfasis

```markdown
Enfatice, también conocido como cursiva, con *asteriscos*.

Fuerte énfasis, también conocido como negrita, con **asteriscos**.
```

::: tip
Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + B</kbd> para poner el texto en negrita y <kbd>Ctrl + I</kbd> para ponerlo en cursiva.
:::

## Subrayar

También hay una configuración opcional para habilitar la representación de subrayado en la *Configuración de vista previa*.

```markdown
_underline_
```

::: tip
Puede utilizar el [atajo](./shortcuts.md) <kbd>Ctrl + U</kbd> para subrayar un texto.
:::

## Tachar

```markdown
~~tachar~~
```

::: tip
Puedes usar el [atajo](./shortcuts.md) <kbd>Alt + Shift + S</kbd> para tachar un texto.
:::

## Listas

Hay muchas formas de crear listas.

```markdown
1. Primer elemento de lista ordenado
2. Otro articulo
   * Sublista desordenada.
1. Los números reales no importan, solo que es un número
   1. Sublista ordenada (solo funciona en el editor, no en la vista previa)
4. Y otro artículo.

* La lista desordenada puede usar asteriscos
- O menos
+ O ventajas
```

::: tip
Si presiona <kbd>Ingrese</kbd> al final de una lista, se creará un nuevo elemento de lista.
:::

## Enlaces

Two of the simpler uses for links are pointing to webpages or other notes. There are multiple ways each of these links can look.

### Enlaces externos

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Plain URLs and URLs in angle brackets will automatically get turned into links in the preview. 
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Enlaces internos

```markdown
[I link to the Journal.md note](Journal.md)

<Journal.md> works similarly.
```

::: tip
Puede utilizar el [atajo](./shortcuts.md) <kbd>Ctrl + L</kbd> para **crear enlaces a páginas web u otras notas**.

Usando <kbd>Ctrl + Shift + X</kbd> aparecerá un diálogo que le ayudará a **inserta archivos adjuntos** en tu nota.

Puede presionar <kbd>Ctrl + Espacio</kbd> mientras el cursor está en un enlace en la edición de notas para seguir el enlace.
:::

### Marcadores

Los marcadores utilizados por la extensión de navegador [QOwnNotes Web Companion](./browser-extension.md) utilizan enlaces en listas.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## Imágenes

Las imágenes se pueden incrustar en QOwnNotes. Ellas se mostrarán en la vista previa.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> para insertar una imagen en una nota. La imagen también puede estar en el portapapeles, el diálogo la detectará y mostrará una vista previa.

También puede pegar directamente una imagen del portapapeles en su nota con <kbd>Ctrl + Shift + V</kbd>.
:::


## Código en línea y bloques de código

```markdown
Inline `code` has `backticks around` it.
```

::: tip
Puede usar el [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> en texto en línea seleccionado o solo dentro del texto para crear un bloque de código en línea.
:::

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

Agregue cuatro espacios delante de su código para marcarlo como bloque de código.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

También puede utilizar tres comillas invertidas para crear un bloque de código.
~~~markdown
```
Code goes here
Code goes here
```
~~~

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
multiple selected lines of text or in an empty line to create a code block. 
:::

### Backtick fence with code highlighting

There also is some syntax highlighting with code blocks in QOwnNotes.

~~~markdown
```bash
# I am a comment
cd Notes
```
~~~

Currently, supported languages (and code block identifiers) are:

* BASh scripting, `bash`
* C, `c`
* C++, `cpp`
* C++, `cxx`
* C++, `c++`
* C#, `c#`
* CMake, `cmake`
* C#, `csharp`
* CSS, `css`
* Go, `go`
* HTML, `html`
* INI, `ini`
* Java, `java`
* JavaScript, `javascript`
* JavaScript, `js`
* JSON, `json`
* Makefile, `make`
* PHP, `php`
* Python, `py`
* Python, `python`
* QML, `qml`
* Rust, `rust`
* Shell scripting, `sh`
* SQL, `sql`
* TypeScript, `ts`
* TypeScript, `typescript`
* V, `v`
* Vex, `vex`
* XML, `xml`
* YAML, `yml`
* YAML, `yaml`

## Tables

Tables aren't part of the core Markdown spec, but the QOwnNotes preview supports them. 

~~~markdown
Colons can be used to align columns.

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

There must be at least 3 dashes separating each header cell.

You can also use inline Markdown.

| Markdown | Less | Pretty |
| --- | --- | --- |
| *Still* | `renders` | **nicely** |
| 1 | 2 | 3 |
~~~

::: tip
Presione <kbd>Alt + Shift + T</kbd> para activar un cuadro de diálogo que puede ayudarlo a crear tablas. Incluso puede importar archivos CSV en ese cuadro de diálogo.

Use <kbd>Ctrl + Espacio</kbd> dentro de una tabla de rebajas para formatearla automáticamente.
:::

## Blockquotes

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can *put* **Markdown** into a blockquote. 
```

::: tip
Puede decirle a QOwnNotes que resalte completamente las citas en bloque o solo el carácter de la cita en bloque en la *Configuración del editor*

Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> para marcar texto como cita en bloque.
:::

## Regla horizontal

Hay tres formas de obtener una regla horizontal: guiones, asteriscos o guiones bajos.

```markdown
Three or more...

Hyphens

---

Asterisks

***

Underscores

___
```

## Saltos de línea

- Puede dividir un párrafo en más de una sola línea para facilitar la edición, aún se representan como un solo párrafo sin interrupciones.
- Puede forzar un salto de línea dentro de un párrafo terminando una línea con dos espacios.
- Puede hacer un párrafo separado delimitándolo con líneas vacías.

::: tip
Puede ingresar dos espacios y una nueva línea con <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>.
:::

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a *separate paragraph*.

This line is also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the *same paragraph*.
```

::: tip
Los espacios finales están resaltados de forma predeterminada en el editor.
:::

## Comentarios

Los comentarios no se muestran en la vista previa.

```markdown
[comment]: # (This comment will not appear in the preview)

<!-- HTML comments are also hidden -->
```

::: tip
Un bloque de comentario html inicial en una nota también se ignorará en la generación automática del nombre de archivo de la nota.
:::

## Listas de casillas de verificación

Puede crear listas de tareas pendientes simples con listas de casillas de verificación.

```markdown
- [x] done
- [ ] todo
```

::: tip
Puede marcar / desmarcar las casillas de verificación en la vista previa.
:::

## Frontmatter

En QOwnNotes puede usar un frontmatter (por ejemplo, YAML) para agregar algo de metainformación adicional. **No se mostrará en la previsualización** ni **perturbará la generación automática de nombres de archivo para las notas**.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

El nombre de archivo de esta nota de ejemplo sería `El título de la nota comienza aquí.md`.
