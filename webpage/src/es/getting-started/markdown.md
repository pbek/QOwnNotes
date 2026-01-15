# Markdown Cheatsheet

Esta hoja de referencia de Markdown está pensada como una referencia rápida y una muestra de la sintaxis de Markdown en QOwnNotes.

## Encabezados

Utilice los encabezados para estructurar sus textos.

```markdown
# Encabezado 1

## Encabezado 2

### Encabezado 3

#### Encabezado 4

##### Encabezado 5

###### Encabezado 6
```

::: tip
El **panel de navegación** muestra la estructura de sus encabezados.
:::

Alternativamente, para H1 y H2, un estilo de subrayado:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
Por defecto, QOwnNotes crea el **nombre de archivo de una nota** a partir del **encabezado 1** (h1).
:::

## Énfasis

```markdown
Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.
```

::: tip
Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + B</kbd> para poner el texto en negrita y <kbd>Ctrl + I</kbd> para ponerlo en cursiva.
:::

## Subrayar

También existe una configuración opcional para habilitar la renderización del subrayado en la _Configuración de la vista previa_.

```markdown
_subrayado_
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
1. Primer elemento de lista ordenada
2. Otro articulo

- Sublista desordenada.

1. Los números reales no importan, solo que sean números
1. And another item.

* Unordered list can use asterisks

- Or minuses

+ Or pluses
```

::: tip
Si presiona <kbd>Enter</kbd> al final de una lista, se creará un nuevo elemento de lista.
:::

## Enlaces

Dos de los usos más simples de los enlaces son apuntar a páginas web u otras notas. Existen múltiples formas en las que cada uno de estos enlaces puede verse.

### Enlaces externos

```markdown
[Soy un enlace en estilo de línea](https://www.google.com)

[Soy un enlace en estilo de línea con un título](https://www.google.com "Página de inicio de Google")

[Puede utilizar números para las definiciones de los enlaces en el estilo de referencia][1]

Las URL y URL entre paréntesis entre corchetes angulares se convertirán automáticamente en enlaces en la vista previa.
http://www.example.com o <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Enlaces internos

Puede enlazar a otras notas o archivos adjuntos de esta forma:

```markdown
[Yo enlazo a la nota Diario.md](Diario.md)

