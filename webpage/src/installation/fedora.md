# Install on Fedora Linux

There are QOwnNotes repositories for **Fedora 28 and higher**.

## On systems with config-manager dnf plugin

Run the following shell commands as root to add the repository.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
You may need to accept the repo key before you can download from it.

If you have troubles import the key yourself with:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
Please note that the portion "Fedora_40" in the above code should reflect the version of Fedora you are using (i.e. "Fedora_39", "Fedora_38" etc.)
:::

## Legacy install method

Use this method if your Fedora version doesn't support the `config-manager` dnf plugin, run these commands as root.

Run the following shell commands as root to trust the repository.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
Again: note that the portion "Fedora_40" in the above code should reflect the version of Fedora you are using (i.e. "Fedora_39", "Fedora_38" etc.)

Then run the following shell commands as root to add the repository and install QOwnNotes from there.

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

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40) (this example link is for Fedora 40)

## QON Version Updating Notes for Fedora
### Problems with GPG Keys?

Changes in Fedora's cryptographic policies can mean "old" repository keys are not automatically extended.  This can lead to problems _updating_ QOwnNotes.

If you have a problem with invalid keys (i.e. GPG errors) such as "certificate is not alive" and/or "key is not alive" due to key expiry, this terminal command should delete the expired key:

```sudo rpm -e $(rpm -q --qf "%{NAME}-%{VERSION}-%{RELEASE}\t%{SUMMARY}\n" gpg-pubkey | grep pbek | cut -f1)```

You must then newly _import_ the current key as described in the beginning of these installation instructions.

Detailed explanation of the command is available on GitHub in a [topic](https://github.com/pbek/QOwnNotes/issues/3008#issuecomment-2197827084) related to this exact issue.
