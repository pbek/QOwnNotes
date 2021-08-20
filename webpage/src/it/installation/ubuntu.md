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

## Vecchie distribuzioni e derivati di Ubuntu Linux

È meglio provare a utilizzare l'AppImage da QOwnNotes [Releases](https://github.com/pbek/QOwnNotes/releases) su GitHub.

Quindi puoi modificare i permessi di esecuzione del file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Dopodiché, dovresti essere in grado di eseguire l'AppImage per lanciare QOwnNotes.

## Repository OBS

Potresti anche essere in grado di utilizzare QOwnNotes su versioni di Ubuntu che non sono più aggiornate su Ubuntu Launchpad, quindi puoi utilizzare i repository da [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

Di seguito sono riportati esempi di chiamate per il repository xUbuntu 16.10.

Esegui il seguente comando shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Esegui i seguenti comandi shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
