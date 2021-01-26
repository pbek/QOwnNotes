# نصب در لینوکس فدورا

مخزن های QOwnNotes برای **فدورا 28 و بالاتر** موجود هستند.

فرمان های پوسته زیر را به صورت ریشه برای اطمینان از مخزن اجرا کنید:

```bash
su -
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_33/repodata/repomd.xml.key
```

فرمان های پوسته زیر را برای افزودن مخزن به صورت ریشه اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
cat > /etc/yum.repos.d/QOwnNotes.repo << EOL
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

[بارگیری مستقیم](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Fedora_33) (این پیوند نمونه برای فدورا 33 است)
