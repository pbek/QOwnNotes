# Traducción

¡Gracias por colaborar!

¿Cómo puedo ayudar a traducir QOwnNotes?

## Únase a nosotros en Crowdin para traducir QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** es una popular plataforma de gestión de localización para aplicaciones móviles, web, software de escritorio y activos relacionados.

Puede registrarse en [Crowdin](https://crowdin.com/project/qownnotes/invite) y **traducir** **QOwnNotes** a su idioma con herramientas sencillas.

**[QOwnNotes en Crowdin](https://crowdin.com/project/qownnotes/invite)**

Simplemente [envíe un mensaje al propietario del proyecto en Crowdin](https://crowdin.com/profile/pbek) o como un [problema de GitHub](https://github.com/pbek/QOwnNotes/issues) si desea traducir QOwnNotes a un nuevo idioma.

Si desea agregar nuevas traducciones a **idiomas existentes**, puede hacerlo de inmediato.

## Corrección de pruebas

Si es un hablante excelente o incluso nativo de un idioma ya traducido que aún no ha sido corregido, le agradecemos que ayude a que la experiencia de QOwnNotes sea más confiable y conveniente verificando y, si es necesario, corrigiendo las traducciones no aprobadas. Debe pedirle permiso al propietario para ser un corrector de pruebas para su idioma.

Haga clic aquí para ver si su idioma está entre los traducidos.

[QOwnNotes en Crowdin](https://translate.qownnotes.org/)

Una vez que [se haya puesto en contacto con el propietario del proyecto en Crowdin](https://crowdin.com/profile/pbek) y haya recibido permiso para corregir en su idioma, ¡estará listo para comenzar!

## Consejos generales

Tenga en cuenta algunas cosas mientras trabaja en el proyecto.

### Exactitud

Localice con la máxima precisión posible mientras cuida la fluidez en su idioma de destino.

### Precisión

El objetivo debe representar la fuente exactamente y sin omisiones ni adiciones de ninguna parte de la cadena.

### Contexto

Si necesita más contexto, o ayuda con una determinada cadena, y sabe otro idioma, puede cambiar el idioma y ver lo que otros traductores hicieron del segmento. Pero siéntete libre de dejar un mensaje y pedir una aclaración en el lado derecho de la página.

### Términos clave de uso frecuente

Es necesario comprobar y utilizar las mismas traducciones para los términos clave de uso frecuente, por ejemplo, "nota" debe traducirse siempre de la misma manera.

Si `notas en la papelera` es `gelöschte Dateien` en alemán, siempre lo traducirías usando esta expresión. La excepción sería cuando gramaticalmente esté obligado a usar una alternativa, pero manténgalo simple.

![captura de pantalla](/img/crowdin/screenshot-7.png)

Para encontrar todas las expresiones idénticas, busque en Crowdin para ver si todas las apariciones de un término tienen la misma traducción, si sabe mejor, y ellos no, puede sugerir una mejor traducción y votar hacia abajo (o hacia arriba) por la traducción mejor/correcta.

Localice todas las ocurrencias de la forma más idéntica posible.

`Nota` definitivamente siempre debería tener una sola traducción.

### Formas plurales

Para el plural, agregue la traducción apropiada cada uno en el bloque correspondiente.

![captura de pantalla](/img/crowdin/screenshot-4.png)

### Variables

Las variables `%1` deben escribirse sin espacios; en los lenguajes RTL puede insertar las variables del original o escribirlas en su traducción.

![captura de pantalla](/img/crowdin/screenshot-1.png)

![captura de pantalla](/img/crowdin/screenshot-5.png)

![captura de pantalla](/img/crowdin/screenshot-3.png)

### Teclas de acceso

Use `&` delante de cualquier letra que deba usarse en un atajo, puede ser igual que en el original pero no es obligatorio.

![captura de pantalla](/img/crowdin/screenshot-4.png)

### Contenedores personalizados

Strings wie `::: tip`, `::: warning` und `::: info` in der Dokumentation sind Markierungen für [benutzerdefinierte Container](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) in VuePress und sollten nicht übersetzt werden.

### Otras cadenas no se traducirán

Algunos segmentos que no traducimos en la Documentación:

- Nunca traduzca códigos o enlaces, las etiquetas deben dejarse como están (y sin espacios añadidos, ya que algunas traducciones automáticas hacen eso, deben eliminarse)
- Nosotras no traducimos fecha y hora (dejar en blanco)
- Y tampoco traducimos las referencias (que QOwnNotes obtuvo en numerosos idiomas de traducción), por lo que simplemente puede omitirlas.

## ¡Traducir y corregir puede ser emocionante!

Tratará con el lenguaje de este software, aprenderá nuevos términos y recibirá mucha información interna sobre el proyecto.

Si está listo para ayudar a corregir la traducción de su idioma, [Amy Fabijenna](https://crowdin.com/profile/rawfreeamy) le da la bienvenida a una breve sesión informativa sobre la práctica de la traducción y la corrección. Puedes preguntarle cualquier cosa que te ayude a divertirte contribuyendo.

¡Muchas gracias por tu ayuda! 🙂
