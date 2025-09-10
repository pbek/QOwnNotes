---
image: /img/bookmarks.png
---

# Extensión del navegador QOwnNotes Web Companion

Permite recortar desde una página del navegador y administrar los marcadores del navegador en todos los navegadores y sistemas operativos.

::: tip
Información

- QOwnNotes debe estar ejecutándose para que funcione la extensión del navegador Web Companion.
- No se necesita una conexión a Internet. Las extensiones del navegador funcionan **sin conexión**.
:::

## Instalación

1. Obtenga la extensión
   - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
   - [Página de complementos para Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
   - También puede encontrar la extensión en [GitHub](https://github.com/qownnotes/web-companion/).
2. Agregue el token de seguridad para configurar la extensión.
   - La primera vez que haga clic en el ícono de la extensión del navegador QOwnNotes, recibirá un cuadro de diálogo con una ficha de seguridad. Copie el token.
   - Vaya a la ubicación de administración de extensiones de su navegador. Haga clic en los detalles de la extensión QOwnNotes.
   - Pegue el token en el campo Token de seguridad.
3. Para usar el modo privado, deberá permitir que la extensión se ejecute en ventanas privadas en la configuración de la extensión.

## Web Clipper

![web-clipper](/img/web-clipper.png)

Haga clic con el botón derecho en una página web o en el texto seleccionado para usar la funcionalidad **web-clipper**. Allí también puede crear una nueva nota con una ** captura de pantalla** de la página web actual.

::: warning
Información Si web clipper no funciona, por favor revise si la aplicación de QOwnNotes para escritorio está ejecutándose, y si la extensión del navegador Web Companion está configurada con la ficha de seguridad.

Además, asegúrese de aceptar el cuadro de diálogo de consentimiento cuando haga clic por primera vez en el ícono de la extensión en la barra de herramientas del navegador.

Si utiliza Firefox, también debe cerciorarse de que ha activado el permiso opcional `Acceder a sus datos en todos los sitios web` en la configuración de _Permisos_ de la extensión. De lo contrario, podría recibir el error `TypeError: chrome.tabs.captureVisibleTab no es una función`.
:::

::: tip
¡Web Clipper también es programable! Eche un vistazo a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) si desea controlar lo que recorta de las páginas web.
:::

## Marcadores

![bookmarks](/img/bookmarks.png)

Si desea tener un control total sobre los marcadores de su navegador y ** usarlos en diferentes navegadores y sistemas operativos **, entonces la extensión del navegador QOwnNotes es para usted.

De manera predeterminada, la extensión del navegador mostrará todos los **enlaces de la nota actual** en una ventana emergente cuando pulse en el icono QOwnNotes en su navegador. Estos enlaces obtendrán una etiqueta `current`.

También puede **administrar sus marcadores en notas** con la etiqueta de nota `marcadores` (modificable en la configuración). Estos enlaces también pueden tener etiquetas y una descripción que se mostrará en la extensión del navegador.

Los nuevos marcadores se almacenan en una nota llamada `Marcadores` (también modificable en la configuración).

::: tip
¡También puede importar los marcadores de su navegador a QOwnNotes con la extensión de navegador Web Companion!
:::

### Sintaxis de los enlaces de marcadores

```markdown
- [Nombre de la página web] (https://www.ejemplo.com)
- [Nombre de la página web] (https://www.ejemplo.com) #etiqueta1 #etiqueta2
- [Nombre de la página web] (https://www.ejemplo.com) solo una descripción
- [Nombre de la página web] (https://www.ejemplo.com) #etiqueta1 #etiqueta2 alguna descripción y etiquetas

* [Nombre de la página web] (https://www.ejemplo.com) el carácter de lista alternativo también funciona
```

Puede buscar nombres, etiquetas de URLs o descripciones en la extensión del navegador. En la nota actual, todos los otros enlaces dentro de texto son analizados, pero sin analizar las etiquetas o la descripción.
