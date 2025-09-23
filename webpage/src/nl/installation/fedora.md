# Installatie op Fedora Linux

Er zijn QOwnNotes-repositories voor **Fedora 28 and higher**.

::: tip
QOwnNotes is provided upstream in the [Fedora repositories](https://packages.fedoraproject.org/pkgs/qownnotes/qownnotes/). But that version is generally a lot behind the latest release of QOwnNotes.

For most users you can just use `dnf install qownnotes` in a terminal window to install the upstream version. Als je de **meest recente versie** wenst, lees dan verder.
:::

## Op systemen met config-manager dnf plugin

Voer de volgende shell-opdrachten uit als root om de repository toe te voegen.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Mogelijk moet u de repo-sleutel accepteren voordat u deze kunt downloaden.

Als u problemen heeft, importeert u de sleutel handmatig met:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Please note that the portion `Fedora_42` in the above code should reflect the version of Fedora you are using (i.e. `Fedora_39`, `Fedora_38` etc.)
:::

## Verouderde installatiemethode

Gebruik deze methode als je Fedora-versie de `config-manager` dnf-plug-in niet ondersteunt, voer deze commando's uit als root.

Voer de volgende shell-opdracht uit als root om de opslagplaats te vertrouwen.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Again: note that the portion `Fedora_42` in the above code should reflect the version of Fedora you are using (i.e. `Fedora_39`, `Fedora_38` etc.)

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

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42) (this example link is for Fedora 42)

## QOwnNotes versie-updating notities voor Fedora

### Problemen met GPG-keys?

Changes in Fedora's cryptographic policies can mean "old" (expired) repository keys are not _automatically_ extended. This can lead to problems _updating_ QOwnNotes.

**Detail:** Als u een probleem heeft met ongeldige sleutels (d.w.z. GPG-fouten) zoalss `certificaat is niet geldig` en/of `sleutel is niet geldig` vanwege het verstrijken van de sleutel, moet deze opdracht op de terminal de verlopen sleutel verwijderen:

```bash
sudo rpm -e $(rpm -q --qf "%{NAME}-%{VERSION}-%{RELEASE}\t%{SUMMARY}\n" gpg-pubkey | grep pbek | cut -f1)
```

Gedetailleerde uitleg van de opdracht is beschikbaar op GitHub in een [onderwerp](https://github.com/pbek/QOwnNotes/issues/3008#issuecomment-2197827084) gerelateerd aan dit exacte probleem.

Once the expired key has been deleted, you must then newly _import_ the **current** key manually as described in the beginning of these installation instructions.
