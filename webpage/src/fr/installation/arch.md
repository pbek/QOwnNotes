# Installer sur Arch Linux

## pacman

Ajoutez les lignes suivantes à votre `/etc/pacman.conf` avec `sudo nano /etc/pacman.conf`:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Exécutez les commandes shell suivantes pour approuver le référentiel:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

If the command `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` fails with a message like: `ERROR: FFC43FC94539B8B0 could not be locally signed.`, you could first find out the actual *keyid* of the downloaded key, i.e. with the command (and output):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Essayer de deviner ce que vous voulez dire...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

Vous pouvez maintenant synchroniser votre base de données de packages et installer le package avec `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Arch_Extra)

::: tip
Bien sûr, vous pouvez également utiliser ce référentiel avec d'autres distributions basées sur Arch Linux, comme Manjaro.
:::

## Arch User Repository (AUR)

Il existe également un package officiel pour QOwnNotes sur AUR, il s'appelle `qownnotes`.

Vous le trouverez ici: [QOwnNotes sur AUR](https://aur.archlinux.org/packages/qownnotes)

Synchronisez votre base de données de packages et installez le package avec `yay`:

```bash
yay -S qownnotes
```

::: tip
Si vous voulez accélérer le temps de construction, vous pouvez lire [CCACHE et AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::
