# Installer sur Fedora Linux

Il existe des référentiels QOwnNotes pour **Fedora 28 et supérieur**.

## On systems with config-manager dnf plugin

Run the following shell commands as root to add the repository.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
You may need to accept the repo key before you can download from it.

If you have troubles import the key yourself with:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_34/repodata/repomd.xml.key
```
:::

## Méthode d'installation héritée

Verwenden Sie diese Methode, wenn Ihre Fedora-Version das dnf-Plugin `config-manager` nicht unterstützt, führen Sie diese Befehle als root aus.

Exécutez les commandes shell suivantes en tant que root pour approuver le référentiel.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_34/repodata/repomd.xml.key
```

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

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

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_34) (this example link is for Fedora 34)
