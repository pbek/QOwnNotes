# ¿Por qué métricas?

## ¿Por qué son útiles las métricas?

QOwnNotes (como en la propia aplicación) es **desarrollado por una y a veces dos personas en su tiempo libre**.

Los lanzamientos se crean para muchas plataformas y distribuciones (a menudo, versiones muy antiguas de las mismas). Mantenerlas es mucho trabajo.

Las métricas son la única fuente confiable de información en qué versiones de plataformas y distribuciones QOwnNotes funciona. También muestran qué funciones son las más populares, de modo que se pueden tomar decisiones sobre qué versiones retirar, qué funciones necesitan más trabajo y cuáles apenas se usan.

Lo mismo se aplica a la selección de idiomas preferidos por parte del usuario, lo que promueve la mejora de las traducciones correspondientes.

## ¿Dónde se almacenan las métricas?

Las métricas se transmiten y almacenan en un servidor [Matomo](https://matomo.org/) autohospedado. Solo los dos desarrolladores principales tienen acceso a los datos. **No se comparten datos con terceros.**

## ¿Cómo se recopilan las métricas?

Las métricas se **anonimizan** (p. ej., se elimina la dirección IP) en el servidor de Matomo después de recibirlas.

¡Ninguna información personal, como el texto que ingresó o los nombres de las notas, se recopila o almacena en el servidor!

## ¿Qué se recopila?

Se transmite cierta información básica como la resolución de pantalla, el sistema operativo y la versión de QOwnNotes utilizada.

Además, por ejemplo, los eventos se envían al servidor cuando se utilizan acciones en las aplicaciones.

QOwnNotes es un software gratuito de código abierto. Puede revisar las métricas todo el tiempo buscando, por ejemplo, [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) y [enviarEventoSiHabilitado](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) en el código fuente para ver qué se envía exactamente.

**¡No se recopila información personal, como el texto que ingresó o los nombres de las notas!**

## ¿Puedo desactivar las métricas?

Puede desactivarlos la primera vez que inicie la aplicación en el cuadro de diálogo de bienvenida o más tarde en el cuadro de diálogo de configuración.

## ¿Por qué las métricas no están desactivadas de forma predeterminada?

Los valores predeterminados son rey. Desactivar las métricas por defecto daría como resultado la ausencia total de métricas significativas.

A la mayoría de la gente no le importan las métricas. Aquellos que se preocupan por las métricas y no quieren votar con sus métricas sobre en qué se trabaja o qué se admite, pueden desactivarlas fácilmente en el primer inicio de la aplicación.

Incluso obtendrá un segundo cuadro de diálogo que le informará sobre las métricas si terminó el cuadro de diálogo de bienvenida demasiado pronto.
