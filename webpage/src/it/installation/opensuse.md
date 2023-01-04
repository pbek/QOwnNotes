# Installa su openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->


## Su tutte le versioni openSUSE

Puoi installare QOwnNotes usando lo strumento [OBS Package Installer](https://github.com/openSUSE/opi)

Esegui i seguenti comandi della shell come root per installare `opi`:

```bash
zypper install opi
```

E poi installa QOwnNotes con questo:

```bash
opi qownnotes
```

Lo strumento interrogherà l'intero servizio OBS per essere sicuro di scegliere `qownnotes`, e non `qownnotes-lang` se richiesto.

Controlla anche che il repository scelto sia quello ufficiale `home:pbek:QOwnNotes` e non uno di terze parti.
:::

::: tip
È necessario scegliere l'opzione per mantenere il repository dopo l'installazione per ottenere gli aggiornamenti.
:::

## openSUSE Leap 15.4

Esegui i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da lì.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Esegui i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da lì.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da lì.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

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

Esegui i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da lì.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
