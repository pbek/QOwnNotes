# Installa su Fedora Linux

Esistono repository QOwnNotes per **Fedora 28 e versioni successive**.

## Sui sistemi con plugin dnf config-manager

Esegui i seguenti comandi shell come root per aggiungere il repository.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Potrebbe essere necessario accettare la chiave del repository prima di poter scaricare.

Se hai problemi, importa tu stesso la chiave con:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```
:::

## Metodo di installazione legacy

Usa questo metodo se la tua versione di Fedora non supporta il plugin dnf `config-manager`, esegui questi comandi come root.

Esegui i seguenti comandi shell come root per considerare attendibile il repository.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```

Esegui i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da lì.

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

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36) (questo collegamento di esempio è per Fedora 36)
