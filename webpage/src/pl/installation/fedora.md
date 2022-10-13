# Zainstaluj w Fedorze Linux

Istnieją repozytoria QOwnNotes dla **Fedory 28 i nowszych**.

## W systemach z wtyczką dnf menedżera konfiguracji

Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium.

Jeśli masz problemy, zaimportuj klucz samodzielnie za pomocą:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```
:::

## Starsza metoda instalacji

Użyj tej metody, jeśli twoja wersja Fedory nie obsługuje wtyczki dnf `config-manager`, uruchom te polecenia jako root.

Uruchom następujące polecenia powłoki jako root, aby zaufać repozytorium.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```

Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium i zainstalować QOwnNotes stamtąd.

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

[Bezpośrednie pobieranie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36) (ten przykładowy link dotyczy Fedory 36)
