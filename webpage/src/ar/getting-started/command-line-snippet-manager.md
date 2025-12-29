# مدير قصاصات سطر الأوامر

يمكنك استخدام [مدير قصاصات سطر أوامر QOwnNotes](https://github.com/qownnotes/qc) من أجل **تنفيذ قصاصات الأوامر المخزنة في الملاحظات** في QOwnNotes من سطر الأوامر.

![qc](/img/qc.png)

يمكنك استخدام **ملاحظات بوسم خاص** لتخزين **قصاصات الأوامر**، والتي يمكنك **تنفيذها من مدير قصاصات سطر الأوامر**.

![commands](/img/commands.png)

## التثبيت

قم بزيارة [صفحة الإصدارة الأخيرة](https://github.com/qownnotes/qc/releases/latest) وقم بتنزيل النسخة التي تريدها.

::: tip
إذا كان [jq](https://stedolan.github.io/jq) مثبتا على جهازك، يمكنك أيضا استخدام هذه القصاصة لتنزيل وتثبيت مثلا النسخة الأخيرة من AppImage لمعمارية AMD64 من لينكس إلى ‪`/usr/local/bin/qc`‬:

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

## الاعتماديات

للبحث عن أوامر من سطر الأوامر، تحتاج إلى تثبيت [fzf](https://github.com/junegunn/fzf) (للبحث الضبابي، أي التقريبي) أو [peco](https://github.com/peco/peco) (أقدم، لكن احتمال كونه مثبتا بشكل افتراضي أكبر).

::: tip
بشكل افتراضي، `fzf` هو المستخدم في البحث، لكن يمكنك استخدام `peco` بضبطه باستخدام `qc configure`.
:::

## التثبيت

![socket-server-token](/img/socket-server-token.png)

Before you are using the snippet manager you need to enable the _Web socket server_ (2) in the _Browser extension / command snippets_ (1) settings in QOwnNotes.

ثم يجب عليك إظهار رمز الأمان (3) ونسخه (4).

الآن افتح ملف إعداد مدير القصاصات باستخدام:

```bash
# إعداد مدير القصاصات
qc configure
```

ثم ضع رمز الأمان في خاصية `token`:

```toml
[QOwnNotes]
token = "رمز الأمان الخاص بك هنا"
```

::: تلميح داخل إعدادات QOwnNotes بإمكانك أيضا وضع وسم الملاحظة الذي يجب إستعماله من أجل البحث عن الأوامر داخل الملاحظات. بشكل تلقائي يتم إستعمال الوسم `commands`.
:::

## صيغة قصاصات الأوامر

يمكنك استخدام **قائمة منقوطة عناصرها أكواد مضمّنة في السطر** لتخزين قصاصات الأوامر. سيتم البحث عن قصاصات الأوامر في جميع الملاحظات الموسومة بالوسم `commands`.

وإذا أضفت `cmd:` قبل الكود المضمّن في السطر، فإنك ستجده أيضا في **الملاحظة الحالية** بغض النظر عن وسوم الملاحظة.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3

* `echo I am also a command` I am a description #tag3 #tag4 #tag5

- cmd: `echo I will be found in the current note` This command will be found in the current note regardless of note tags

<!-- Example for asking for user input -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

يمكن استخدام **كتل أكواد `bash` أو `shell` ** لقصاصات الأوامر، على أن تُسبق بعنوان من المستوى الثاني أو أعلى، والذي سيستخدم لوصفها. الوسوم مدعومة إذا كانت بين العنوان وكتلة الكود.

    ## افعل شيئا بكتلة كود باش: bash

    - سيتم تجاهل هذا النص
    - لكن يمكن استخدام الوسوم: #وسم١ #وسم٢

    ```bash
    echo do this
    echo do that
    ```


    ## افعل شيئا آخر بكتلة كود شل: sh

    ```sh
    echo do something else
    echo do something other
    ```

سيُنتج هذا المثال قصاصتي أوامر: أولاهما موسومة بالوسمين `وسم١` و&nbsp;`وسم٢`.

## الاستخدام

```bash
# ابحث عن قصاصات أوامر ونفذها
qc exec
```

```bash
# ابحث عن قصاصات أوامر واعرضها
qc search
```

## التهيئة

نفّذ `qc configure`.

```toml
[General]
  editor = "vim"            # محرر النصوص المفضل لديك
  column = 40               # عرض العمود في أمر السرد
  selectcmd = "fzf"         # برنامج التحديد من أجل أمر التحرير (fzf أو peco)
  sortby = ""               # تعيين طريقة ترتيب القصاصات؛ القيم المقبولة هي:
      # recency = بالأحدث (القيمة الافتراضية)
      # -recency = بالأحدث، ترتيب عكسي
      # description = بالوصف
      # -description = بالوصف، ترتيب عكسي
      # command = بالأمر
      # -command = بالأمر، ترتيب عكسي
      # output = بالناتج
      # -output = بالناتج، ترتيب عكسي

[QOwnNotes]
  token = "MvTagHXF"        # رمز الأمان
  websocket_port = 22222    # منفذ مقبس الويب في التطبيق
```

## إكمال الصَدفة

يمكنك توليد كود إكمال الصدفة المناسب لصدفتك بالأمر <code dir="ltr">qc completion &lt;اسم_صدفتك&gt;</code>.

مثلا لصدفة Fish، يمكنك تنفيذ الأمر:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
