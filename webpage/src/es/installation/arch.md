# Instalar en Arch Linux

## Repositorio para Usuarios de Arch (AUR)

Alternativamente, también hay un paquete oficial para QOwnNotes en AUR, se llama `qownnotes`.

Lo encontrarás aquí:[QOwnNotes on AUR](https://aur.archlinux.org/packages/qownnotes)

Sincroniza tu base de datos de paquetes e instala el paquete con `yay`:

```bash
yay -S qownnotes
```

:: tip Si deseas acelerar el tiempo de construcción, es posible que desee leer [CCACHE y AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::

## pacman

::: advertencia [OBS](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop) actualmente, parece tener problemas de compilación en Arch Linux. Mejor utilizar el AUR o la [AppImage](./appimage.md) por ahora.
:::

Agregue las siguientes líneas a su `/etc/pacman.conf ` con `sudo nano /etc/pacman.conf`:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Ejecute los siguientes comandos de shell para confiar en el repositorio:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

Si el comando `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` falla con un mensaje como: `ERROR: FFC43FC94539B8B0 no se pudo firmar localmente.`, primero puede averiguar el *keyid* real de la clave descargada, por ejemplo, con el comando (y salida):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Tratando de adivinar a qué te refieres...
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
