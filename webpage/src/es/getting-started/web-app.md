---
image: /img/blog/web-application-settings.png
---

# Aplicación web de QOwnNotes

Puede insertar fotos de su teléfono móvil en la nota actual en QOwnNotes en su escritorio a través de la **aplicación web ** en [app.qownnotes.org](https://app.qownnotes.org/).

![Navegador de aplicaciones web QOwnNotes](/img/blog/web-application-browser.png "Envíe fotos desde su teléfono móvil a QOwnNotes en el escritorio")

La aplicación web de su teléfono se comunica con su aplicación de escritorio QOwnNotes a través de Internet. La comunicación entre QOwnNotes y el servicio web está ** encriptada por transporte **. No se almacenarán imágenes en el servidor.

Había una página de configuración ** aplicación web ** para agregar compatibilidad con la aplicación web.

![Configuración de la aplicación web QOwnNotes](/img/blog/web-application-settings.png "Configurar la comunicación con la aplicación web")

No olvide **copiar su token secreto** de la página de configuración de QOwnNotes a [app.qownnotes.org](https://app.qownnotes.org/) para poder comunicarse con su instancia local de QOwnNotes. También puede escanear el token del código QR con su teléfono en la aplicación web.

Puede encontrar el código fuente de la aplicación web en [aplicación web en GitHub](https://github.com/qownnotes/web-app).

También puede alojar este servicio web usted mismo; las imágenes de la ventana acoplable recién compiladas están disponibles en [ qownnotes-web-app en DockerHub ](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
