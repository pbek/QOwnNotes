# Instalar en KaOS Linux

Hay un paquete mantenido por la comunidad `qownnotes` en el repositorio [Paquetes de la comunidad KaOS](https://github.com/KaOS-Community-Packages/qownnotes).

Puede instalarlo escribiendo:

```bash
kcp -i qownnotes
```

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale los paquetes de Secret Service que falten y reinicie su sesión de escritorio.

Para KDE Plasma:

```bash
sudo pacman -S kwalletmanager kwallet
```

Para escritorios basados en GNOME y otros sistemas de Servicio Secreto, instale `gnome-keyring`, `libsecret` y `seahorse` desde sus repositorios configurados.

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
