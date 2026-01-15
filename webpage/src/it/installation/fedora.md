# Installa su Fedora Linux

Esistono repository QOwnNotes per **Fedora 28 e versioni successive**.

::: tip
QOwnNotes is provided upstream in the [Fedora repositories](https://packages.fedoraproject.org/pkgs/qownnotes/qownnotes/). But that version is generally a lot behind the latest release of QOwnNotes.

For most users you can just use `dnf install qownnotes` in a terminal window to install the upstream version. Altrimenti, se si vuole la **versione più aggiornata**, proseguire con la lettura.
:::

## On systems with Fedora 41 and higher

Starting with [Fedora 41](https://fedoraproject.org/wiki/Changes/SwitchToDnf5), dnf5 is the default package manager and includes the config-manager plugin by default. Run the following commands as root to add the repository and install QOwnNotes:

```bash
dnf config-manager add-repo --from-repofile=https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/home:pbek:QOwnNotes.repo

dnf install qownnotes
```

Change the portion `Fedora_42` in the above code with the version of Fedora you are using (i.e. `Fedora_41`, `Fedora_Rawhide` etc.).

## Sui sistemi con plugin dnf config-manager

Eseguire i seguenti comandi shell come root (amministratore) per aggiungere il repository.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
Potrebbe essere necessario accettare la chiave del repository prima di poter scaricarlo da lì.

Se si ha qualche problema si può importare manualmente la chiave con:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Please note that the portion `Fedora_42` in the above code should reflect the version of Fedora you are using (i.e. `Fedora_39`, `Fedora_38` etc.)
:::

## Metodo di installazione legacy

Usare questo metodo nel caso in cui la versione di Fedora non supporta il plugin di dnf `config-manager`, in tal caso eseguire i seguenti comandi come root.

Eseguire il seguente comando shell come root per rendere attendibile il repository.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_42/repodata/repomd.xml.key
```

Again: note that the portion `Fedora_42` in the above code should reflect the version of Fedora you are using (i.e. `Fedora_39`, `Fedora_38` etc.)

Dunque eseguire i seguenti comandi come root per aggiungere il repository e installare QOwnNotes da esso.

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

## Note sull'aggiornamento della versione di QOwnNotes su Fedora

### Problemi con la chiave GPG?

Changes in Fedora's cryptographic policies can mean "old" (expired) repository keys are not _automatically_ extended. This can lead to problems _updating_ QOwnNotes.

**Dettagli:** se si hanno problemi con l'invalidità delle chiavi (es.: Errori GPG) come `certificate is not alive` (il certificato non è disponibile) e/o `key is not alive` (la chiave non è disponibile) a causa della scadenza di essi i seguenti comandi dovrebbero eliminare la chiave scaduta:

```bash
sudo rpm -e $(rpm -q --qf "%{NAME}-%{VERSION}-%{RELEASE}\t%{SUMMARY}\n" gpg-pubkey | grep pbek | cut -f1)
```

La guida dettagliata dei comandi è disponibile su GitHub nel [topic](https://github.com/pbek/QOwnNotes/issues/3008#issuecomment-2197827084) riguardante questa esatta problematica.

Once the expired key has been deleted, you must then newly _import_ the **current** key manually as described in the beginning of these installation instructions.
