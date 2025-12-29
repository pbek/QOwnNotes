# مرور کلی

<ScreenshotCarousel />

<v-divider />

**Q** برای [چارچوب Qt](https://www.qt.io/) که بکار می رود  
**Own** برای اینکه شما صاحب یادداشت های خودتان هستید  
**Notes** برای یادداشت هایی که بر می دارید

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) برنامه **یادداشت برداری مارک داون** پرونده با متن ساده و **متن باز** (GPL) برای سیستم عامل های **لینوکس (GNU)**، **مکینتاش X** و **ویندوز** است که (به طور اختیاری) با هم با [**برنامه یادداشت**](https://github.com/nextcloud/notes) [نکست کلود](https://nextcloud.com/) و [اون کلود](https://owncloud.org/) کار می کند.

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[مفهوم QOwnNotes را درک کنید](concept.md)

## ویژگی ها

- You can use your **existing text or markdown files**, no need for an import
- QOwnNotes در ++C نوشته شده و برای **مصرف منابع کم** بهینه سازی شده است (بدون برنامه الکترونی نیازمند (hungry Electron app) به حافظه و CPU).
- **ورودی اورنوت و جاپلین**
- **Compatible** with the [Notes](https://apps.nextcloud.com/apps/notes) application of **Nextcloud / ownCloud** and **mobile** and Nextcloud Text
- Create **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) cards** and link to them in your notes
- Support for **sharing notes** on your **ownCloud** / **Nextcloud** server
- Manage your Nextcloud / ownCloud **todo lists** (**Nextcloud tasks** or **Tasks Plus** / **Calendar Plus**) or use another **CalDAV server** to sync your tasks to
- Older **versions** of your notes can be **restored** from your **Nextcloud / ownCloud** server
- **Trashed notes** can be **restored** from your **Nextcloud / ownCloud** server
- **External changes** of note files are **watched** (notes or note list are reloaded)
- Support for **hierarchical note tagging** and **note subfolders**
- Optional **encryption** of notes, AES-256 is built in, or you can use custom encryption methods like **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) or **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- The **notes folder** can be **freely chosen** (multiple note folders can be used)
- **Searching parts of words** in notes is possible and search results are highlighted in the notes
- [**Browser extension**](browser-extension.md) to **add notes from the selected text**, **make screenshots** or **manage your bookmarks**
- [**Spellchecking**](../editor/spellchecking.md) support
- **Portable mode** for carrying QOwnNotes around on USB sticks
- **Scripting support** and an online [**script repository**](https://github.com/qownnotes/scripts) where you can install scripts inside the application
- Implementation of the **[OpenAI completion API](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** to be used in scripts
- **Vim mode**
- **[Web application](web-app.md)** to **insert photos from your mobile phone** into the current note in QOwnNotes on your desktop

## ظاهر

- **برجسته سازی مارک داون** یادداشت ها و **حالت پیش نمایش مارک داون**
- **پشتیبانی از پوسته حالت تاریک**
- **Theming support** for the **Markdown syntax highlighting**
- همه **پنل ها در هر جایی که می خواهید می توانند قرار بگیرند**، حتی می توانند **شناور** یا **انباشته** باشند (کاملاً قابل اتصال)
- با پشتیبانی از **شمایل های پوسته freedesktop**، می توانید با **شمایل های دسکتاپ مجلی** و با **پوسته دسکتاپ تاریک** و محبوب خود از QOwnNotes بهره مند شوید
- **حالت بدون مزاحمت**، **حالت تمام صفحه** و **حالت ماشین تحریر**
- **شماره خط**
- **Differences** between current note and externally changed note are **shown** in a dialog

## زبان ها

- **در دسترس برای بیش از 60 زبان مختلف** مانند انگلیسی، آلمانی، فرانسوی، لهستانی، چینی، ژاپنی، روسی، پرتغالی، مجاری، هلندی و اسپانیایی
  - [کمک شما](../contributing/translation.md) برای بهبود این ترجمه ها یا ترجمه QOwnNotes به زبان های بیشتر بسیار مایه خشنودی خواهد بود

<style>
.v-divider {
  margin: 20px;
}

.sheet {
  position: absolute;
  bottom: 50px;
  background-color: rgba(0,0,0, 0.5);
  color: white;
  text-align: center;
  display: flex;
  align-items:center;
  justify-content:center;
  height: 50px;
  width: 100%;
}

.v-window__next {
  right: 0;
}

@media (max-width: 500px) {
  .v-carousel {
    height: 400px!important;
  }
}

@media (max-width: 350px) {
  .v-carousel {
    height: 250px!important;
  }
}

@media (max-width: 200px) {
  .v-carousel {
    height: 150px!important;
  }
}
</style>
