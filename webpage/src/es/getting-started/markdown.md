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
Énfasis, también conocido como cursiva, con *asteriscos*.

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
~~strike out~~
```

::: tip
Puedes usar el [atajo](./shortcuts.md) <kbd>Alt + Shift + S</kbd> para tachar un texto.
:::

## Liza

Hay muchas formas de crear listas.

```markdown
1. Primer elemento de lista ordenado
2. Otro articulo
   * Sublista desordenada.
1. Los números reales no importan, solo que es un número
   1. Sublista ordenada
4. Y otro artículo.

* La lista desordenada puede usar asteriscos
- O menos
+ O ventajas
```

::: tip
Si presiona <kbd>Ingrese</kbd> al final de una lista, se creará un nuevo elemento de lista.
:::

## Links

Hay muchas formas de crear listas.

```markdown
[Soy un enlace de estilo en línea] (https://www.google.com)

[Soy un enlace de estilo en línea con título] (https://www.google.com "Página principal de Google")

[Puede utilizar números para las definiciones de enlaces de estilo de referencia] [1]

Las URL y las URL entre paréntesis angulares se convertirán automáticamente en enlaces en la vista previa. 
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

::: tip
Puede utilizar el [atajo](./shortcuts.md) <kbd>Ctrl + L</kbd> para **crear enlaces a páginas web u otras notas**.

Usando <kbd>Ctrl + Shift + X</kbd> aparecerá un diálogo que le ayudará a **inserta archivos adjuntos** en tu nota.

Puede presionar <kbd>Ctrl + Espacio</kbd> mientras el cursor está en un enlace en la edición de notas para seguir el enlace.
:::

### Bookmarks

Los marcadores utilizados por la extensión de navegador [QOwnNotes Web Companion](./browser-extension.md) utilizan enlaces en listas.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 alguna descripción y etiquetas
```

## Images

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
Inline `code` has `back-ticks around` it.
```

::: tip
Puede usar el [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> en texto en línea seleccionado o solo dentro del texto para crear un bloque de código en línea.
:::

Blocks of code are either fenced by lines with three back-ticks, or are indented with four spaces.

### 4-Spaces fence

Agregue cuatro espacios delante de su código para marcarlo como bloque de código.

```markdown
    s = "Código con sangría de espacio"
    print s
```

### Valla de tachuela

También puede utilizar tres comillas invertidas para crear un bloque de código.
~~~markdown
```
El código va aquí
El código va aquí
```
~~~

::: tip
Puede usar el [atajo] (./ shortcuts.md) <kbd>Ctrl + Shift + C</kbd>  en
varias líneas de texto seleccionadas o en una línea vacía para crear un bloque de código. 
:::

### Cerca de backtick con resaltado de código

También hay algo de resaltado de sintaxis con bloques de código en QOwnNotes.

~~~ rebaja
bash
# Soy un comentario
cd notas
~~~
~~~

## Mesas

Las tablas no forman parte de la especificación principal de Markdown, pero la vista previa de QOwnNotes las admite. 

`` rebaja
Se pueden utilizar dos puntos para alinear columnas.

| Tablas | Son | Cool |
| ------------- |: -------------: | -----: |
| col 3 es | alineado a la derecha | $ 1600 |
| col 2 es | centrado | $ 12 |
| rayas de cebra | están ordenados | $ 1 |

Debe haber al menos 3 guiones que separen cada celda de encabezado.

También puede utilizar Markdown en línea.

| Markdown | Menos | Pretty |
| --- | --- | --- |
| * Aún * | `renders` | ** muy bien ** |
| 1 | 2 | 3 |
~~~

::: tip
Presione <kbd>Alt + Mayús + T</kbd> para activar un diálogo que puede ayudarlo a crear tablas. Incluso puede importar archivos CSV en ese cuadro de diálogo.

Use <kbd>Ctrl + Espacio</kbd> dentro de una tabla de rebajas para formatearla automáticamente.
:::

## Blockquotes

```markdown
> Las citas en bloque son muy útiles en el correo electrónico para emular el texto de respuesta.
> Esta línea es parte de la misma cita.

Descanso de cotización.

> Esta es una línea muy larga que se cotizará correctamente cuando termine. Vaya, sigamos escribiendo para asegurarnos de que esto sea lo suficientemente largo como para envolverlo para todos. Oh, puedes *poner* **Markdown** en una cita en bloque. 
```

::: tip
Puede decirle a QOwnNotes que resalte completamente las citas en bloque o solo el carácter de la cita en bloque en la *Configuración del editor*

Puede usar el [atajo](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> para marcar texto como cita en bloque.
:::

## Regla horizontal

Hay tres formas de obtener una regla horizontal: guiones, asteriscos o guiones bajos.

```markdown
Tres o más ...

Guiones

---

Asteriscos

***

Subrayados

___
```

## Saltos de línea

- **Dos líneas nuevas** le darán un **párrafo nuevo**.
- **Una nueva línea**le dará una** nueva línea en el mismo párrafo**.
- Para tener un salto de línea **sin un párrafo**, necesitará usar **dos espacios finales**.

```markdown
Aquí hay una línea para empezar.

Esta línea está separada de la anterior por dos nuevas líneas, por lo que será un *párrafo separado*.

Esta línea también comienza un párrafo separado, pero ...
Esta línea solo está separada por dos espacios finales y una sola línea nueva, por lo que es una línea separada en el *mismo párrafo*.
```

::: tip
Los espacios finales están resaltados de forma predeterminada en el editor.
:::

## Comentarios

Los comentarios no se muestran en la vista previa.

```markdown
[comentario]: # (este comentario no aparecerá en la vista previa)

<! - Los comentarios HTML también están ocultos - >
```

::: tip
A leading html comment block in a note will also be ignored in the automatic note filename generation.
:::

## Listas de casillas de verificación

You can create simple todo lists with checkbox lists.

```markdown
- [x] done
- [ ] todo
```

::: tip
You can check/un-check checkboxes in the preview.
:::

## Frontmatter

In QOwnNotes you can use a frontmatter (e.g. YAML) to add some extra meta information. It will **not be shown in the preview** and will **not disturb the the automatic note filename generation**.

```markdown
---
título: Algún nombre
descripción: Alguna descripción
---

# El título de la nota comienza aquí

Algún texto
```

The filename of this example note would be `Note headline starts here.md`.
