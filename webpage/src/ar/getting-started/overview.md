# نظرة عامة

<ScreenshotCarousel />

<v-divider />

**Q** نسبةً إلى [إطار عمل Qt](https://www.qt.io/) المستخدم  
**Own** لأنك أنت من يملك ملاحظاتك  
**Notes** للملاحظات التي تدوّنها

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) هو تطبيق **حر ومفتوح المصدر** (بترخيص GPL) **لتدوين الملاحظات بصيغة ماركداون** في ملفات نصية مجردة، لأنظمة **غنو/لينكس** و**ماك&nbsp;أو&nbsp;إس** و**ويندوز**، والذي (اختياريًا) يعمل مع [**تطبيق Notes**](https://github.com/nextcloud/notes) من [Nextcloud](https://nextcloud.com/) و&nbsp;[ownCloud](https://owncloud.org/).

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[نظرة على مفاهيم QOwnNotes](concept.md)

## الخصائص

- يمكنك استخدام **ملفاتك الحالية، النصية أو التي بصيغة ماركداون**؛ لا تحتاج إلى استيرادها
- QOwnNotes مكتوب بلغة سي++ ومحسَّن **لتقليل استهلاك الموارد** (ليس تطبيق Electron متعطش للذاكرة والمعالج)
- **استيراد من Evernote و&nbsp;Joplin**
- **متوافق** مع تطبيق [Notes](https://apps.nextcloud.com/apps/notes) من **Nextcloud / ownCloud** و**المحمول** وكذلك Nextcloud Text
- أنشئ بطاقات **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck)** وأضف روابط إليهم في ملاحظاتك
- دعم **مشاركة الملاحظات** على خادم **ownCloud** أو **Nextcloud** الخاص بك
- إدارة **قوائم مهامك** على Nextcloud أو ownCloud (تطبيقات **Nextcloud tasks** أو **Tasks Plus** أو **Calendar Plus**)، أو استخدام **خادم CalDAV** آخر لمزامنة مهامك معه
- **النسخ** الأقدم من ملاحظاتك يمكن **استعادتها** من خادم **Nextcloud أو ownCloud** الخاص بك
- **Trashed notes** can be **restored** from your **Nextcloud / ownCloud** server
  - There are also **local trash** and **system trash** modes
- **التعديلات خارج التطبيق** على ملفات الملاحظات يتم **مراقبتها** (ويتم إعادة تحميل الملاحظة أو قائمة الملاحظات)
- دعم **الوسوم الهرمية للملاحظات** وكذلك **المجلدات الفرعية للملاحظات**
  - Includes moving and reorganizing subfolders with link updates and selective subfolder visibility
- Optional **wiki-style note links** like `[[Note]]` with autocomplete, heading anchors, aliases, backlinks, and refactoring support
- **تعمية** اختيارية للملاحظات؛ يتضمن التطبيق خوارزمية AES-256، لكن يمكنك استخدم طريقة تعمية مخصصة مثل **[Keybase.io](https://keybase.io/)** (انظر [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) أو **PGP** (انظر [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- **مجلد الملاحظات** يمكن **اختياره بحرية** (يمكن اختيار مجلدات متعددة للملاحظات)
- إمكانية **البحث بأجزاء الكلمات** في الملاحظات، وتظليل نتائج البحث فيها
- [**إضافة للمتصفح**](browser-extension.md) حتى يمكنك **إضافة ملاحظات من النص المحدد** أو **أخذ لقطات شاشة** أو **إدارة إشاراتك المرجعية**
- [**Spellchecking**](../editor/spellchecking.md), optional [**LanguageTool**](../editor/languagetool.md), optional [**Harper**](../editor/harper.md), and optional [**Markdown LSP**](../editor/markdownlsp.md) support (e.g. [Marksman](https://github.com/artempyanykh/marksman) for completions and diagnostics, or [Rumdl](https://github.com/rvben/rumdl) for linting)
- **الوضع المحمول** لحمل QOwnNotes على وحدة تخزين USB والتنقل به
- **قابلية البرمجة** وكذلك [**مستودع بُريمِجات**](https://github.com/qownnotes/scripts) على الإنترنت، حيث يمكنك تثبيت البُريمِجات من داخل التطبيق
- Built-in **AI support** with script integration for providers like **OpenAI** and **Groq**
  - Includes a built-in **MCP server** so external AI agents can search and fetch notes securely
- **وضع Vim**
- **[تطبيق ويب](web-app.md)** ليمكنك **إضافة صور من محمولك** إلى الملاحظة الحالية في QOwnNotes على حاسوبك

## المظهر

- **تلوين ماركداون** للملاحظات **ووضع معاينة لماركداون**
  - Includes **inline image previews**, **Markdown heading folding**, and optional hiding of Markdown formatting syntax
- **Dark mode theme support** with live switching and custom **color modes**
- **دعم السمات** في **تلوين ماركداون**
- جميع **اللوحات تستطيع وضعها أينما شئت**؛ يمكن حتى جعلها **حرة طافية** أو **مرصوصة** (قابلة للتركيب)
- دعم **أيقونات سمات فري دسكتوب**؛ يمكنك حتى استخدام QOwnNotes مع **أيقونات سطح مكتبك الأصيلة** و**سمة سطح مكتبك المظلمة** المفضلة
- **الوضع الخالي من المشوشات**، **ووضع ملء الشاشة**، **ووضع الآلة الكاتبة**
- **ترقيم السطور**
- **Block selection** and other power-user editor workflows
- Support for **global keyboard shortcuts on Wayland**
- **Differences** between current note and externally changed note are **shown** in a dialog

## اللغات

- **متاح بأكثر من ٦٠ لغة مختلفة** مثل العربية والإنجليزية والفرنسية والألمانية والإسبانية والبولندية والصينية واليابانية والروسية والبرتغالية والمجرية والهولندية
  - [مساعدتك](../contributing/translation.md) مرحب بها جدا لتحسين هذه الترجمات أو لترجمة QOwnNotes إلى لغات أخرى

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
