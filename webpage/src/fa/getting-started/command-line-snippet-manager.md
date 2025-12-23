# مدیر تکه کد خط فرمان

با استفاده از [مدیر تکه کد خط فرمان QOwnNotes](https://github.com/qownnotes/qc) می توانید **تکه کدهای دستوری ذخیره شده در یادداشت ها** را در QOwnNotes و از طریق خط فرمان اجرا کنید.

![qc](/img/qc.png)

با استفاده از **یادداشت های دارای برچسب خاص** می توانید **تکه کدهای دستوری** را ذخیره کرده و **از مدیر تکه کد خط فرمان اجرا کنید**.

![دستورات](/img/commands.png)

## نصب

[صفحه جدیدترین انتشار](https://github.com/qownnotes/qc/releases/latest) را ملاحظه کرده و نسخه مورد نیاز خود را بارگیری کنید.

::: tip
در صورت نصب [jq](https://stedolan.github.io/jq) نیز می‌توانید از این تکه کد استفاده کنید تا برای مثال آخرین نسخه Linux AMD64 AppImage را بارگیری و در آدرس `/usr/local/bin/qc` نصب کنید:

```bash
curl https://api.github.com/repos/qownnotes/qc/releases/latest | \
jq '.assets[] | select(.browser_download_url | endswith("_linux_amd64.tar.gz")) | .browser_download_url' | \
xargs curl -Lo /tmp/qc.tar.gz && \
tar xfz /tmp/qc.tar.gz -C /tmp && \
rm /tmp/qc.tar.gz && \
sudo mv /tmp/qc /usr/local/bin/qc && \
/usr/local/bin/qc --version
```

:::

## متعلقات

[fzf](https://github.com/junegunn/fzf) (جستجوی فازی) یا [peco](https://github.com/peco/peco) (نسخه قدیمی تر اما به احتمال بیشتر نصب شده به صورت پیش فرض) برای جستجوی دستورات در خط فرمان باید نصب شده باشد.

::: tip
به صورت پیش فرض، `fzf` برای جستجو استفاده می شود اما می‌توانید از `peco` با انجام تنظیمات بر روی آن با `qc configure` استفاده کنید.
:::

## تنظیمات

![توکن-سرور-سوکت](/img/socket-server-token.png)

Before you are using the snippet manager you need to enable the _Web socket server_ (2) in the _Browser extension / command snippets_ (1) settings in QOwnNotes.

سپس باید توکن امنیتی را نشان داده (3) و از آن رونوشت برداری کنید (4).

اکنون از طریق دستور پیشرو پرونده پیکربندی مدیر تکه کد را باز کنید:

```bash
# پیکربندی مدیر تکه کد
qc configure
```

و توکن امنیتی را در مکان مشخص `توکن` قرار دهید:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

::: tip
در قسمت تنظیمات QOwnNotes نیز می توانید تعیین کنید که از چه برچسب یادداشتی برای جستجوی دستورات در یادداشت ها استفاده شود. به صورت پیش فرض برچسب `دستورات` مورد استفاده قرار می گیرد.
:::

## چیدمان تکه کدهای دستوری

شما می توانید از **لیست های نامرتب با کد بلوک های هم تراز** برای ذخیره سازی تکه کدهای دستوری استفاده کنید. همه یادداشت های دارای برچسب `دستورات` برای تکه کدهای دستوری مورد جستجو قرار می گیرند.

در صورت افزودن `cmd:` قبل از کد بلوک هم تراز، دستور نیز بدون توجه به برچسب یادداشت در **یادداشت کنونی** دریافت خواهد شد.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3

* `echo I am also a command` I am a description #tag3 #tag4 #tag5

- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags

<!-- Example for asking for user input -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

**کد بلوک های `بش` یا `پوسته`** (بعد از سرخط 2 یا بالاتر به صورت توضیح) برای جستجوی تکه کدهای دستوری نیز قابل استفاده هستند. همچنین اگر برچسب ها بین عنوان و کد بلوک قرار گیرند، پشتیبانی خواهند شد.

    ## انجام این کار با کد بلوک «بش»

    - این متن نادیده گرفته می شود
    - اما این برچسب ها قابل استفاده هستند: #tag1 #tag2

    ```bash
    echo do this
    echo do that
    ```

    ## کار دیگری با کد بلوک «sh» انجام دهید

    ```sh
    echo do something else
    echo do something other
    ```

مثال فوق منجر به دو تکه کد دستوری خواهد شد که مورد نخست دارای دو برچسب `tag1` و `tag2` می باشد.

## کاربرد

```bash
# جستجو و اجرای تکه کدهای دستوری
qc exec
```

```bash
# جستجو و چاپ تکه کدهای دستوری
qc search
```

## پیکربندی

`qc configure` را اجرا کنید.

```toml
[General]
  editor = "vim"            # your favorite text editor
  column = 40               # column size for list command
  selectcmd = "fzf"         # selector command for edit command (fzf or peco)
  sortby = ""               # specify how snippets get sorted (recency (default), -recency, description, -description, command, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # your QOwnNotes API token
  websocket_port = 22222    # websocket port in QOwnNotes
```

## تکمیل پوسته

شما می توانید کد تکمیل پوسته مربوط را با `qc completion <shell>` ایجاد کنید.

برای مثال برای پوسته ماهی می توانید از این دستور استفاده کنید:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
