# نصب در لینوکس اوپن سوزه

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->


## در هر نسخه اپن سوزه

شما می توانید QOwnNotes را با استفاده از ابزار [OBS Package Installer](https://github.com/openSUSE/opi) نصب کنید.

فرمان پوسته ای زیر را به صورت ریشه برای نصب `opi`اجرا کنید:

```bash
zypper install opi
```

سپس QOwnNotes را با این دستور نصب کنید:

```bash
opi qownnotes
```

::: warning
این ابزار کل سرویس OBS را جستجو خواهد کرد؛ بنابراین در صورت جویا شدن از انتخاب `qownnotes` و نه `qownnotes-lang` اطمینان حاصل نمایید.

همچنین بررسی کنید که مخزن انتخاب شده، `home:pbek:QOwnNotes` اصلی باشد نه یک مورد شخص ثالث.
:::

::: tip
شما باید این گزینه را انتخاب کنید تا مخزن را پس از نصب برای دریافت به روز رسانی ها حفظ کنید.
:::

## اوپن سوزه لیپ 15.4

فرمان های پوسته ای زیر را برای افزودن مخزن به صورت ریشه اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## اوپن سوزه لیپ 15.3

فرمان های پوسته ای زیر را برای افزودن مخزن به صورت ریشه اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## اوپن سوزه لیپ 15.2

فرمان های پوسته زیر را برای افزودن مخزن به صورت ریشه اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)


## SLE 12 SP3 Backports

Run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_12_SP3_Backports)

## SLE 15

فرمان های پوسته ای زیر را برای افزودن مخزن به صورت ریشه اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[دانلود مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
