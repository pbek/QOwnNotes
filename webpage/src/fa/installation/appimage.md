# نصب به صورت AppImage

شما می توانید آخرین نسخه AppImage را از [صفحه انتشار های QOwnNotes](https://github.com/pbek/QOwnNotes/releases) بارگیری کنید. آن باید در صفحه مورد نظر به شکلی نظیر `QOwnNotes-x86_64.AppImage` نامگذاری شود.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed, you can also download the latest AppImage directly:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

سپس می توانید مجوزهای اجرا را در پرونده تغییر دهید:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
AppImages need `libfuse2` to run! For example under Ubuntu you can install the library with `sudo apt install libfuse2`.
:::

Afterward you should be able to execute the AppImage to run QOwnNotes.

::: warning
در صورت تمایل به استفاده از **بروز رسان خودکار**، لطفاً مطمئن شوید که AppImage خود را در جایی که حساب کاربری شما به آن دسترسی نوشتاری دارد (مثلاً در دایرکتوری شخصی)، قرار دهید.
:::

::: tip
در صورت بروز مشکل در اجرای AppImage، به دلیل بسیار قدیمی بودن نسخه glibc شما، [AppImage built on OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage) که بایستی با glibc 2.16 ایجاد شود را می‌توانید امتحان کنید.
:::

## Portable mode

You can run the AppImage in **portable mode** by passing the `--portable` argument:

```bash
./QOwnNotes-x86_64.AppImage --portable
```

In portable mode all application data is stored in a `Data` folder next to the AppImage file instead of in your home directory. This includes:

- Settings (`Data/QOwnNotes.ini`)
- The notes database (`Data/QOwnNotes.sqlite`)
- Your notes (`Data/Notes/`)
- Scripts, spell-check dictionaries, and other application data

This makes it easy to carry QOwnNotes together with all your notes on a USB drive or any other portable storage device.

::: tip
Make sure the directory containing the AppImage file is writable, so that the `Data` folder can be created next to it.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on your Linux distribution and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
