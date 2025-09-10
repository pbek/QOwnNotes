# Instalar en Arch Linux

## Arch User Repository (AUR)

Alternativamente, también hay un paquete oficial para QOwnNotes en el AUR. Se llama `qownnotes`.

Lo encontrará aquí: [QOwnNotes en el AUR](https://aur.archlinux.org/packages/qownnotes)

Sincronice su base de datos de paquetes e instale el paquete con `yay`:

```bash
yay -S qownnotes
```

:: tip Si desea acelerar el tiempo de construcción, es posible que desee leer [CCACHE y AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::

## pacman

Agregue las siguientes líneas a su `/etc/pacman.conf ` con `sudo nano /etc/pacman.conf`:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

Si el comando `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` falla con un mensaje como: `ERROR: FFC43FC94539B8B0 no se pudo firmar localmente.`, primero puede averiguar el _keyid_ real de la clave descargada, por ejemplo, con el comando (y salida):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Trying to guess what you mean ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

Ahora puede sincronizar su base de datos de paquetes e instalar el paquete con `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra)

::: tip
Por supuesto, también puede utilizar este repositorio con otras distribuciones basadas en Arch Linux, como Manjaro.
:::
