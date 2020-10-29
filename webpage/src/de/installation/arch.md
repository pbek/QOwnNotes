# Install on Arch Linux

## pacman

Add the following lines to your `/etc/pacman.conf` with `sudo nano /etc/pacman.conf`:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Run the following shell commands to trust the repository:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key FFC43FC94539B8B0
```

Synchronize your package database and install the package with `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[Direct Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Arch_Extra)

::: tip
Of course you can also use this repository with other Arch Linux based distributions, like Manjaro.
:::

## Arch User Repository (AUR)

Alternatively there also is an official package for QOwnNotes on AUR, it is called `qownnotes`.

You will find it here: [QOwnNotes on AUR](https://aur.archlinux.org/packages/qownnotes)

Synchronize your package database and install the package with `yay`:

```bash
yay -S qownnotes
```

::: tip
If you want to speed up build time you may want to read
[CCACHE and AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::
