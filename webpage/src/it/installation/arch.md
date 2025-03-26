# Installa su Arch Linux

## Arch User Repository (AUR)

In alternativa c'è anche un pacchetto ufficiale per QOwnNotes su AUR, si chiama `qownnotes`.

Lo troverai qui: [QOwnNotes su AUR](https://aur.archlinux.org/packages/qownnotes)

Sincronizza il database dei pacchetti e installa il pacchetto con `yay`:

```bash
yay -S qownnotes
```

::: tip
Se vuoi accelerare i tempi di costruzione potresti voler leggere [CCACHE and AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::

## pacman

Aggiungi le seguenti righe al tuo `/etc/pacman.conf` con `sudo nano /etc/pacman.conf`:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Esegui i seguenti comandi della shell per considerare attendibile il repository:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

If the command `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` fails with a message like: `ERROR: FFC43FC94539B8B0 could not be locally signed.`, you could first find out the actual _keyid_ of the downloaded key, i.e. with the command (and output):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Trying to guess what you mean ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

Ora puoi sincronizzare il database dei pacchetti e installare il pacchetto con `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra)

::: tip
Ovviamente puoi anche usare questo repository con altre distribuzioni basate su Arch Linux, come Manjaro.
:::
