# Zainstaluj na Raspberry Pi OS

## Raspberry Pi OS 11

Uruchom następujące polecenia powłoki, aby zaufać repozytorium.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/Release.key -O - | sudo apt-key add -
```

Uruchom następujące polecenia powłoki, aby dodać repozytorium i zainstalować z niego QOwnNotes.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list "
aktualizacja sudo apt-get
sudo apt-get install qownnotes
```

[Pobieranie bezpośrednie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

## System operacyjny Raspberry Pi 10

Uruchom następujące polecenia powłoki, aby zaufać repozytorium.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Uruchom następujące polecenia powłoki, aby dodać repozytorium i zainstalować z niego QOwnNotes.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Bezpośrednie pobieranie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10)

## Raspbian 9.0

Uruchom następujące polecenia powłoki, aby zaufać repozytorium.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Uruchom następujące polecenia powłoki, aby dodać repozytorium i zainstalować z niego QOwnNotes.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Pobieranie bezpośrednie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0)
