# Installa su Fedora Linux

Esistono repository QOwnNotes per **Fedora 28 e versioni successive**.

Eseguire i seguenti comandi della shell come root per considerare attendibile il repository.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_33/repodata/repomd.xml.key
```

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da lì.

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

[Download diretto](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Fedora_33) (questo collegamento di esempio è per Fedora 33)
