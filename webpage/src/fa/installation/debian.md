# نصب در لینوکس دبیان

## دبیان 11.0

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید:

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

فرمان های پوسته زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
در صورتی که از این مخزن برای سایر نسخه های لینوکس دبیان استفاده می کنید، لطفاً مطمئن شوید که حداقل **نسخه 5.11** از **Qt** را نصب کرده اید.
:::

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## دبیان 10.0

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید.

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

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

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

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
