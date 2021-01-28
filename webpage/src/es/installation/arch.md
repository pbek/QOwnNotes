# Install on Arch Linux

## pacman

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

If the command `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` fails with a message like: `ERROR: FFC43FC94539B8B0 could not be locally signed.`, you could first find out the actual *keyid* of the downloaded key, i.e. with the command (and output):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Tratando de adivinar a qué te refieres ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

Vous pouvez maintenant synchroniser votre base de données de packages et installer le package avec `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[Descarga Directa](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Arch_Extra)

::: tip
Por supuesto, también puede usar este repositorio con otras distribuciones basadas en Arch Linux, como Manjaro.
:::

## Repositorio de usuarios de Arch (AUR)

Il existe également un package officiel pour QOwnNotes sur AUR, il s'appelle `qownnotes`.

Vous le trouverez ici: [QOwnNotes sur AUR](https://aur.archlinux.org/packages/qownnotes)

Synchronisez votre base de données de packages et installez le package avec `yay`:

```bash
yay -S qownnotes
```

::: tip
Si vous souhaitez accélérer le temps de construction, vous pouvez lire [CCACHE et AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::
