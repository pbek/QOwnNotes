# Zainstaluj na Debian Linux

## Debian 11.0

Uruchom poniższe komendy powłoki, aby dodać repozytorium jako zaufane.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key -O - | sudo apt-key add -
```

Uruchom poniższe komendy powłoki, aby dodać repozytorium i zainstalować z niego QOwnNotes.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Jeśli używasz tego repozytorium dla innych wersji Debiana Linux, upewnij się, że masz zainstalowane **Qt** co najmniej w **wersji 5.11**.
:::

[Pobierz bezpośrednio](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Uruchom poniższe komendy powłoki, aby dodać repozytorium jako zaufane.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Uruchom poniższe komendy powłoki, aby dodać repozytorium i zainstalować z niego QOwnNotes.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Jeśli używasz tego repozytorium dla innych wersji Debiana Linux, upewnij się, że masz zainstalowane **Qt** co najmniej w **wersji 5.8**.
:::

[Pobierz bezpośrednio](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

Uruchom poniższe komendy powłoki, aby dodać repozytorium jako zaufane.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Uruchom poniższe komendy powłoki, aby dodać repozytorium i zainstalować z niego QOwnNotes.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: wskazówka Jeśli używasz tego repozytorium w innych wersjach Debiana, upewnij się, że **Qt** jest zainstalowane **w wersji 5.5** lub wyższej
:::

[Pobierz bezpośrednio](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
