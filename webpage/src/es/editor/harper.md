# Harper

QOwnNotes puede usar [Harper](https://writewithharper.com/) para
**comprobar la gramática y estilo fuera de línea** opcional en el editor de notas.

Complementa el soporte existente para el [corrector ortográfico](spellchecking.md) en lugar de reemplazarlo.

![harper-demo](/img/editor/harper.webp)

## Características

- Subrayados de colores para detectar errores de gramática, estilo, puntuación y redacción
- Sugerencias de sustituciones en el menú contextual
- `Ignore this rule` and `Ignore word` actions in the context menu
- A quick toggle in the **Edit** menu with `Check grammar with Harper`
- Local checking with the `harper-ls` language server over `Stdio` or `TCP`

## Configuración

Open `Settings` and select `Editor -> Harper`.

- Turn on `Enable offline grammar and style checking with Harper`
- Choose the `Transport`
  - `Stdio (recommended)` starts a local `harper-ls` process directly
  - `TCP` connects to an already running `harper-ls` server
- For `Stdio`, set the `Command / binary path`
  - El comando por defecto es `harper-ls`
  - `Auto-detect` searches for it in your `PATH`
- For `TCP`, set the `Server address` and `Port`
  - The defaults are `127.0.0.1` and `4000`
- Elija el `dialecto`
  - `Estadounidense`
  - `Británico`
  - `Australiano`
  - `Canadiense`
  - `Indio`
- Adjust the `Check delay` to control how long QOwnNotes waits after typing before sending a request
- Select which linters should be enabled
  - `Spell Check`
  - `Sentence Capitalization`
  - `Palabras repetidas`
  - `Oraciones largas`
  - `An vs A`
  - `Unclosed Quotes`
  - `Correct Number Suffix`
  - `Espacios`
  - `Quote Spacing`
  - `No French Spaces`
  - `Wrong Apostrophe`
  - `Spelled Numbers`

Use `Test Connection` to verify that QOwnNotes can either start `harper-ls` in `Stdio` mode or
reach the configured server in `TCP` mode.

## Cómo funciona

- QOwnNotes comprueba los bloques del editor que son visibles, en lugar de todo el documento de una sola vez
- Se omiten las líneas en blanco, los encabezados y los bloques de código
- Requests are debounced so Harper is not queried on every keystroke
- Results are shown inline in the Markdown editor
- Harper and LanguageTool can be enabled at the same time

## Notas

- Harper support is optional and depends on the feature being enabled in your build
- `Stdio` mode is the simplest setup for local offline checking
- If Harper cannot be reached, QOwnNotes shows a warning and stops checking until it is available again

## Relacionados

- [Corrector ortográfico](spellchecking.md)
- [LanguageTool](languagetool.md)
- [Concepto](../getting-started/concept.md)
