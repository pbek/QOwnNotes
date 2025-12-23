# التثبيت على آرتش لينكس

## مستودع مستخدمي آرتش (AUR)

بدلا من استخدام pacman، توجد أيضا حزمة رسمية من QOwnNotes في AUR؛ تسمى `qownnotes`.

ستجدها هنا: [QOwnNotes على AUR](https://aur.archlinux.org/packages/qownnotes)

قم بمزامنة قاعدة بيانات الحزم وتثبيت الحزمة باستخدام `yay`:

```bash
yay -S qownnotes
```

::: tip
إذا أردت تسريع عملية البناء، ربما تحب قراءة [CCACHE and AUR (بالإنجليزية)](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/).
:::

## pacman

أضف السطور التالية إلى ملف `/etc/pacman.conf`لديك بأمر التحرير `sudo nano /etc/pacman.conf`:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

نفّذ الأوامر التالية في الطرفية لاستيثاق المستودع:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

If the command `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` fails with a message like: `ERROR: FFC43FC94539B8B0 could not be locally signed.`, you could first find out the actual _keyid_ of the downloaded key, i.e. with the command (and output):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: WARNING: no command supplied.  Trying to guess what you mean ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

يمكنك الآن مزامنة قاعدة بيانات الحزم وتثبيت الحزمة باستخدام `pacman`:

```bash
sudo pacman -Syy qownnotes
```

[تنزيل مباشر](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra)

::: tip
بالطبع يمكنك أيضا استخدام هذا المستودع مع التوزيعات الأخرى المبنية على آرتش، مثل مانجارو.
:::
