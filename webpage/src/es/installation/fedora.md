# Instalar en Fedora Linux

Hay repositorios QOwnNotes para **Fedora 28 y superior**.

## En sistemas con el complemento config-manager de dnf

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Tal vez necesite aceptar la clave del repositorio para poder descargar desde este.

Si tiene problemas, importe la clave usted mismo con:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```
:::

## Método de instalación heredado

Utilice este método si su versión de Fedora no admite el complemento `config-manager` de dnf. Ejecute las órdenes siguientes con permisos administrativos.

Ejecute los siguientes comandos de shell como root para confiar en el repositorio.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36/repodata/repomd.xml.key
```

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

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

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_36) (este enlace de ejemplo es para Fedora 36)
