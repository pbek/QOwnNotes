---
image: /img/bookmarks.png
---

# Extensión del navegador QOwnNotes Web Companion

![marcadores](/img/bookmarks.png)

Visite [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) o la [página de complementos de Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) para instalar la [**extensión del navegador QOwnNotes Web Companion**](https://github.com/qownnotes/web-companion/).

También puede encontrar la extensión en [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes debe estar ejecutándose para que funcione la extensión del navegador Web Companion. Las extensiones del navegador funcionan ** sin conexión **, no se necesita conexión a Internet.
:::

## Cortadora web

Haga clic con el botón derecho en una página web o en el texto seleccionado para usar la funcionalidad **web-clipper**. Allí también puede crear una nueva nota con una ** captura de pantalla** de la página web actual.

::: tip
¡El cortador web también es programable! Eche un vistazo a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) si desea controlar lo que recorta de las páginas web.
:::

## Marcadores

Si desea tener un control total sobre los marcadores de su navegador y ** usarlos en diferentes navegadores y sistemas operativos **, entonces la extensión del navegador QOwnNotes es para usted.

De forma predeterminada, la extensión del navegador mostrará todos los enlaces ** de la nota actual** en una ventana emergente cuando haga clic en el icono QOwnNotes en su navegador. Estos enlaces obtendrán una etiqueta `current`.

También puede **administrar sus marcadores en notas** con la etiqueta de nota `marcadores` (modificable en la configuración). Estos enlaces también pueden tener etiquetas y una descripción que se mostrará en la extensión del navegador.

Los nuevos marcadores se almacenan en una nota llamada `Marcadore` (también modificable en la configuración).

::: tip
¡También puede importar los marcadores de su navegador a QOwnNotes con la extensión del navegador web complementario!
:::

### Sintaxis de los enlaces de marcadores

```markdown
- [nombre de la página web] (https://www.example.com)
- [nombre de la página web] (https://www.example.com) #tag1 #tag2
- [Nombre de la página web] (https://www.example.com) solo una descripción
- [Nombre de la página web] (https://www.example.com) #tag1 #tag2 alguna descripción y etiquetas
* [Nombre de la página web] (https://www.example.com) el carácter de lista alternativo también funciona
```

Puede buscar nombre, etiquetas de URL o descripción en la extensión del navegador.
