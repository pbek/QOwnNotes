# التثبيت على فيدورا

توجد مستودعات QOwnNotes لتوزيعة **فيدورا 28 والأحدث**.

## على الأنظمة ذات إضافة config-manager لـ&nbsp;dnf

نفّذ الأمر التالي في الطرفية بصلاحيات الجذر لإضافة المستودع.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
ربما تحتاج إلى قبول مفتاح المستودع قبل التنزيل منه.

إذا واجهت مشاكل، يمكنك استيراد المفتاح بنفسك بالأمر:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
لاحظ أن "Fedora_40" في الأمر السابق يجب أن تغيّرها إلى ما يناسب نسخة توزيعتك (أي "Fedora_39" أو "Fedora_38" إلخ).
:::

## على الأنظمة الأقدم

استخدم هذه الطريقة إذا كانت نسختك من فيدورا لا تدعم إضافة `config-manager` لـ&nbsp;dnf.

نفّذ الأمر التالي في الطرفية بصلاحيات الجذر لاستيثاق المستودع.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
مجددا، لاحظ أن "Fedora_40" في الأمر السابق يجب أن تغيّرها إلى ما يناسب نسخة توزيعتك (أي "Fedora_39" أو "Fedora_38" إلخ).

ثم نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

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

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40) (هذا الرابط مثال على فيدورا 40)
