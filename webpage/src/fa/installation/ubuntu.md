# نصب در لینوکس اوبونتو، المنتری او اس و لینوکس مینت

## لینوکس اوبونتو 18.04 یا جدیدتر، المنتری او اس، لینوکس مینت 18 یا جدیدتر

**QOwnNotes** را در **لینوکس اوبونتو** (حداقل 18.04) با استفاده از مخزن PPA نصب کنید.

لازم به ذکر است در صورت تمایل به استفاده از QOwnNotes در **رزبری پای**، بسته هایی نیز برای ARM در مخزن در نظر گرفته شده اند.

ترمینال را باز کرده و خطوط زیر را برای افزودن مخزن و نصب QOwnNotes وارد نمایید.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[بارگیری مستقیم](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## توزیع ها و مشتقات قدیمی تر لینوکس اوبونتو

بهتر است سعی کنید AppImage را از [انتشار های QOwnNotes در گیت هاب](https://github.com/pbek/QOwnNotes/releases) بکار گیرید.

سپس می توانید مجوزهای اجرا را در پرونده تغییر دهید:

```bash
chmod a+x QOwnNotes-*.AppImage
```

پس از آن حتماً قادر به اجرای AppImage برای استفاده از QOwnNotes خواهید بود.

## مخزن OBS

بعلاوه، این امکان را دارید که از QOwnNotes در نسخه هایی از اوبونتو که دیگر در لانچ پد اوبونتو بروزرسانی نمی شوند، استفاده نمایید؛ در این صورت می توانید از مخازن مربوط به [سرویس ساخت اوپن سوزه](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop) بهره گیرید.

در زیر چند نمونه از دستورات برای مخزن زوبونتو 16.10 ذکر شده است.

فرمان پوسته زیر را برای اطمینان از مخزن اجرا کنید.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

فرمان های پوسته زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[بارگیری مستقیم](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/xUbuntu_16.10)
