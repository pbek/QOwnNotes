# Installer sur Fedora Linux

Il existe des référentiels QOwnNotes pour **Fedora 28 et supérieur**.

Exécutez les commandes shell suivantes en tant que root pour approuver le référentiel.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_33/repodata/repomd.xml.key
```

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

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

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Fedora_33) (cet exemple de lien est pour Fedora 33)
