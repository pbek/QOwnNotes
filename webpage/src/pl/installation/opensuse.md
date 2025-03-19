# Zainstaluj na openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->

## W każdej wersji openSUSE

Możesz zainstalować QOwnNotes za pomocą narzędzia [OBS Package Installer](https://github.com/openSUSE/opi).

Uruchom następujące polecenia powłoki jako root, aby zainstalować `opi`:

```bash
zypper install opi
```

Następnie zainstaluj z nim QOwnNotes:

```bash
opi qownnotes
```

::: warning
To narzędzie prześle zapytanie do całej usługi OBS, więc pamiętaj, aby wybrać `qownnotes`, a nie `qownnotes-lang`, jeśli zostaniesz o to poproszony.

Sprawdź również, czy wybrane repozytorium jest oficjalnym `home:pbek:QOwnNotes`, a nie innym.
:::

::: tip
Aby uzyskać aktualizacje, musisz wybrać opcję zachowania repozytorium po instalacji.
:::

## openSUSE Leap 15.6

Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium i zainstalować QOwnNotes stamtąd.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

## openSUSE Leap 15.4

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

## SLE 15

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
