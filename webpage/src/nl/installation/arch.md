# Installatie op Arch Linux

## Arch User Repository (AUR)

Als alternatief is er ook een officieel pakket voor QOwnNotes op AUR, het heet `qownnotes`.

U vindt het hier: [QOwnNotes op AUR](https://aur.archlinux.org/packages/qownnotes)

Synchroniseer uw pakketdatabase en installeer het pakket met `yay`:

```bash
yay -S qownnotes
```

::: tip
Als je de bouwtijd wilt versnellen, wil je misschien [CCACHE en AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/) lezen.
:::

## pacman

Voeg de volgende regels toe aan je `/etc/pacman.conf` with `sudo nano /etc/pacman.conf`:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Voer de volgende shell-opdrachten uit om de repository te vertrouwen:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

If the command `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` fails with a message like: `ERROR: FFC43FC94539B8B0 could not be locally signed.`, you could first find out the actual _keyid_ of the downloaded key, i.e. with the command (and output):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Ik probeer te raden wat je bedoelt ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

U kunt nu uw pakketdatabase synchroniseren en het pakket installeren met `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra)

::: tip
Je kunt deze repository natuurlijk ook gebruiken met andere op Arch Linux gebaseerde distributies, zoals Manjaro.
:::
