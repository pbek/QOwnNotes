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

## openSUSE Leap 16.0

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da lì.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0)

## openSUSE Leap 15.6

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da esso.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da esso.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

## openSUSE Leap 15.4

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da esso.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da esso.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da esso.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Eseguire i seguenti comandi della shell come root per aggiungere il repository e installare QOwnNotes da esso.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

## SLE 15

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
