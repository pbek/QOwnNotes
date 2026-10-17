# Instalar en Slackware Linux

## Slackware 15.0

First install [Qt 6 from SlackBuilds.org](https://slackbuilds.org/repository/15.0/libraries/qt6/), including its required dependencies.

Then check out the QOwnNotes SlackBuilds repository and build the application.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/15.0/qownnotes
./dobuild.sh
```

Después de eso, puede instalar el paquete generado con `installpkg`.

[Slackbuild de QOwnNotes en GitHub](https://github.com/pbek/qownnotes-slackbuilds/)

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale una implementación de Secret Service y reinicie su sesión de escritorio.

Para escritorios basados en GNOME y otros sistemas de Servicio Secreto, instale `gnome-keyring`, `libsecret` y `seahorse` desde sus repositorios de Slackware o SlackBuilds.

Para KDE Plasma, instale soporte para KWallet como `kwalletmanager`.

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
