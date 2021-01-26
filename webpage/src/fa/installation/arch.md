# نصب در آرچ لینوکس

## پک من

خطوط زیر را به `/etc/pacman.conf` خود با `sudo nano /etc/pacman.conf` اضافه کنید:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

اگر فرمان `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` با پیامی مثل: `خطا: FFC43FC94539B8B0 به صورت محلی قابل پاراف شدن نیست.` خطا داد، نخست می توانید از *keyid* حقیقی کلید بارگیری شده یعنی با فرمان (و خروجی) آگاه شوید:

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: اخطار: فرمانی فراهم نشد.  در تلاش برای حدس زدن منظور شما ...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

هم اکنون می توانید پایگاه داده بسته خود را همگان سازی کرده و آن بسته را با  `پک من` نصب کنید:

```bash
sudo pacman -Syy qownnotes
```

[بارگیری مستقیم](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Arch_Extra)

::: tip
البته همچنین می توانید با سایر توزیع های مبتنی بر آرچ لینوکس نظیر مانجارو از این مخزن استفاده نمایید.
:::

## مخزن کاربر آرچ (AUR)

از سوی دیگر، یک بسته رسمی نیز برای QOwnNotes در AUR با نام `qownnotes` وجود دارد.

شما آن را در اینجا خواهید یافت: [QOwnNotes در AUR](https://aur.archlinux.org/packages/qownnotes)

پایگاه داده بسته خود را همگان سازی کرده و آن بسته را با  `yay` نصب کنید:

```bash
yay -S qownnotes
```

::: tip
در صورت تمایل به سرعت بخشیدن به تخمین زمان شاید بخواهید [CCACHE و AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/) را مطالعه نمایید.
:::
