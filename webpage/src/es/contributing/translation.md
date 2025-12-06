# Traducción

¡Gracias por colaborar!

¿Cómo puedo ayudar a traducir QOwnNotes?

::: warning
Por favor, no intente editar los archivos de traducción en `src/languages` directamente. Todos se generan automáticamente mediante [Crowdin](https://crowdin.com/project/qownnotes).
:::

## Únase a nosotros en Crowdin para traducir QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** es una plataforma popular de gestión de localización para aplicaciones móviles, web, software de escritorio y recursos relacionados.

Puede registrarse en [Crowdin](https://crowdin.com/project/qownnotes) y **traducir** **QOwnNotes** a su idioma con herramientas fáciles.

**[QOwnNotes en Crowdin](https://crowdin.com/project/qownnotes)**

Simplemente [envíe un mensaje al propietario del proyecto en Crowdin](https://crowdin.com/profile/pbek) o como una [incidencia de GitHub](https://github.com/pbek/QOwnNotes/issues) si desea traducir QOwnNotes a un nuevo idioma.

Si desea agregar nuevas traducciones a **idiomas existentes**, puede hacerlo de inmediato.

## Revisión y corrección

Si usted es un hablante nativo de un idioma ya traducido que aún no ha sido corregido, agradecemos su ayuda para hacer que la experiencia de QOwnNotes sea más confiable y conveniente verificando, y si es necesario, corrigiendo las traducciones no aprovadas. Debe pedirle permiso al propietario para ser un revisor y corrector en su idioma.

¡Haga clic aquí para ver si su idioma está entre los traducidos!

[QOwnNotes en Crowdin](https://translate.qownnotes.org/)

Una vez que [se haya puesto en contacto con el propietario del proyecto en Crowdin](https://crowdin.com/profile/pbek) y haya recibido permiso para revisar y corregir en su idioma, ¡estará listo para comenzar!

## Consejos generales

Por favor tenga en cuenta algunas cosas mientras trabaja en el proyecto.

### Exactitud

Por favor localice con la máxima exactitud posible mientras cuida la fluidez en su idioma de destino.

### Precisión

El objetivo debe representar el orígen de manera exacta, y sin omisiones ni adiciones en ninguna parte de la cadena.

### Contexto

Si requiere de mayor contexto, o de ayuda con una cadena en particular, y sabe otro idioma, puede cambiar el idioma y ver lo que otros traductores hicieron del segmento. No obstante, siéntase libre de dejar un mensaje y solicitar una aclaración en el lado derecho de la página.

### Términos clave de uso frecuente

Es necesario comprobar y utilizar las mismas traducciones para los términos clave de uso frecuente. Por ejemplo, "nota" debe traducirse siempre de la misma manera.

Si `trashed` es `notas en la papelera` en español, siempre lo traduciría utilizando esta expresión. La excepción sería cuando gramaticalmente esté obligado a usar una alternativa, pero manténgalo simple.

![screenshot](/img/crowdin/screenshot-7.png)

Para encontrar todas las expresiones idénticas, busque en Crowdin para ver si todas las apariciones de un término tienen la misma traducción. Si sabe más que ellos, puede sugerir una mejor traducción y votar a favor (o en contra) de la mejor traducción o más correcta.

Traduzca todas las apariciones de la forma más idéntica posible.

`Nota` definitivamente siempre debería tener una sola traducción.

### Formas plurales

Para el plural, por favor agregue la traducción apropiada, cada una en el bloque correspondiente.

![screenshot](/img/crowdin/screenshot-4.png)

### Variables

Las variables `%1` deben escribirse sin espacios; en los idiomas de derecha a izquierda puede o bien insertar las variables como aparecen originalmente, o escribirlas en su traducción.

![screenshot](/img/crowdin/screenshot-1.png)

![screenshot](/img/crowdin/screenshot-5.png)

![screenshot](/img/crowdin/screenshot-3.png)

### Claves de acceso

Utilice `&` delante de cualquier letra que deba usarse en un atajo. Puede ser igual que en el original pero no es obligatorio.

![screenshot](/img/crowdin/screenshot-4.png)

### Contenedores personalizados

Cadenas como `::: tip`, `::: warning` e `::: info` en la documentación, son marcadores para [contenedores personalizados](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) en VuePress. No deberían ser traducidos.

### Otras cadenas que no deben ser traducidas

Algunos segmentos que no traducimos en la Documentación:

- Nunca traduzca códigos o enlaces. Las etiquetas deben ser dejadas como están (y sin espacios añadidos, ya que algunas traducciones automáticas hacen eso. Esas deben ser eliminadas)
- No traducimos la fecha y hora (dejar en blanco)
- Y tampoco traducimos las referencias (que QOwnNotes obtuvo en numerosos idiomas de traducción), por lo que simplemente puede omitirlas.

## ¡Traducir, revisar y corregir puede ser emocionante!

Tratará con el lenguaje de este software, aprenderá nuevos términos y recibirá mucha información interna sobre el proyecto.

Si está listo para ayudar a corregir la traducción de su idioma, comuníquese con [Patrizio](https://crowdin.com/profile/pbek) y pregunte si tiene alguna pregunta y/u obtenga los derechos para la revisión y corrección en su idioma nativo.

¡Muchísimas gracias por su ayuda! 🙂
