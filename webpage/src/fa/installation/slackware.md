# نصب در لینوکس اسلکویر

## اسلکویر 14.2

در ابتدا شما باید Qt 5، libxkbcommon، libproxy و js185 را نصب کنید.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

سپس می توانید مخزن اسلکبیلدز QOwnNotes را وارسی کرده و برنامه را بسازید.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

پس از آن می توانید بسته ایجاد شده را با `installpkg` نصب کنید.

## اسلکویر 14.1

نخست باید Qt 5 را نصب نمایید.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

سپس می توانید مخزن اسلکبیلدز QOwnNotes را وارسی کرده و برنامه را بسازید.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

پس از آن می توانید بسته ایجاد شده را با `installpkg` نصب کنید.

[اسلکبیلدز QOwnNotes در گیت هاب](https://github.com/pbek/qownnotes-slackbuilds/)
