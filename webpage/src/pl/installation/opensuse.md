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

## openSUSE Leap 15.4

Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium i zainstalować QOwnNotes stamtąd.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
błyskawiczne odświeżenie
Zypper zainstalować qownnotes
```

[Pobieranie bezpośrednie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium i zainstalować QOwnNotes stamtąd.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Bezpośrednie pobieranie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium i zainstalować QOwnNotes stamtąd.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Bezpośrednie pobieranie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)


## SLE 12 SP3 Backports

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports)

## SLE 15

Uruchom następujące polecenia powłoki jako root, aby dodać repozytorium i zainstalować QOwnNotes stamtąd.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Bezpośrednie pobieranie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
