# وارد کردن یادداشت ها

## اورنوت

کادر گفتگوی ورودی اورنوت در دسترس قرار دارد تا بتوانید به فهرست `یادداشت / ورود` دسترسی داشته باشید.

## جاپلین

کادر گفتگوی ورودی جاپلین در دسترس قرار دارد تا بتوانید به فهرست `یادداشت / ورود` دسترسی داشته باشید.

## تام بوی

با استفاده از اسکریپتی به نام [ترومبون](https://github.com/samba/trombone) می توانید یادداشت های تام بوی خود را وارد کنید.

با نصب python2 می توانید اسکریپت را به کار بیندازید. It will convert your Tomboy notes to an Evernote `.enex` file, which then you are able to import to QOwnNotes.

For anyone wanting to do this, first make sure you have Python2 installed and you might want to also install `python-is-python2` (you can remove that later):

```bash
 python2 python-is-python2 ،sudo apt را نصب می کند 
```

Download the trombone file from GitHub, extract and jump into that folder:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

Then cd into the folder where your tomboy notes are:

```bash
 cd ~/.local/share/tomboy/
```

Then run this:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

If you get unicode errors about specific notes, just remove each note and run the conversion again until it gives a message saying `Saving...` . You will have a file called `EXPORT.enex` which can then be imported into QOwnNotes.

During the import into QOwnNotes you might uncheck all of the attributes to import except maybe the creation/modification date, since Tomboy does not have those features.
