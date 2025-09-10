# نظرة عامة

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="QOwnNotes screenshot" />
    <div class="sheet">
      تحرير ملاحظاتك مع تلوين ماركداون ووسوم ملونة ومجلدات فرعية
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Minimal view" />
    <div class="sheet">
      واجهة مستخدم بسيطة والتي يمكن تبسيطها أكثر أيضا
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vertical view" />
    <div class="sheet">
      أظهر ملاحظاتك رأسيًا بتحريك اللوحات
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-note-relations.png" alt="Note relations" />
    <div class="sheet">
      Note relations panel
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Portable mode" />
    <div class="sheet">
      الوضع المحمول لوحدات تخزين USB
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="One column" />
    <div class="sheet">
      إمكانية وضع جميع اللوحات في أي مكان تشاء
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="screenshot darkmode" />
    <div class="sheet">
      الوضع المظلم
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="screenshot-distraction-free-mode" />
    <div class="sheet">
      الوضع الخالي من المشوشات
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Note encryption" />
    <div class="sheet">
      تشفير AES اختياري للملاحظات (وقابل للبرمجة أيضا)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Encrypted note" />
    <div class="sheet">
      الملاحظات المشفرة هي مجرد نص أيضا
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
    <div class="sheet">
      إظهار الفروقات عندما تُعدّل الملاحظات خارج التطبيق
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="screenshot-export-print" />
    <div class="sheet">
      طباعة الملاحظات وتصديرها بصيغة PDF
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-freedesktop-theme.png" alt="screenshot-freedesktop-theme" />
    <div class="sheet">
      أيقونات من مشروع فري دسكتوب
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-other-workspace.png" alt="screenshot-other-workspace" />
    <div class="sheet">
      يمكنك استخدام مساحات عمل مختلفة
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="screenshot-qml" />
    <div class="sheet">
      قابل للبرمجة
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="screenshot-russian" />
    <div class="sheet">
      متاح بلغات عديدة
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="screenshot-search-in-all-notes" />
    <div class="sheet">
      بحث في جميع الملاحظات
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="screenshot-search-in-current-note" />
    <div class="sheet">
      بحث في الملاحظة الحالية
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="screenshot-settings-note-folders" />
    <div class="sheet">
      إمكانية استخدام مجلدات متعددة للملاحظات
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
    <div class="sheet">
      إدارة قوائم مهامك من خلال CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      إدارة ملاحظاتك المهملة على خادم Nextcloud الخاص بك
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      إدارة نسخ ملاحظاتك على خادم Nextcloud الخاص بك
    </div>
  </v-carousel-item>
</v-carousel>
</template>

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
- **الملاحظات المهملة** يمكن **استعادتها** من خادم **Nextcloud أو ownCloud** الخاص بك
- **التعديلات خارج التطبيق** على ملفات الملاحظات يتم **مراقبتها** (ويتم إعادة تحميل الملاحظة أو قائمة الملاحظات)
- دعم **الوسوم الهرمية للملاحظات** وكذلك **المجلدات الفرعية للملاحظات**
- **تعمية** اختيارية للملاحظات؛ يتضمن التطبيق خوارزمية AES-256، لكن يمكنك استخدم طريقة تعمية مخصصة مثل **[Keybase.io](https://keybase.io/)** (انظر [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) أو **PGP** (انظر [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- **مجلد الملاحظات** يمكن **اختياره بحرية** (يمكن اختيار مجلدات متعددة للملاحظات)
- إمكانية **البحث بأجزاء الكلمات** في الملاحظات، وتظليل نتائج البحث فيها
- [**إضافة للمتصفح**](browser-extension.md) حتى يمكنك **إضافة ملاحظات من النص المحدد** أو **أخذ لقطات شاشة** أو **إدارة إشاراتك المرجعية**
- دعم [**التدقيق الإملائي**](../editor/spellchecking.md)
- **الوضع المحمول** لحمل QOwnNotes على وحدة تخزين USB والتنقل به
- **قابلية البرمجة** وكذلك [**مستودع بُريمِجات**](https://github.com/qownnotes/scripts) على الإنترنت، حيث يمكنك تثبيت البُريمِجات من داخل التطبيق
- دعم **[API الإكمال بالذكاء المصطنع من OpenAI](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** لاستعماله في البرمجة
- **وضع Vim**
- **[تطبيق ويب](web-app.md)** ليمكنك **إضافة صور من محمولك** إلى الملاحظة الحالية في QOwnNotes على حاسوبك

## المظهر

- **تلوين ماركداون** للملاحظات **ووضع معاينة لماركداون**
- **دعم سِمة الوضع المظلم**
- **دعم السمات** في **تلوين ماركداون**
- جميع **اللوحات تستطيع وضعها أينما شئت**؛ يمكن حتى جعلها **حرة طافية** أو **مرصوصة** (قابلة للتركيب)
- دعم **أيقونات سمات فري دسكتوب**؛ يمكنك حتى استخدام QOwnNotes مع **أيقونات سطح مكتبك الأصيلة** و**سمة سطح مكتبك المظلمة** المفضلة
- **الوضع الخالي من المشوشات**، **ووضع ملء الشاشة**، **ووضع الآلة الكاتبة**
- **ترقيم السطور**
- **الفروقات** بين الملاحظة الحالية والملاحظة المعدّلة خارج التطبيق **تظهر** في صندوق حوار

## اللغات

- **متاح بأكثر من ٦٠ لغة مختلفة** مثل العربية والإنجليزية والفرنسية والألمانية والإسبانية والبولندية والصينية واليابانية والروسية والبرتغالية والمجرية والهولندية
  - [مساعدتك](../contributing/translation.md) مرحب بها جدا لتحسين هذه الترجمات أو لترجمة QOwnNotes إلى لغات أخرى

<style>
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
