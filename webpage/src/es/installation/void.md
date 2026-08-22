# Instalar en Void Linux

Hay un paquete creado por la comunidad para **Void Linux** en [qownnotes srcpkg](https://github.com/void-linux/void-packages/tree/master/srcpkgs/qownnotes) que se puede instalar de la siguiente manera:

```bash
# Sincronice los archivos de índice del repositorio remoto e instale QOwnNotes
xbps-install -S qownnotes
```

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale los paquetes de Secret Service que falten y reinicie su sesión de escritorio.

Para GNOME y otros ordenadores de escritorio basados en Servicio Secreto:

```bash
sudo xbps-install -S gnome-keyring libsecret seahorse
```

Para KDE Plasma:

```bash
sudo xbps-install -S kwalletmanager
```

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
