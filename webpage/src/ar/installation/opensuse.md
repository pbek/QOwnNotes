# التثبيت على openSUSE Linux

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->

## جميع نسخ أوبن سوزي

يمكنك تثبيت QOwnNotes باستخدام أداة [مثبت حزم OBS](https://github.com/openSUSE/opi).

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لتثبيت `opi`:

```bash
zypper install opi
```

ثم ثبّت QOwnNotes باستخدامه:

```bash
opi qownnotes
```

::: warning
ستسأل هذه الأداة خدمة OBS بكاملها، لذا تأكد من أن تختار `qownnotes` وليس `qownnotes-lang` إذا سئلت.

تأكد أيضا من أن المستودع المختار هو الرسمي `home:pbek:QOwnNotes` وليس مستودع طرف ثالث.
:::

::: tip
تحتاج إلى تحديد خيار الإبقاء على المستودع بعد التثبيت للحصول على التحديثات.
:::

## openSUSE Leap 15.6

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

## openSUSE Leap 15.4

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

## SLE 15

نفّذ الأوامر التالية في الطرفية بصلاحيات الجذر لإضافة المستودع وتثبيت QOwnNotes منه.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
