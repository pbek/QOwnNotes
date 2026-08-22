# Instalar como Snap

Hay un complemento `qownnotes` en la [tienda Snap](https://snapcraft.io/qownnotes).

Puede instalarlo escribiendo:

```bash
snap install qownnotes
```

Posteriormente, será capaz de ejecutar QOwnNotes mediante el comando `qownnotes`.

::: tip
Los [snaps](http://snapcraft.io) funcionan en muchas distribuciones de Linux, como Ubuntu, Arch Linux, Debian, Fedora, openSUSE, Gentoo Linux, OpenWRT, OpenEmbedded y Yocto Project.
:::

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale los paquetes de Secret Service que falten en el sistema anfitrión y reinicie su sesión de escritorio.

Para escritorios basados en GNOME y otros sistemas de Servicio Secreto, instale `gnome-keyring`, `libsecret` y `seahorse`.

Para KDE Plasma, instale soporte para KWallet como `kwalletmanager`.

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
