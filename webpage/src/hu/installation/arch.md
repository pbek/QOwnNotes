# Telepítés Arch Linux rendszerre

## pacman

Adja hozzá a következő sorokat a `/etc/pacman.conf` fájlba a `sudo nano /etc/pacman.conf` paranccsal:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Futtassa a következő shell parancsokat a lerakat megbízhatóságához:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

Ha a `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` parancs meghiúsul, például: `HIBA: FFC43FC94539B8B0 nem lehet helyileg aláírni.`, először megtudhatja a tényleges A letöltött kulcs *kulcsazonosítója*, azaz a paranccsal (és kimenettel)

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Megpróbáljuk kitalálni, mire gondol ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

Most szinkronizálhatja a csomagadatbázist, és telepítheti a csomagot a `pacman` paranccsal:

```bash
sudo pacman -Syy qownnotes
```

[Közvetlen letöltés](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Arch_Extra)

::: tip Természetesen ezt az adattárat használhatja más Arch Linux alapú disztribúciókkal is, például a Manjaróval. :::

## Arch felhasználói adattár (AUR)

Alternatív megoldásként létezik egy hivatalos csomag is az AUR-on lévő QOwnNotes számára, amelyet `qownnotes` -nek hívnak.

Itt találja: [QOwnNotes az AUR-on](https://aur.archlinux.org/packages/qownnotes)

Szinkronizálja a csomagadatbázist, és telepítse a csomagot a `yay` paranccsal:

```bash
yay -S qownnotes
```

::: tip Ha fel akarja gyorsítani az építési időt, érdemes elolvasnia a [CCACHE és az AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/) t. :::
