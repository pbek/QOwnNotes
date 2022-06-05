# التثبيت على أوبنتو وإلمنتري ولينكس منت

## أوبنتو 18.04 أو أحدث، إلمنتري، أو لينكس منت 18 أو أحدث

قم بتثبيت **QOwnNotes** على **أوبنتو** (نسخة 18.04 على الأقل) باستخدام المستودع الشخصي (PPA).

لاحظ أن المستودع به أيضا حزم لمعمارية ARM، إذا أردت استخدام QOwnNotes على **Raspberry Pi**.

افتح طرفية، وأدخل السطور التالية فيها لإضافة المستودع وتثبيت QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[تنزيل مباشر](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## النسخ الأقدم من أوبنتو ومشتقاتها

الأفضل تجربة استخدام الـ&nbsp;AppImage من [إصدارات QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes/releases).

عندئذٍ يمكنك جعل الملف قابل للتنفيذ بالأمر:

```bash
chmod a+x QOwnNotes-*.AppImage
```

بعدئذٍ ستتمكن من تنفيذ الـ&nbsp;AppImage لتشغيل QOwnNotes.

## مستودع OBS

ربما تستطيع أيضا استخدام QOwnNotes على نسخ أوبنتو التي لم تعد تستقبل تحديثات في المستودع الشخصي، باستخدام مستودعات [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

إليك أمثلة على استخدام مستودع xUbuntu 16.10.

نفّذ الأمر التالي في الطرفية لاستيثاق المستودع.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

نفّذ الأوامر التالية في الطرفية لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
