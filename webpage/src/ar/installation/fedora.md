# التثبيت على فيدورا

توجد مستودعات QOwnNotes لتوزيعة **فيدورا 28 والأحدث**.

::: tip
يتوفر QOwnNotes في [مستودعات فيدورا](https://packages.fedoraproject.org/pkgs/qownnotes/qownnotes/). لكن النسخة المتاحة فيها غالبا تكون أقدم رقعةً أو رقعتين من نسخة المستودع المتاحة عبر الشرح التالي.

يمكن لمعظم المستخدمين أن ينفذوا الأمر `dnf install qownnotes` في الطرفية. أما إذا أردت **أحدث نسخة مطلقًا**، فرجاءً تابع القراءة.
:::

## على الأنظمة ذات إضافة config-manager لـ&nbsp;dnf

نفّذ الأمر التالي في الطرفية بصلاحيات الجذر لإضافة المستودع.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: tip
ربما تحتاج إلى قبول مفتاح المستودع قبل التنزيل منه.

إذا واجهت مشاكل، يمكنك استيراد المفتاح يدويا بالأمر:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```

لاحظ أن "Fedora_40" في الأمر السابق يجب أن تغيّرها إلى ما يناسب نسخة توزيعتك (أي "Fedora_39" أو "Fedora_38" إلخ).
:::

## على الأنظمة الأقدم

استعمل هذه الطريقة إذا كانت نسختك من فيدورا لا تدعم إضافة `config-manager` لـ&nbsp;dnf.

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

## ملاحظات بخصوص ترقية QOwnNotes على فيدورا

### مشاكل مع مفاتيح GPG؟

Changes in Fedora's cryptographic policies can mean "old" (expired) repository keys are not _automatically_ extended. This can lead to problems _updating_ QOwnNotes.

**التفصيل:** إذا واجهت مشكلة مع المفاتيح التالفة (أيْ أخطاء GPG) بسبب انتهاء المفتاح، مثل `certificate is not alive` أو `key is not alive` أو كليهما، فإن تنفيذ هذا الأمر في الطرفية سيحذف المفتاح المنتهي:

```bash
sudo rpm -e $(rpm -q --qf "%{NAME}-%{VERSION}-%{RELEASE}\t%{SUMMARY}\n" gpg-pubkey | grep pbek | cut -f1)
```

ستجد شرحًا مفصلًا لهذا الأمر في [نقاش على GitHub](https://github.com/pbek/QOwnNotes/issues/3008#issuecomment-2197827084) بخصوص هذه العلة نفسها.

Once the expired key has been deleted, you must then newly _import_ the **current** key manually as described in the beginning of these installation instructions.
