# Installa su Ubuntu Linux, Elementary OS e Linux Mint

## Ubuntu Linux 18.04 o più recente, Elementary OS, Linux Mint 18 o più recente

Installa **QOwnNotes** su **Ubuntu Linux** (minimo 18.04) utilizzando il repository PPA.

Nota che ci sono anche pacchetti per ARM nel repository, nel caso tu voglia usare QOwnNotes su un **Raspberry Pi**.

Apri un terminale e inserisci le seguenti righe per aggiungere il repository e installare QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Anteprima QOwnNotes Qt6

Se vuoi già provare i pacchetti Qt6 e sei su Ubuntu 22.04 o più recente, puoi usare questo repository:

```bash
# Rimuovi Qt5 PPA nel caso sia installato
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Aggiungi Qt6 PPA
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Installa QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

Nel caso ci fossero problemi si prega di segnalarli sulla sezione [Issues di QOwnNotes su GiHub](https://github.com/pbek/QOwnNotes/issues)!

## Vecchie distribuzioni e derivati di Ubuntu Linux

In questo caso è più conveniente usare la versione AppImage da [QOwnNotes Releases su GitHub](https://github.com/pbek/QOwnNotes/releases).

Dopo aver scaricato il file AppImage è necessario modificare i permessi di quest ultimo per poterlo eseguire:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Successivamente dovresti essere in grado di eseguire l'AppImage per avviare QOwnNotes.

## Repository OBS

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

Below are example calls for the xUbuntu 20.04 repository.

Eseguire il seguente comando shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/Release.key -O - | sudo apt-key add -
```

Eseguire i seguenti comandi shell per aggiungere il repository e installare QOwnNotes direttamente da esso.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04)
