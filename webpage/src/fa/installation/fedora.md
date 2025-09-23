# نصب در لینوکس فدورا

مخزن های QOwnNotes برای **فدورا 28 و بالاتر** موجود هستند.

::: tip
QOwnNotes is provided upstream in the [Fedora repositories](https://packages.fedoraproject.org/pkgs/qownnotes/qownnotes/). But that version is generally a lot behind the latest release of QOwnNotes.

For most users you can just use `dnf install qownnotes` in a terminal window to install the upstream version. If you want the **most up-to-date version**, please continue reading.
:::

## در سیستم‌های دارای پلاگین config-manager dnf

Run the following shell commands as root to add the repository.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
You may need to accept the repo key before you can download from it.

If you have any problems, import the key manually with:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Please note that the portion `Fedora_42` in the above code should reflect the version of Fedora you are using (i.e. `Fedora_39`, `Fedora_38` etc.)
:::

## روش نصب پیشینه

Use this method if your Fedora version doesn't support the `config-manager` dnf plugin, run these commands as root.

Run the following shell command as root to trust the repository.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Again: note that the portion `Fedora_42` in the above code should reflect the version of Fedora you are using (i.e. `Fedora_39`, `Fedora_38` etc.)

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

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42) (this example link is for Fedora 42)

## QOwnNotes version-updating notes for Fedora

### Problems with GPG keys?

Changes in Fedora's cryptographic policies can mean "old" (expired) repository keys are not _automatically_ extended. This can lead to problems _updating_ QOwnNotes.

**Detail:** If you have a problem with invalid keys (i.e. GPG errors) such as `certificate is not alive` and/or `key is not alive` due to key expiry, this terminal command should delete the expired key:

```bash
sudo rpm -e $(rpm -q --qf "%{NAME}-%{VERSION}-%{RELEASE}\t%{SUMMARY}\n" gpg-pubkey | grep pbek | cut -f1)
```

Detailed explanation of the command is available on GitHub in a [topic](https://github.com/pbek/QOwnNotes/issues/3008#issuecomment-2197827084) related to this exact issue.

Once the expired key has been deleted, you must then newly _import_ the **current** key manually as described in the beginning of these installation instructions.
