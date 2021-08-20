# Installer sur openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->


## Sur chaque version openSUSE

Vous pouvez installer QOwnNotes à l'aide de l'outil [OBS Package Installer](https://github.com/openSUSE/opi).

Exécutez les commandes shell suivantes en tant que root pour installer `opi` :

```bash
zypper install opi
```

Ensuite, installez QOwnNotes avec :

```bash
opi qownnotes
```

::: warning
Cet outil interrogera l'ensemble du service OBS, alors assurez-vous de choisir `qownnotes`, et non `qownnotes-lang` si cela vous est demandé.

Vérifiez également que le dépôt choisi est le `home:pbek:QOwnNotes` officiel et non un tiers.
:::

::: tip
Vous devez choisir l'option de conserver le référentiel après l'installation pour obtenir les mises à jour.
:::

## openSUSE 13.2

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_13.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_13.2)

## openSUSE Leap 15.3

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Leap 15.1

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.1/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.1)

## openSUSE Leap 15.0

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.0/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.0)

## openSUSE Leap 42.3

Exécutez les commandes shell suivantes en tant que root pour approuver le référentiel.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3/repodata/repomd.xml.key
```

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.3)

## openSUSE Leap 42.2

Exécutez les commandes shell suivantes en tant que root pour approuver le référentiel.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2/repodata/repomd.xml.key
```

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.2)

## openSUSE Leap 42.1

Exécutez les commandes shell suivantes en tant que root pour approuver le référentiel.

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1/repodata/repomd.xml.key
```

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_42.1)

## openSUSE Tumbleweed

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)


## SLE 12 SP3 Backports

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports)

## SLE 15

Exécutez les commandes shell suivantes en tant que root pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
