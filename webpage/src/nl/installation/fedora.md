# Installatie op Fedora Linux

Er zijn QOwnNotes-repositories voor **Fedora 28 and higher**.

## A config-manager dnf beépülő modullal rendelkező rendszereken

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Mogelijk moet u de repo-sleutel accepteren voordat u deze kunt downloaden.

Als je problemen hebt met het zelf importeren van de sleutel met:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
Houd er rekening mee dat het gedeelte "Fedora_40" in de bovenstaande code uw gebruikte versie van Fedora moet weerspiegelen (d.w.z. "Fedora_39", "Fedora_38" enz.)
:::

## Legacy installatiemethode

Gebruik deze methode als je Fedora-versie de `config-manager` dnf-plug-in niet ondersteunt, voer deze commando's uit als root.

Voer de volgende shell-opdrachten uit als root om de repository te vertrouwen.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
Nogmaals: merk op dat het gedeelte "Fedora_40" in de bovenstaande code uw gebruikte versie van Fedora moet weerspiegelen (d.w.z. "Fedora_39", "Fedora_38" enz.)

Voer vervolgens de volgende shell-opdrachten uit als root om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
cat > /etc/yum.repos.d/QOwnNotes.repo << EOL
[qownnotes]
name=OBS repo for QOwnNotes (Fedora \$releasever - \$basearch)
type=rpm-md
baseurl=http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/
gpgcheck=1
gpgkey=http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/repodata/repomd.xml.key
enabled=1
EOL

dnf clean expire-cache
dnf install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40) (deze voorbeeldlink is voor Fedora 40)
