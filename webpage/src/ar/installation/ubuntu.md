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

## معاينة نسخة Qt6 من QOwnNotes

If you already want to try the Qt6 packages, built with cmake and are on Ubuntu 24.04 or newer, you can use this repository:

```bash
# أزل المستودع الشخصي الخاص بنسخة كيوت 5 إذا كان لديك
sudo add-apt-repository --remove ppa:pbek/qownnotes

# أضف المستدوع الشخصي الخاص بنسخة كيوت 6
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# وأخيرا تثبيت QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[تنزيل مباشر](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## النسخ الأقدم من أوبنتو ومشتقاتها

الأفضل تجربة استخدام الـ&nbsp;AppImage من [إصدارات QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes/releases).

عندئذٍ يمكنك جعل الملف قابلا للتنفيذ:

```bash
chmod a+x QOwnNotes-*.AppImage
```

بعدئذٍ ستتمكن من تنفيذ الـ&nbsp;AppImage لتشغيل QOwnNotes.

## مستودع OBS

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). The packages for Ubuntu 24.04+ are built with cmake and Qt6.

Below are example calls for the xUbuntu 24.04 repository.

نفّذ الأمر التالي في الطرفية لاستيثاق المستودع.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

نفّذ الأوامر التالية في الطرفية لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
