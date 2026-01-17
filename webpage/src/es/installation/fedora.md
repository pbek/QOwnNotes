# Instalar en Fedora Linux

Hay repositorios de QOwnNotes para **Fedora 28 y superior**.

::: tip
QOwnNotes se proporciona en los [repositorios de Fedora](https://packages.fedoraproject.org/pkgs/qownnotes/qownnotes/). Pero esa versión usualmente está muy por detrás del último lanzamiento de QOwnNotes.

Para la mayoría de los usuarios, basta con utilizar `dnf install qownnotes` en una ventana de terminal para instalar la versión. Si desea la **versión más actualizada**, por favor continúe leyendo.
:::

## En sistemas con Fedora 41 y superior

A partir de [Fedora 41](https://fedoraproject.org/wiki/Changes/SwitchToDnf5), dnf5 es el administrador de paquetes predeterminado e incluye el complemento config-manager de forma predeterminada. Ejecute las órdenes siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes:

```bash
dnf config-manager add-repo --from-repofile=https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/home:pbek:QOwnNotes.repo

dnf install qownnotes
```

Cambie la parte `Fedora_42` en el código anterior con la versión de Fedora que esté utilizando (es decir, `Fedora_41`, `Fedora_Rawhide`, etc.).

## En sistemas con el complemento config-manager de dnf

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Tal vez necesite aceptar la clave del repositorio antes de poder descargar desde este.

Si tiene problemas, importe la clave por su propia cuenta con:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Por favor tenga en cuenta que la porción `Fedora_42` en el código anterior debe reflejar la versión de Fedora que está utilizando (por ejemplo `Fedora_39`, `Fedora_38` etc.)
:::

## Método de instalación heredado

Utilice este método si su versión de Fedora no admite el complemento `config-manager` de dnf. Ejecute estos comandos como root.

Ejecute la orden de consola siguiente con privilegios administrativos para marcar el repositorio como de fiar.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Nuevamente, tenga en cuenta que la porción `Fedora_42` en el código anterior debe reflejar la versión de Fedora que está utilizando (por ejemplo `Fedora_39`, `Fedora_38` etc.)

Tras ello, ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

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

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42) (este enlace de ejemplo es para Fedora 42)

## Notas sobre la actualización de versiones de QOwnNotes para Fedora

### ¿Problemas con claves GPG?

Los cambios en las políticas criptográficas de Fedora pueden significar que las claves "antiguas" (expiradas) del repositorio no son extendidas _automáticamente_. Esto puede provocar problemas al _actualizar_ QOwnNotes.

**Detalle:** Si tiene un problema con claves inválidas (por ejemplo errores GPG) como que el `certificado no esté activo` y/o que la `clave no esté activa` debido a su expiración, este comando de terminal debería eliminar la clave expirada:

```bash
sudo rpm -e $(rpm -q --qf "%{NAME}-%{VERSION}-%{RELEASE}\t%{SUMMARY}\n" gpg-pubkey | grep pbek | cut -f1)
```

La explicación detallada del comando se encuentra disponible en GitHub en un [tema](https://github.com/pbek/QOwnNotes/issues/3008#issuecomment-2197827084) relacionado con esta misma incidencia.

Una vez que la clave expirada haya sido eliminada, debe _importar_ nuevamente la clave **actual** de forma manual, tal y como se describe al inicio de estas instrucciones de instalación.
