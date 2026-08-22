# Instalar en Solus

Hay un paquete `qownnotes` mantenido por [Proyecto Solus](https://getsol.us/).

Puede instalarlo escribiendo:

```bash
sudo eopkg install qownnotes
```

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale los paquetes de Secret Service que falten y reinicie su sesión de escritorio.

Para GNOME y otros ordenadores de escritorio basados en Servicio Secreto:

```bash
sudo eopkg install gnome-keyring libsecret seahorse
```

Para KDE Plasma, instale el soporte de KWallet desde sus repositorios configurados.

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
