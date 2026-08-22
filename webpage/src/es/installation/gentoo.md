# Instalar en Gentoo Linux

Agregue el repositorio QOwnNotes con el siguiente comando (consulte [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Luego puede instalar QOwnNotes de la forma habitual.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[Repositorio (overlay) de QOwnNotes en GitHub](https://github.com/qownnotes/gentoo-overlay/)

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
