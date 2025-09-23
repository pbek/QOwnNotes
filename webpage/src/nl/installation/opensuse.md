# Installatie op openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->

## Minden openSUSE verzión

U kunt QOwnNotes installeren met de tool [OBS Package Installer](https://github.com/openSUSE/opi).

Voer de volgende shell-opdrachten uit als root om `opi` te installeren:

```bash
zypper install opi
```

Installeer dan QOwnNotes ermee:

```bash
opi qownnotes
```

::: warning
Deze tool zal de hele OBS-service doorzoeken, dus zorg ervoor dat u `qownnotes` kiest en niet `qownnotes-lang` als daarom wordt gevraagd.

Controleer ook of de gekozen repo de officiële `home:pbek:QOwnNotes` is en niet die van een derde partij.
:::

::: tip
U moet de optie kiezen om de repository na installatie te behouden om updates te krijgen.
:::

## openSUSE Leap 16.0

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0)

## openSUSE Leap 15.6

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

## openSUSE Leap 15.4

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

## SLE 15

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
