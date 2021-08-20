# Installieren unter openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->


## Auf jeder openSUSE-Version

Sie können QOwnNotes mit dem Tool [OBS Package Installer](https://github.com/openSUSE/opi) installieren.

Führen Sie die folgenden Shell-Befehle als Root aus, um `opi` zu installieren:

```bash
zypper install opi
```

Dann QOwnNotes damit installieren:

```bash
opi qownnotes
```

::: warning
Dieses Tool fragt den gesamten OBS-Dienst ab. Wählen Sie daher `qownnotes` und nicht `qownnotes-lang`, wenn Sie dazu aufgefordert werden.

Überprüfen Sie auch, ob das ausgewählte Repository das offizielle `home:pbek:QOwnNotes` ist und kein Drittanbieter.
:::

::: tip
Sie müssen die Option auswählen, das Repository nach der Installation beizubehalten, um Updates zu erhalten.
:::

## openSUSE 13.2

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_13.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_13.2)

## openSUSE Leap 15.3

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Leap 15.1

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.1/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.1)

## openSUSE Leap 15.0

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.0/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.0)

## openSUSE Leap 42.3

Führen Sie die folgenden Shell-Befehle als root aus, um dem Repository zu vertrauen.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3/repodata/repomd.xml.key
```

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3)

## openSUSE Leap 42.2

Führen Sie die folgenden Shell-Befehle als root aus, um dem Repository zu vertrauen.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2/repodata/repomd.xml.key
```

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2)

## openSUSE Leap 42.1

Führen Sie die folgenden Shell-Befehle als root aus, um dem Repository zu vertrauen.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1/repodata/repomd.xml.key
```

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1)

## openSUSE Tumbleweed

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)


## SLE 12 SP3 Backports

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports)

## SLE 15

Führen Sie die folgenden Shell-Befehle als root aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
