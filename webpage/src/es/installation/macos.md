# Instalar en macOS ™

Descargue la última **imagen de disco de macOS** desde los[lanzamientos de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/releases). Ábrala y arrastre la aplicación **QOwnNotes** a su **carpeta de aplicaciones**.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
Si recibe un mensaje de error `QOwnNotes.app no se puede abrir porque no se puede verificar el desarrollador`, debe visitar su **Configuración de seguridad** de macOS y hacer clic en `Abrir de todos modos`.
:::

::: tip
The application uses its **internal updater** to update itself. You can see the latest version on the bottom right of the application.
:::

## Homebrew

Hay un barril de QOwnNotes mantenido por la comunidad en [Homebrew](https://formulae.brew.sh/cask/qownnotes).

Puedes instalarlo con:

```bash
brew install qownnotes
```

## Nix

You can also install QOwnNotes with the [Nix package manager](https://wiki.nixos.org/wiki/Nix_package_manager) under macOS natively on **x86 and Apple Silicon**, see [Install via Nix](./nix.md).
