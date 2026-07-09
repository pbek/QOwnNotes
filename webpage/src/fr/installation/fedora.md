# Installer sur Fedora Linux

Il existe des dépôts QOwnNotes pour **Fedora 28 et ultérieur**.

::: tip
QOwnNotes est disponible directement dans les [dépôts officiels Fedora](https://packages.fedoraproject.org/pkgs/qownnotes/qownnotes/). Mais cette version est généralement très en retard sur la dernière version de QOwnNotes.

La plupart des utilisateurs peuvent simplement utiliser `dnf install qownnotes` dans une fenêtre de terminal pour installer la version officielle. Si en revanche vous souhaitez la **version la plus à jour**, continuez à lire.
:::

## Systèmes avec Fedora 41 et ultérieur

À partir de [Fedora 41](https://fedoraproject.org/wiki/Changes/SwitchToDnf5), dnf5 est le gestionnaire de paquets par défaut et inclut toujours le greffon config-manager. Exécutez les commandes suivantes en tant que root pour ajouter le dépôt puis installer QOwnNotes :

```bash
dnf config-manager addrepo --from-repofile=https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/home:pbek:QOwnNotes.repo

dnf install qownnotes
```

Changez la portion `Fedora_42` dans le code ci-dessus avec la version de Fedora que vous utilisez (c’est-à-dire `Fedora_41`, `Fedora_Rawhide`, etc.).

## Systèmes avec greffon config-manager dnf

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Vous devrez peut-être accepter la clé du dépôt avant de pouvoir y télécharger du contenu.

Si vous rencontrez des problèmes, importez la clé manuellement avec :

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Veuillez noter que la portion `Fedora_42` dans le code ci-dessus doit refléter la version de Fedora que vous utilisez (c’est-à-dire `Fedora_39`, `Fedora_38`, etc.)
:::

## Méthode d'installation obsolète

Utilisez cette méthode si votre version de Fedora ne prend pas en charge pas le greffon dnf `config-manager`.

Exécutez la commande shell suivante en tant que root pour approuver le dépôt.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Encore une fois : veuillez noter que la portion `Fedora_42` dans le code ci-dessus doit refléter la version de Fedora que vous utilisez (c’est-à-dire `Fedora_39`, `Fedora_38`, etc.)

Exécutez ensuite les commandes shell suivantes en tant que root pour ajouter le dépôt puis installer QOwnNotes.

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

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42) (ce lien d’exemple est pour Fedora 42)

## Notes sur le version-updating de QOwnNotes pour Fedora

### Des problèmes avec les clés GPG ?

Des changements de la politique de cryptographie de Fedora font que les « vielles » clés de dépôt (c’est-à-dire les clés expirées) ne sont pas _automatiquement_ étendues. Cela peut poser des problèmes pour _mettre à jour_ QOwnNotes.

**Détails :** si vous avez des problèmes de clés invalides (c’est-à-dire des erreurs GPG) telles que `certificate is not alive` et/ou `key is not alive` à cause d’expiration de clés, cette commande de terminal devrait supprimer la clé expirée :

```bash
sudo rpm -e $(rpm -q --qf "%{NAME}-%{VERSION}-%{RELEASE}\t%{SUMMARY}\n" gpg-pubkey | grep pbek | cut -f1)
```

L’explication détaillée de la commande est disponible sur GitHub sur un [topic](https://github.com/pbek/QOwnNotes/issues/3008#issuecomment-2197827084) lié à ce problème-ci.

Une fois que la clé expirée a été supprimée, vous devez _importer_ la clé **courante** manuellement comme indiqué au début de ces instructions d’installation.

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service :

```bash
sudo dnf install gnome-keyring libsecret seahorse
```

Pour KDE Plasma :

```bash
sudo dnf install kwalletmanager kf6-kwallet
```

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
