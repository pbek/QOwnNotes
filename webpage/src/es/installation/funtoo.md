# Instalar en Funtoo Linux

Funtoo Linux tiene un paquete de QOwnNotes autogenerado en [desktop-kit](https://github.com/funtoo/desktop-kit/tree/1.4-release/app-office/qownnotes), y puede instalarse de la siguiente manera:

```bash
sudo emerge -av qownnotes
```

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale los paquetes de Secret Service que falten y reinicie su sesión de escritorio.

Para GNOME y otros ordenadores de escritorio basados en Servicio Secreto:

```bash
sudo emerge -av gnome-extra/gnome-keyring app-crypt/libsecret app-crypt/seahorse
```

Para KDE Plasma:

```bash
sudo emerge -av kde-apps/kwalletmanager kde-frameworks/kwallet
```

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
