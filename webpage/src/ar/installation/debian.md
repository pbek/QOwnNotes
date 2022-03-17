# التثبيت على دبيان

## دبيان 11.0

نفّذ الأوامر التالية في الطرفية لاستيثاق المستودع.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key -O - | sudo apt-key add -
```

نفّذ الأوامر التالية في الطرفية لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip إذا استخدمت هذا المستودع مع نسخة أخرى من دبيان، فنرجو التأكد من أن **Qt** **النسخة 5.11** على الأقل مثبتة لديك. :::

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## دبيان 10.0

نفّذ الأوامر التالية في الطرفية لاستيثاق المستودع.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

نفّذ الأوامر التالية في الطرفية لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip إذا استخدمت هذا المستودع مع نسخة أخرى من دبيان، فنرجو التأكد من أن **Qt** **النسخة 5.8** على الأقل مثبتة لديك. :::

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## دبيان 9.0

نفّذ الأوامر التالية في الطرفية لاستيثاق المستودع.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

نفّذ الأوامر التالية في الطرفية لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip إذا استخدمت هذا المستودع مع نسخة أخرى من دبيان، فنرجو التأكد من أن **Qt** **النسخة 5.5** على الأقل مثبتة لديك. :::

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
