# Instalar en openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->


## En cualquier versión de openSUSE

Es posible instalar QOwnNotes a través de la herramienta [Instalador de paquetes OBS](https://github.com/openSUSE/opi).

Ejecute las órdenes de consola siguientes con permisos administrativos para instalar `opi`:

```bash
zypper install opi
```

A continuación, instale QOwnNotes con:

```bash
opi qownnotes
```

::: warning
Esta herramienta efectuará una consulta en la totalidad del servicio OBS, así que cerciórese de elegir `qownnotes`, y no `qownnotes-lang`, si recibe una pregunta.

Además, compruebe que el repositorio elegido sea el oficial, `home:pbek:QOwnNotes`, y no uno de terceros.
:::

::: tip
Marque la opción para conservar el repositorio tras la instalación y así poder recibir actualizaciones.
:::

## openSUSE Leap 15.4

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Ejecute los siguientes comandos de shell como root para agregar el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga Directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)


## SLE 12 SP3 Backports

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports)

## SLE 15

Ejecute los siguientes comandos de shell como root para agregar el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga Directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
