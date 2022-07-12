# Installer sur Fedora Linux

Il existe des dépôts QOwnNotes pour **Fedora 28 et supérieur**.

## Systèmes avec greffon config-manager dnf

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Vous devrez peut-être accepter la clef du dépôt avant de pouvoir y télécharger du contenu.

Si vous rencontrez des problèmes, importez vous-même la clé avec :

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```
:::

## Méthode d'installation obsolète

Utilisez cette méthode si votre version de Fedora ne comporte pas le greffon dnf `config-manager`.

Exécutez les commandes shell suivantes en tant que root pour approuver le dépôt.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt et installer QOwnNotes à partir de là.

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

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36) (cet exemple de lien est pour Fedora 36)
