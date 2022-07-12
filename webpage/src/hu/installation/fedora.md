# Telepítés Fedora Linux rendszerre

Vannak QOwnNotes tárak a **Fedora 28 és újabb verziókhoz**.

## A config-manager dnf beépülő modullal rendelkező rendszereken

Futtassa a következő shell parancsokat rootként az adattár hozzáadásához.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Előfordulhat, hogy el kell fogadnia a repo kulcsot, mielőtt letölthet róla.

Ha problémái vannak, akkor importálja a kulcsot saját kezüleg:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```
:::

## Régi telepítési módszer

Használja ezt a módszert, ha a Fedora verziója nem támogatja a `config-manager` dnf bővítményt, futtassa ezeket a parancsokat rootként.

Futtassa a következő shell parancsokat rootként, hogy megbízhasson az adattárban.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
cat > /etc/yum.repos.d/QOwnNotes.repo << EOL
[qownnotes]
name=OBS repo for QOwnNotes (Fedora \$releasever - \$basearch)
type=rpm-md
baseurl=http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/
gpgcheck=1
gpgkey=http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/repodata/repomd.xml.key
enabled=1
EOL

dnf clean expire-cache
dnf install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36) (ez a példa hivatkozás a Fedora 36-hoz)
