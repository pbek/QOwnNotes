# Telepítés Fedora Linux rendszerre

Vannak QOwnNotes tárak a **Fedora 28 és újabb verziókhoz**.

Futtassa a következő shell parancsokat rootként, hogy megbízhasson a tárban.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_33/repodata/repomd.xml.key
```

Futtassa rootként a következő shell parancsokat a lerakat hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
cat > /etc/yum.repos.d/QOwnNotes.repo << EOL
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

[Közvetlen letöltés](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Fedora_33) (ez a példa link a Fedora 33-ra vonatkozik)
