# Installieren Sie unter Arch Linux

## pacman

Fügen Sie Ihrer ` /etc/pacman.conf </ 0> mit <code> sudo nano /etc/pacman.conf </ 0> die folgenden Zeilen hinzu:</p>

<pre><code class="ini">[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
`</pre>

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key - lsign-key FFC43FC94539B8B0
```

Synchronisieren Sie Ihre Paketdatenbank und installieren Sie das Paket mit ` pacman `:

```bash
sudo pacman -Syy qownnotes
```

[Direkter Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Arch_Extra)

::: tip
Natürlich können Sie dieses Repository auch mit anderen Arch Linux-basierten Distributionen wie Manjaro verwenden.
:::

## Arch User Repository (AUR)

Alternativ gibt es auch ein offizielles Paket für QOwnNotes auf AUR, es heißt ` qownnotes `.

Sie finden es hier: [ QOwnNotes on AUR ](https://aur.archlinux.org/packages/qownnotes)

Synchronisieren Sie Ihre Paketdatenbank und installieren Sie das Paket mit ` yay `:

```bash
yay -S qownnotes
```

::: tip
Wenn Sie die Erstellungszeit beschleunigen möchten, lesen Sie  CCACHE und AUR </ 0>. :::</p>
