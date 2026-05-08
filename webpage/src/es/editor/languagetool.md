# LanguageTool

QOwnNotes puede utilizar un servicio [LanguageTool](https://languagetool.org/) local o remoto para
realizar una **revisión gramatical y de estilo** opcional en el editor de notas.

Complementa la función de [corrección ortográfica](spellchecking.md) ya existente, en lugar de sustituirla.

## Características

- Subrayados de colores para indicar los problemas detectados de gramática, estilo, puntuación y tipografía
- Sugerencias de sustituciones en el menú contextual
- `More info...` links for rules that provide extra documentation
- `Ignore this rule` actions for rules you do not want to see again
- Un interruptor rápido en el menú **Ortografía** con `Verificar gramática con LanguageTool`

## Configuración

Open `Settings -> Editor` and configure the **LanguageTool** section.

- Turn on `Enable grammar and style checking with LanguageTool`
- Establezca la "URL del servidor"
  - Los servidores locales pueden usar algo como `http://localhost:8081`
  - QOwnNotes utiliza automáticamente el endpoint `/v2/check` en ese servidor
- Selecciona el "Idioma"
  - "Detectar automáticamente" está disponible
  - También puede introducir un código de idioma personalizado si es necesario
- Agregue una clave de API `API key` opcional si su servicio de LanguageTool la requiere
- Adjust the `Check delay` to control how long QOwnNotes waits after typing before sending a request
- Seleccione las categorías que deben verificarse
  - `Ortografía`
  - `Gramática`
  - `Estilo`
  - `Puntuación`
  - `Tipografía`

Utilice la función "Probar conexión" para verificar que el servicio configurado sea accesible.

## Cómo funciona

- QOwnNotes comprueba los bloques del editor que están visibles, en lugar de todo el documento de una sola vez
- Las líneas en blanco, los encabezados y los bloques de código se omiten
- Las solicitudes se gestionan mediante un retardo, de modo que el servicio no se consulta con cada pulsación de tecla
- Los resultados se muestran directamente en el editor de Markdown

## Notas

- La compatibilidad con LanguageTool es opcional y depende de que la función esté habilitada en su compilación
- Si no se puede acceder al servicio, QOwnNotes muestra una advertencia y deja de comprobarlo hasta que vuelva a estar disponible
- Puede utilizarlo con un servidor LanguageTool autohospedado localmente u otro punto final de LanguageTool compatible

## Relacionados

- [Corrector ortográfico](spellchecking.md)
- [Concepto](../getting-started/concept.md)
