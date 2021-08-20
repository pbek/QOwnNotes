# Install on openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->


## Minden openSUSE verzión

A QOwnNotes az [OBS Package Installer](https://github.com/openSUSE/opi) eszközzel telepíthető.

Futtassa a következő shell parancsokat rootként az `opi` telepítéséhez:

```bash
zypper install opi
```

Then install QOwnNotes with it:

```bash
opi qownnotes
```

::: warning
Ez az eszköz lekérdezi a teljes OBS szolgáltatást, ezért feltétlenül válassza a `qownnotes` lehetőséget, és ne a `qownnotes-lang` lehetőséget.

Ellenőrizze azt is, hogy a kiválasztott repo a hivatalos `home:pbek:QOwnNotes` és nem harmadik féltől származik-e.
:::

::: tip
A frissítések megszerzéséhez ki kell választania az adattár telepítés utáni megőrzését.
:::

## openSUSE 13.2

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_13.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_13.2)

## openSUSE Leap 15.3

Futtassa rootként a következő shell parancsokat a lerakat hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Leap 15.1

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.1/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.1)

## openSUSE Leap 15.0

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.0/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.0)

## openSUSE Leap 42.3

Futtassa a következő shell parancsokat rootként, hogy megbízhasson az adattárban.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3/repodata/repomd.xml.key
```

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3)

## openSUSE Leap 42.2

Futtassa a következő shell parancsokat, hogy megbízhasson az adattárban.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2/repodata/repomd.xml.key
```

Rootként futtassa a következő shell parancsokat az adattár hozzáadásához és onnan a QOwnNotes telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2)

## openSUSE Leap 42.1

Futtassa a következő shell parancsokat rootként, hogy megbízhasson az adattárban.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1/repodata/repomd.xml.key
```

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1)

## openSUSE Tumbleweed

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)


## SLE 12 SP3 Hátlapok

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports)

## SLE 15

Futtassa rootként a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
