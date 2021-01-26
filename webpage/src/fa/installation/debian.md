# نصب در لینوکس دبیان

## دبیان 10.0

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

فرمان های پوسته زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
در صورتی که از این مخزن برای سایر نسخه های لینوکس دبیان استفاده می کنید، لطفاً مطمئن شوید که حداقل **نسخه 5.8** از **Qt** را نصب کرده اید.
:::

[بارگیری مستقیم](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_10)

## دبیان 9.0

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

فرمان های پوسته زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
در صورتی که از این مخزن برای سایر نسخه های لینوکس دبیان استفاده می کنید، لطفاً مطمئن شوید که حداقل **نسخه 5.5** از **Qt** را نصب کرده اید.
:::

[بارگیری مستقیم](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_9.0)

## دبیان 8.0

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/Release.key -O - | sudo apt-key add -
```

فرمان های پوسته زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
در صورتی که از این مخزن برای سایر نسخه های لینوکس دبیان استفاده می کنید، لطفاً مطمئن شوید که حداقل **نسخه 5.3** از **Qt** را نصب کرده اید.
:::

[بارگیری مستقیم](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_8.0)
