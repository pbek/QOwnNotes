# Markdown LSP

QOwnNotes supports integration with a **Markdown Language Server Protocol (LSP)** server for
enhanced editing features such as diagnostics (linting), completions, code actions, and
document formatting directly in the note editor.

## Características

- Colored underlines for detected Markdown issues and rule violations
- Context-menu **code actions** for diagnostics (e.g. quick fixes suggested by the server)
- **Ignore rule** action in the context menu to suppress specific diagnostic rules permanently
- **Completions** popup for note links and other server-provided suggestions (e.g. wiki-link completions with [Marksman](https://github.com/artempyanykh/marksman))
- **Format Document** and **Format Selection** actions via the context menu
- Incremental document sync for efficient communication with the server
- Verbose logging mode for debugging server communication

## Supported Language Servers

Two Markdown LSP servers are known and supported out of the box:

| Servidor                                             | Comando        | Notas                                                                                                |
| ---------------------------------------------------- | -------------- | ---------------------------------------------------------------------------------------------------- |
| [Marksman](https://github.com/artempyanykh/marksman) | `marksman`     | Predeterminado; proporciona completado automático de enlaces de estilo wiki y navegación entre notas |
| [Rumdl](https://github.com/rvben/rumdl)              | `rumdl server` | Markdown linting rules (similar to `markdownlint`)                                |

Any LSP-compatible Markdown server that communicates over `stdio` should also work by
configuring the command manually.

## Configuración

Open `Settings` and navigate to `Editor -> Markdown LSP`.

- Turn on **Enable Markdown language server (LSP)**
- Set the **Command** field to the executable name or full path of your LSP server
  - El valor por defecto es `marksman`
  - Use **Auto-detect** to automatically search your `PATH` for `marksman` or `rumdl`
    and fill in the command field
- Optionally provide **Arguments** to pass to the server process
  - Para `rumdl` el argumento obligatorio es `server`
- Enable **Show all LSP server log output in debug log** for verbose diagnostic output
- Use **Test Connection** to verify that QOwnNotes can start the server and complete
  the LSP initialization handshake against your current note folder
- Use **Reset ignored rules** to clear all previously ignored diagnostic rule IDs

## Cómo funciona

- When you open a note, QOwnNotes sends a `textDocument/didOpen` notification to the server
- Mientras escribe, los cambios se reenvían mediante `textDocument/didChange` usando sincronización incremental
  cuando el servidor lo soporta, o sincronización completa de documentos en caso contrario
- Diagnostics pushed by the server via `textDocument/publishDiagnostics` are rendered as
  **colored underlines** in the Markdown editor
- Right-clicking on an underlined region shows available **code actions** and the option
  to **ignore the rule** for that diagnostic
- Completions are shown as a popup when the server provides completion items
- Document formatting is sent as `textDocument/formatting` or `textDocument/rangeFormatting`

## Installing a Markdown LSP Server

### Marksman

Marksman provides wiki-link completions, cross-note navigation, and diagnostics for
Markdown projects. It works especially well with QOwnNotes' wiki-style note links.

- **Linux/macOS**: Descargue desde los [lanzamientos de GitHub](https://github.com/artempyanykh/marksman/releases)
  o instale a través de su gestor de paquetes (por ejemplo, `brew install marksman` en macOS,
  `nix-env -iA nixpkgs.marksman` en NixOS)
- **Windows**: Descargue el ".exe" desde la página de lanzamientos de GitHub y colóquelo en algún lugar de su ruta "PATH"

Después de instalar, use **Detección automática** en la configuración para localizarlo automáticamente.

### Rumdl

[Rumdl](https://github.com/rvben/rumdl) es un linter rápido de Markdown que aplica reglas de estilo configurables y puede ejecutarse como un servidor LSP.

**Instalar mediante Cargo (Rust):**

```bash
cargo install rumdl
```

**Instalar mediante pip:**

```bash
pip install rumdl
```

**Descargar un binario precompilado:**

Los binarios precompilados para Linux, macOS y Windows están disponibles en la página de [lanzamientos de Rumdl en GitHub](https://github.com/rvben/rumdl/releases).

After installing, set the **Command** to `rumdl` and the **Arguments** to `server`.

## Notas

- El cliente LSP se comunica con el servidor a través de `stdio` (entrada/salida estándar)
- El proceso del servidor se inicia utilizando la carpeta de notas actual como raíz del espacio de trabajo
- Ignored rules are stored persistently and can be cleared with **Reset ignored rules**
- Si no se puede iniciar el servidor, QOwnNotes muestra una advertencia en el área de estado

## Relacionado

- [Corrector ortográfico](spellchecking.md)
- [LanguageTool](languagetool.md)
- [Harper](harper.md)
- [Concepto](../getting-started/concept.md)
