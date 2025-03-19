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
Esta herramienta efectuará una consulta en la totalidad del servicio OBS. Por lo tanto, cerciórese de elegir `qownnotes`, y no `qownnotes-lang`, si recibe una pregunta.

Además, compruebe que el repositorio elegido sea el oficial, `home:pbek:QOwnNotes`, y no uno de terceros.
:::

::: tip
Necesita escoger la opción para conservar el repositorio tras la instalación para poder recibir actualizaciones.
:::

## openSUSE Leap 15.6

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

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

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

## SLE 15

Ejecute las órdenes de consola siguientes con permisos administrativos para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
