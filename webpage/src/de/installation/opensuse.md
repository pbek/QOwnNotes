# Installieren unter openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->

## Auf jeder openSUSE-Version

Sie können QOwnNotes mit dem Tool [OBS Package Installer](https://github.com/openSUSE/opi) installieren.

Führen Sie die folgenden Shell-Befehle als Root aus, um `opi` zu installieren:

```bash
zypper install opi
```

Installieren Sie QOwnNotes dann damit:

```bash
opi qownnotes
```

::: warning
Dieses Tool fragt den gesamten OBS-Dienst ab. Wählen Sie daher `qownnotes` und nicht `qownnotes-lang`, wenn Sie dazu aufgefordert werden.

Überprüfen Sie auch, dass das ausgewählte Repository das offizielle `home:pbek:QOwnNotes` ist und kein Drittanbieter.
:::

::: tip
Sie müssen die Option auswählen, das Repository nach der Installation beizubehalten, um Updates zu erhalten.
:::

## openSUSE Leap 15.6

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

## openSUSE Leap 15.4

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

## SLE 15

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
