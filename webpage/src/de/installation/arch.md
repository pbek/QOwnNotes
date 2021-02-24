# Installieren Sie unter Arch Linux

## pacman

Fügen Sie Ihrer `/etc/pacman.conf` mit `sudo nano/etc/pacman.conf` die folgenden Zeilen hinzu:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

Wenn der Befehl `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` mit der folgenden Meldung fehlschlägt: `FEHLER: FFC43FC94539B8B0 konnte nicht lokal signiert werden.`, Sie konnten zuerst den tatsächlichen Wert *keyid* des heruntergeladenen Schlüssels, d.h. mit dem Befehl (und der Ausgabe) ermitteln:

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Trying to guess what you mean ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

You can now synchronize your package database and install the package with `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[Direkter Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Arch_Extra)

::: tip
Natürlich können Sie dieses Repository auch mit anderen Arch Linux-basierten Distributionen wie Manjaro verwenden.
:::

## Arch User Repository (AUR)

Alternativ gibt es auch ein offizielles Paket für QOwnNotes auf AUR, es heißt `qownnotes`.

Sie finden es hier: [QOwnNotes on AUR](https://aur.archlinux.org/packages/qownnotes)

Synchronisieren Sie Ihre Paketdatenbank und installieren Sie das Paket mit `yay`:

```bash
yay -S qownnotes
```

::: tip
Wenn Sie die Erstellungszeit beschleunigen möchten, lesen Sie [CCACHE and AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::
