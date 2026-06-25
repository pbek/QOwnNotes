# Install on openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->

## On every openSUSE version

You can install QOwnNotes using the [OBS Package Installer](https://github.com/openSUSE/opi) tool.

Run the following shell commands as root to install `opi`:

```bash
zypper install opi
```

Then install QOwnNotes with it:

```bash
opi qownnotes
```

::: warning
This tool will query the entire OBS service so be sure to choose `qownnotes`,
and not `qownnotes-lang` if asked.

Also check that the chosen repo is the official `home:pbek:QOwnNotes` and not a third-party one.
:::

::: tip
You need to choose the option to keep the repository after installation to get updates.
:::

## openSUSE Leap 16.0

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0)

## openSUSE Leap 15.6

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

## openSUSE Leap 15.4

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For GNOME and other Secret Service based desktops:

```bash
sudo zypper install gnome-keyring libsecret-1-0 seahorse
```

For KDE Plasma:

```bash
sudo zypper install kwalletmanager
```

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
