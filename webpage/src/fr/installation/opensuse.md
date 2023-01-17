# Installer sur openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->


## Sur toutes les versions d'openSUSE

Vous pouvez installer QOwnNotes à l'aide de l'outil [OBS Package Installer](https://github.com/openSUSE/opi).

Exécutez les commandes shell suivantes en tant que root pour installer `opi` :

```bash
zypper install opi
```

Installez ensuite QOwnNotes avec :

```bash
opi qownnotes
```

::: warning
Cet outil interrogeant l'ensemble du service OBS, assurez-vous de choisir `qownnotes`, et non `qownnotes-lang` si cela vous est demandé.

Vérifiez également que le dépôt choisi est le `home:pbek:QOwnNotes` officiel et non un dépôt tiers.
:::

::: tip
Vous devez activer l'option de conserver le référentiel après l'installation pour obtenir les mises à jour.
:::

## openSUSE Leap 15.4

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt puis installer QOwnNotes.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)


## SLE 12 SP3 Backports

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt puis installer QOwnNotes.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports)

## SLE 15

Exécutez les commandes shell suivantes en tant que root pour ajouter le dépôt puis installer QOwnNotes.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
