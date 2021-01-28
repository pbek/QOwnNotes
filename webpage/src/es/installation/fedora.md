# Install on Fedora Linux

Hay repositorios QOwnNotes para **Fedora 28 y superior**.

Ejecute los siguientes comandos de shell como root para confiar en el repositorio.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_33/repodata/repomd.xml.key
```

Ejecute los siguientes comandos de shell como root para agregar el repositorio e instalar QOwnNotes desde allí.

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

[Descarga directa](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Fedora_33) (este enlace de ejemplo es para Fedora 33)
