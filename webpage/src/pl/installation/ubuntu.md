# Instalacja w systemie Ubuntu Linux, elementary OS oraz Linux Mint

## Ubuntu Linux 18.04 lub nowszy, elementary OS, Linux Mint 18 lub nowszy

Instalowanie **QOwnNotes** w systemie **Ubuntu Linux** (minimum 18.04) przy użyciu repozytorium PPA.

W repozytorium dostępne są także pakiety dla ARM, jeżeli chciałbyś korzystać z QOwnNotes na **Raspberry Pi**.

Otwórz terminal i wprowadź następujące linie aby dodać repozytorium i zainstalować QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Pobieranie bezpośrednie](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Starsze dystrybucje Ubuntu Linux i pochodne

Najlepiej wypróbuj AppImage z [wydań QOwnNotes w serwisie GitHub](https://github.com/pbek/QOwnNotes/releases).

Następnie należy ustawić uprawnienia do uruchamiania pliku:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Po wykonaniu tych czynności powinieneś mieć możliwość uruchomienia AppImage aby włączyć QOwnNotes.

## Repozytorium OBS

Możesz także mieć możliwość używania QOwnNotes w niewspieranych już przez Ubuntu Launchpad wersjach Ubuntu korzystając z repozytoriów [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

Poniżej znajdują się przykładowe wywołania dla repozytorium xUbuntu 16.10.

Uruchom następujące polecenie powłoki, aby zaufać repozytorium.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Uruchom następujące polecenia powłoki, aby dodać repozytorium i zainstalować z niego QOwnNotes.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Bezpośrednie pobieranie](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