<Journal.md> funciona de forma similar.
```

::: tip
Puede utilizar el [atajo](./shortcuts.md) <kbd>Ctrl + L</kbd> para **crear enlaces a páginas web u otras notas**.

<kbd>Ctrl + Shift + X</kbd> abrirá un diálogo que le ayudará a **inserta archivos adjuntos** en su nota.

Puede presionar <kbd>Ctrl + Espacio</kbd> mientras el cursor está en un enlace en la edición de notas para abrir el enlace.
:::

::: warning
Información No se admiten enlaces internos con un título:

```markdown
[Yo enlazo a la nota de Diario.md](Diario.md "Título no soportado")
```

:::

### Marcadores

Los marcadores utilizados por la extensión de navegador [QOwnNotes Web Companion](./browser-extension.md) utilizan enlaces en listas.

```markdown
- [Nombre de la página web](https://www.ejemplo.com) #etiqueta1 #etiqueta2 alguna descripción y etiquetas
```

## Imágenes

Las imágenes se pueden incrustar en QOwnNotes. Estas se mostrarán en la vista previa.

```markdown
![texto alternativo](media/mi-imagen.jpg)
```

::: tip
Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> para insertar una imagen en una nota. La imagen también puede estar en el portapapeles, el diálogo la detectará y mostrará una vista previa.

También puede pegar directamente una imagen del portapapeles en su nota con <kbd>Ctrl + Shift + V</kbd>.
:::

::: warning
Info No se admiten enlaces de imágenes con un título:

```markdown
![texto alternativo](multimedia/mi-imagen.jpg "Título no soportado")
```

:::

## Código en línea y bloques de código

```markdown
El `código` en línea tiene `comillas invertidas` a su alrededor.
```

::: tip
Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> en texto en línea seleccionado o solo dentro del texto para crear un bloque de código en línea.
:::

Los bloques de código están delimitados por líneas con tres comillas invertidas o indentados con cuatro espacios.

### Valla de 4 espacios

Agregue cuatro espacios delante de su código para marcarlo como bloque de código.

```markdown
    s = "Código con identación de espacio"
    print s
```

### Valla de comillas invertidas

También puede utilizar tres comillas invertidas para crear un bloque de código.
````markdown
```
El código va aquí
El código va aquí
```
````

::: tip
Puede usar el [atajo] (./ shortcuts.md) <kbd>Ctrl + Shift + C</kbd> en
varias líneas de texto seleccionadas o en una línea vacía para crear un bloque de código.
:::

### Valla de comillas invertidas con resaltado de código

También hay algo de resaltado de sintaxis con bloques de código en QOwnNotes.

````markdown
```bash
# Soy un comentario
cd Notas
````
````

Actualmente, los idiomas admitidos (e identificadores de bloque de código) son:

- BASh scripting, `bash`
- C, `c`
- C++, `cpp`
- C++, `cxx`
- C++, `c++`
- C#, `c#`
- CMake, `cmake`
- C#, `csharp`
- CSS, `css`
- Go, `go`
- HTML, `html`
- INI, `ini`
- Java, `java`
- JavaScript, `javascript`
- JavaScript, `js`
- JSON, `json`
- Makefile, `make`
- PHP, `php`
- Python, `py`
- Python, `python`
- QML, `qml`
- Rust, `rust`
- Shell scripting, `sh`
- SQL, `sql`
- TypeScript, `ts`
- TypeScript, `typescript`
- V, `v`
- Vex, `vex`
- XML, `xml`
- YAML, `yml`
- YAML, `yaml`

## Tablas

Las tablas no forman parte de la especificación principal de Markdown, pero la vista previa de QOwnNotes las admite.

~~~markdown
Se pueden usar virguillas para alinear columnas.

| Las        |      Tablas      |  Molan |
| ------------- | :-----------: | ----: |
| la col 3 está      | alineada a la derecha | $1600 |
| la col 2 is      |   centrada    |   $12 |
| las plecas |   son elegantes    |    $1 |

Debe haber al menos 3 guiones separando cada celda de encabezado.

También puede utilizar Markdown en línea.

| Markdown | Menos      | Chulo     |
| -------- | --------- | ---------- |
| _Aun_  | `se renderiza` | **bellamente** |
| 1        | 2         | 3          |
````

::: tip
Presione <kbd>Alt + Shift + T</kbd> para activar un cuadro de diálogo que puede ayudarlo a crear tablas. Incluso puede importar archivos CSV en ese cuadro de diálogo.

Use <kbd>Ctrl + Espacio</kbd> dentro de una tabla de markdown para formatearla automáticamente.
:::

## Citas en bloque

```markdown
> Las citas en bloque son muy útiles en los correos electrónicos para emular el texto de respuesta.
> Esta línea es parte de la misma cita.

Salto de cita.

> Esta es una línea muy larga que aún se citará adecuadamente cuando la cita termine. Vaya, sigamos escribiendo para asegurarnos de que esto sea lo suficientemente largo como para envolver todo. Oh, puede _poner_ **Markdown** en una cita en bloque.
```

::: tip
Puede decirle a QOwnNotes que resalte completamente las citas en bloque o solo el carácter de la cita en bloque en la _Configuración del editor_

Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> para marcar texto como cita en bloque.
:::

## Regla horizontal

Hay tres formas de crear una regla horizontal: guiones, asteriscos o guiones bajos.

```markdown
Tres o más...

Hyphens

---

Asterisks

***

Underscores

___
```

## Saltos de línea

- Puede dividir un párrafo en más de una sola línea para facilitar la edición. Las líneas aún se renderizan como un solo párrafo sin interrupciones.
- Puede forzar un salto de línea dentro de un párrafo terminando una línea con dos espacios.
- Puede hacer un párrafo separado delimitándolo con líneas vacías.

::: tip
Puede ingresar dos espacios y una nueva línea con <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>.
:::

```markdown
Aquí hay una línea con la cual podemos comenzar.

Esta línea está separada de la anterior por dos líneas nuevas, por lo que será un _párrafo separado_.

This line also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

::: tip
Los espacios finales están resaltados de forma predeterminada en el editor.
:::

## Comentarios

Los comentarios no se muestran en la vista previa.

```markdown
[comentario]: # "Este comentario no aparecerá en la vista previa"

<!-- Los comentarios de HTML también están ocultos -->
```

::: tip
Un bloque de comentario html inicial en una nota también se ignorará en la generación automática del nombre de archivo de la nota.
:::

## Listas de casillas de verificación

Puede crear listas de tareas pendientes simples con listas de casillas de verificación.

```markdown
- [x] hecho
- [ ] por hacer
```

::: tip
Puede marcar / desmarcar las casillas de verificación en la vista previa.
:::

## Frontmatter

En QOwnNotes puede usar un frontmatter (por ejemplo, YAML) para agregar algo de metainformación adicional. **No se mostrará en la previsualización** ni **perturbará la generación automática de nombres de archivo para las notas**.

```markdown
---
title: Un título
description: Alguna descripción
---

# El titular de la nota comienza aquí

Algún texto
```

El nombre de archivo de esta nota de ejemplo sería `El título de la nota comienza aquí.md`.
