# نصب در لینوکس فدورا

مخزن های QOwnNotes برای **فدورا 28 و بالاتر** موجود هستند.

## در سیستم‌های دارای پلاگین config-manager dnf

فرمان های پوسته زیر را به صورت ریشه برای افزودن مخزن اجرا کنید.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
شما احتمالاً باید کلید مخزن را قبل از اینکه قادر به بارگیری از آن باشید، بپذیرید.

در صورت مواجه شدن با مشکل، کلید را با این کد وارد کنید:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
Please note that the portion "Fedora_40" in the above code should reflect the version of Fedora you are using (i.e. "Fedora_39", "Fedora_38" etc.)
:::

## روش نصب پیشینه

در صورتی که نسخه فدورای شما از پلاگین `config-manager` dnf پشتیبانی نمی کند، از این روش استفاده کرده و این دستورات را به صورت ریشه اجرا کنید.

فرمان های پوسته ای زیر را به صورت ریشه برای اطمینان از مخزن اجرا کنید.

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
