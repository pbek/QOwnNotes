const { description } = require("../../package");
const _ = require("lodash");
const utils = require("./utils");

// We don't want translation for these
const contactNavItems = [
  {
    text: "Ask question",
    link: "https://github.com/pbek/QOwnNotes/issues#support",
  },
  {
    text: "Ask for feature",
    link: "https://github.com/pbek/QOwnNotes/issues#feature",
  },
  { text: "Report bug", link: "https://github.com/pbek/QOwnNotes/issues#bug" },
  { text: "Telegram Channel", link: "https://t.me/QOwnNotes" },
  {
    text: "Matrix/Element.io Room",
    link: "https://matrix.to/#/#qownnotes:matrix.org",
  },
  { text: "Gitter Chat", link: "https://gitter.im/qownnotes/qownnotes" },
  { text: "IRC Channel", link: "https://web.libera.chat/#qownnotes" },
  { text: "Mastodon", link: "https://social.qownnotes.org/@qownnotes" },
  { text: "Twitter", link: "https://twitter.com/QOwnNotes" },
  { text: "Facebook", link: "https://www.facebook.com/QOwnNotes" },
];

const gettingStartedNavItems = [
  "getting-started/overview",
  "getting-started/concept",
  "getting-started/demo",
  "getting-started/markdown",
  "getting-started/importing-notes",
  "getting-started/cli-parameters",
  "getting-started/browser-extension",
  "getting-started/command-line-snippet-manager",
  "getting-started/web-app",
  "getting-started/qownnotesapi",
  "getting-started/note-relations",
  "getting-started/searching",
  "getting-started/shortcuts",
  "getting-started/git-versioning",
];

const installationNavItems = [
  "installation/",
  "installation/ubuntu",
  "installation/windows",
  "installation/macos",
  "installation/debian",
  "installation/opensuse",
  "installation/fedora",
  "installation/snap",
  "installation/flatpak",
  "installation/appimage",
  "installation/nix",
  "installation/arch",
  "installation/solus",
  "installation/kaos",
  "installation/centos",
  "installation/raspberry-pi-os",
  "installation/gentoo",
  "installation/funtoo",
  "installation/void",
  "installation/slackware",
  "installation/freebsd",
  "installation/building",
];

const editorNavItems = [
  "editor/spellchecking",
  "editor/ai",
  "editor/time-formats",
];

const scriptingNavItems = [
  "scripting/",
  "scripting/methods-and-objects",
  "scripting/hooks",
  "scripting/classes",
];

const contributingNavItems = [
  "contributing/get-involved",
  "contributing/code-of-conduct",
  "contributing/maintainers",
  "contributing/translation",
  "contributing/donate",
  "contributing/sponsors",
];

const faqNavItems = [
  "faq/preview-without-edit",
  "faq/references",
  "faq/metrics",
];

const gettingStartedNavItem = "/getting-started/overview";
const installationNavItem = "/installation/";
const changelogNavItem = "/changelog";
const donateNavItem = "/contributing/donate";

module.exports = {
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#title
   */
  title: "QOwnNotes",

  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#description
   * Note: This overrides too much
   */
  // description: description,

  /**
   * Extra tags to be injected to the page HTML `<head>`
   *
   * ref：https://v1.vuepress.vuejs.org/config/#head
   */
  head: [
    ["meta", { name: "theme-color", content: "#3eaf7c" }],
    ["meta", { name: "apple-mobile-web-app-capable", content: "yes" }],
    [
      "meta",
      { name: "apple-mobile-web-app-status-bar-style", content: "black" },
    ],
    [
      "meta",
      { name: "viewport", content: "width=device-width, initial-scale=1.0" },
    ],
    [
      "link",
      {
        rel: "icon",
        type: "image/png",
        sizes: "128x128",
        href: "/favicon.png",
      },
    ],
    [
      "link",
      {
        rel: "stylesheet",
        href: "https://cdn.jsdelivr.net/npm/@mdi/font@5.x/css/materialdesignicons.min.css",
      },
    ],
    [
      "link",
      {
        rel: "alternate",
        title: "QOwnNotes Releases RSS",
        type: "application/rss+xml",
        href: "https://feeds.feedburner.com/QOwnNotesReleases",
      },
    ],
    [
      "link",
      {
        rel: "alternate",
        title: "QOwnNotes Blog RSS",
        type: "application/rss+xml",
        href: "https://feeds.feedburner.com/QOwnNotesBlog",
      },
    ],
    ["link", { rel: "me", href: "https://social.qownnotes.org/@patrizio" }],
    ["link", { rel: "me", href: "https://social.qownnotes.org/@qownnotes" }],
    // ['link', { rel: 'stylesheet', href: `https://cdn.jsdelivr.net/npm/vuetify/dist/vuetify.min.css` }],
    // ['script', { src: `https://cdn.jsdelivr.net/npm/vue/dist/vue.js` }],
    // ['script', { src: `https://cdn.jsdelivr.net/npm/vuetify/dist/vuetify.js` }],
  ],

  locales: {
    // The key is the path for the locale to be nested under.
    // As a special case, the default locale can use '/' as its path.
    "/": {
      lang: "en-US", // this will be set as the lang attribute on <html>
      title: "QOwnNotes",
    },
    "/de/": {
      lang: "de-DE",
      title: "QOwnNotes",
    },
    "/it/": {
      lang: "it-IT",
      title: "QOwnNotes",
    },
    "/fr/": {
      lang: "fr-FR",
      title: "QOwnNotes",
    },
    "/es/": {
      lang: "es-ES",
      title: "QOwnNotes",
    },
    "/fa/": {
      lang: "fa-IR",
      title: "QOwnNotes",
    },
    "/nl/": {
      lang: "nl-NL",
      title: "QOwnNotes",
    },
    "/hu/": {
      lang: "hu-HU",
      title: "QOwnNotes",
    },
    "/ar/": {
      lang: "ar-AR",
      title: "QOwnNotes",
    },
    "/pl/": {
      lang: "pl-PL",
      title: "QOwnNotes",
    },
    "/ko/": {
      lang: "ko-KR",
      title: "QOwnNotes",
    },
  },

  /**
   * Theme configuration, here is the default theme configuration for VuePress.
   *
   * ref：https://v1.vuepress.vuejs.org/theme/default-theme-config.html
   */
  themeConfig: {
    repo: "pbek/QOwnNotes",
    editLinks: true,
    domain: "https://www.qownnotes.org",
    author: { name: "Patrizio Bekerle", twitter: "@PatrizioBekerle" },
    docsBranch: "main",
    docsDir: "webpage/src",
    editLinkText: "Help us improve this page!",
    lastUpdated: false,
    logo: "/favicon.png",
    activeHeaderLinks: false,
    locales: {
      "/": {
        // text for the language dropdown
        selectText: "🌍",
        // label for this locale in the language dropdown
        label: "English",
        // Aria Label for locale in the dropdown
        ariaLabel: "Languages",
        // text for the edit-on-github link
        editLinkText: "Help us improve this page!",
        // algolia docsearch options for current locale
        // https://v2.vuepress.vuejs.org/reference/plugin/docsearch.html#options
        algolia: {
          appId: "LP8PS829KJ",
          apiKey: "52d0b99615b7586dcd5a360a5cfe3f07",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Getting started",
            link: gettingStartedNavItem,
          },
          {
            text: "Installation",
            link: installationNavItem,
          },
          {
            text: "Contact",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Donate",
            link: donateNavItem,
          },
        ],
        sidebar: [
          {
            title: "Getting started",
            children: gettingStartedNavItems,
          },
          {
            title: "Installation",
            children: installationNavItems,
          },
          {
            title: "Editor",
            children: editorNavItems,
          },
          {
            title: "Scripting",
            children: scriptingNavItems,
          },
          {
            title: "Contributing",
            children: contributingNavItems,
          },
          {
            title: "FAQ",
            children: faqNavItems,
          },
          {
            title: "Blog",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Changelog",
            children: [changelogNavItem],
          },
        ],
      },
      "/de/": {
        selectText: "🌍",
        label: "Deutsch",
        editLinkText: "Hilf uns diese Seite zu verbessern!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Loslegen",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "de"),
          },
          {
            text: "Installation",
            link: utils.getNavItemForLanguage(installationNavItem, "de"),
          },
          {
            text: "Kontakt",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Spenden",
            link: utils.getNavItemForLanguage(donateNavItem, "de"),
          },
        ],
        sidebar: [
          {
            title: "Loslegen",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "de",
            ),
          },
          {
            title: "Installation",
            children: utils.getNavItemsForLanguage(installationNavItems, "de"),
          },
          {
            title: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "de"),
          },
          {
            title: "Skripten",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "de"),
          },
          {
            title: "Beitragen",
            children: utils.getNavItemsForLanguage(contributingNavItems, "de"),
          },
          {
            title: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "de"),
          },
          {
            title: "Blog (auf Englisch)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Änderungsprotokoll (auf Englisch)",
            children: [changelogNavItem],
          },
        ],
      },
      "/ko/": {
        selectText: "🌍",
        label: "한국어",
        editLinkText: "이 페이지를 개선할 수 있도록 도와주세요!",
        algolia: {
          appId: "LP8PS829KJ",
          apiKey: "52d0b99615b7586dcd5a360a5cfe3f07",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "시작하기",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "ko"),
          },
          {
            text: "설치",
            link: utils.getNavItemForLanguage(installationNavItem, "ko"),
          },
          {
            text: "연락하다",
            items: contactNavItems,
          },
          {
            text: "기부하다",
            link: utils.getNavItemForLanguage(donateNavItem, "ko"),
          },
        ],
        sidebar: [
          {
            title: "시작하기",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "ko",
            ),
          },
          {
            title: "설치",
            children: utils.getNavItemsForLanguage(installationNavItems, "ko"),
          },
          {
            title: "편집자",
            children: utils.getNavItemsForLanguage(editorNavItems, "ko"),
          },
          {
            title: "스크립팅",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "ko"),
          },
          {
            title: "기여",
            children: utils.getNavItemsForLanguage(contributingNavItems, "ko"),
          },
          {
            title: "자주하는 질문",
            children: utils.getNavItemsForLanguage(faqNavItems, "ko"),
          },
          {
            title: "Blog (auf Englisch)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Changelog(영문)",
            children: [changelogNavItem],
          },
        ],
      },
      "/es/": {
        selectText: "🌍",
        label: "Español",
        editLinkText: "¡Ayúdanos a mejorar este sitio!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Empezando",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "es"),
          },
          {
            text: "Instalación",
            link: utils.getNavItemForLanguage(installationNavItem, "es"),
          },
          {
            text: "Contacto",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Donar",
            link: utils.getNavItemForLanguage(donateNavItem, "es"),
          },
        ],
        sidebar: [
          {
            title: "Empezando",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "es",
            ),
          },
          {
            title: "Instalación",
            children: utils.getNavItemsForLanguage(installationNavItems, "es"),
          },
          {
            title: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "es"),
          },
          {
            title: "Guiones",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "es"),
          },
          {
            title: "Contribuir",
            children: utils.getNavItemsForLanguage(contributingNavItems, "es"),
          },
          {
            title: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "es"),
          },
          {
            title: "Blog (en inglés)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Registro de cambios (en inglés)",
            children: [changelogNavItem],
          },
        ],
      },
      "/hu/": {
        selectText: "🌍",
        label: "Magyar",
        editLinkText: "Segítsen javítani ezt az oldalt!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Az első lépések",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "hu"),
          },
          {
            text: "Telepítés",
            link: utils.getNavItemForLanguage(installationNavItem, "hu"),
          },
          {
            text: "Kapcsolat",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Adomány",
            link: utils.getNavItemForLanguage(donateNavItem, "hu"),
          },
        ],
        sidebar: [
          {
            title: "Az első lépések",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "hu",
            ),
          },
          {
            title: "Telepítés",
            children: utils.getNavItemsForLanguage(installationNavItems, "hu"),
          },
          {
            title: "Szerkesztő",
            children: utils.getNavItemsForLanguage(editorNavItems, "hu"),
          },
          {
            title: "Szkriptek",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "hu"),
          },
          {
            title: "Adomány",
            children: utils.getNavItemsForLanguage(contributingNavItems, "hu"),
          },
          {
            title: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "hu"),
          },
          {
            title: "Blog (angolul)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Változások (angolul)",
            children: [changelogNavItem],
          },
        ],
      },
      "/fr/": {
        selectText: "🌍",
        label: "Français",
        editLinkText: "Aidez-nous à améliorer ce site!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Commencer",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "fr"),
          },
          {
            text: "Installation",
            link: utils.getNavItemForLanguage(installationNavItem, "fr"),
          },
          {
            text: "Contact",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Faire un don",
            link: utils.getNavItemForLanguage(donateNavItem, "fr"),
          },
        ],
        sidebar: [
          {
            title: "Commencer",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "fr",
            ),
          },
          {
            title: "Installation",
            children: utils.getNavItemsForLanguage(installationNavItems, "fr"),
          },
          {
            title: "Éditeur",
            children: utils.getNavItemsForLanguage(editorNavItems, "fr"),
          },
          {
            title: "Les scripts",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "fr"),
          },
          {
            title: "Contribuer",
            children: utils.getNavItemsForLanguage(contributingNavItems, "fr"),
          },
          {
            title: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "fr"),
          },
          {
            title: "Blog (en anglais)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Changelog (en anglais)",
            children: [changelogNavItem],
          },
        ],
      },
      "/it/": {
        selectText: "🌍",
        label: "Italiano",
        editLinkText: "Aiutaci a migliorare questo sito!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Iniziare",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "it"),
          },
          {
            text: "Installazione",
            link: utils.getNavItemForLanguage(installationNavItem, "it"),
          },
          {
            text: "Contatto",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Donare",
            link: utils.getNavItemForLanguage(donateNavItem, "it"),
          },
        ],
        sidebar: [
          {
            title: "Iniziare",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "it",
            ),
          },
          {
            title: "Installazione",
            children: utils.getNavItemsForLanguage(installationNavItems, "it"),
          },
          {
            title: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "it"),
          },
          {
            title: "Script",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "it"),
          },
          {
            title: "Contribuire",
            children: utils.getNavItemsForLanguage(contributingNavItems, "it"),
          },
          {
            title: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "it"),
          },
          {
            title: "Blog (in inglese)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Log delle modifiche (in inglese)",
            children: [changelogNavItem],
          },
        ],
      },
      "/fa/": {
        selectText: "🌍",
        label: "فارسی",
        editLinkText: "در اصلاح این صفحه ما را یاری کنید!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "شروع",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "fa"),
          },
          {
            text: "نصب",
            link: utils.getNavItemForLanguage(installationNavItem, "fa"),
          },
          {
            text: "تماس",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "کمک مالی",
            link: utils.getNavItemForLanguage(donateNavItem, "fa"),
          },
        ],
        sidebar: [
          {
            title: "شروع",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "fa",
            ),
          },
          {
            title: "نصب و راه اندازی",
            children: utils.getNavItemsForLanguage(installationNavItems, "fa"),
          },
          {
            title: "ویرایشگر",
            children: utils.getNavItemsForLanguage(editorNavItems, "fa"),
          },
          {
            title: "اسکریپت نویسی",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "fa"),
          },
          {
            title: "مشارکت",
            children: utils.getNavItemsForLanguage(contributingNavItems, "fa"),
          },
          {
            title: "سؤالات متداول",
            children: utils.getNavItemsForLanguage(faqNavItems, "fa"),
          },
          {
            title: "وبلاگ (به انگلیسی)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "گزارش تغییرات (به انگلیسی)",
            children: [changelogNavItem],
          },
        ],
      },
      "/nl/": {
        selectText: "🌍",
        label: "Nederlands",
        editLinkText: "Help ons deze pagina te verbeteren!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Aan de slag",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "nl"),
          },
          {
            text: "Installatie",
            link: utils.getNavItemForLanguage(installationNavItem, "nl"),
          },
          {
            text: "Contact",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Doneren",
            link: utils.getNavItemForLanguage(donateNavItem, "nl"),
          },
        ],
        sidebar: [
          {
            title: "Aan de slag",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "nl",
            ),
          },
          {
            title: "Installatie",
            children: utils.getNavItemsForLanguage(installationNavItems, "nl"),
          },
          {
            title: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "nl"),
          },
          {
            title: "Skripten",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "nl"),
          },
          {
            title: "Bijdragen",
            children: utils.getNavItemsForLanguage(contributingNavItems, "nl"),
          },
          {
            title: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "nl"),
          },
          {
            title: "Blog (in het Engels)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Changelog (in het Engels)",
            children: [changelogNavItem],
          },
        ],
      },
      "/pl/": {
        selectText: "🌍",
        label: "Polski",
        editLinkText: "Pomóż nam ulepszyć tę stronę!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "Pierwsze kroki",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "pl"),
          },
          {
            text: "Instalacja",
            link: utils.getNavItemForLanguage(installationNavItem, "pl"),
          },
          {
            text: "Kontakt",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "Wpłać",
            link: utils.getNavItemForLanguage(donateNavItem, "pl"),
          },
        ],
        sidebar: [
          {
            title: "Pierwsze kroki",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "pl",
            ),
          },
          {
            title: "Instalacja",
            children: utils.getNavItemsForLanguage(installationNavItems, "pl"),
          },
          {
            title: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "pl"),
          },
          {
            title: "Skrypty",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "pl"),
          },
          {
            title: "Wkład",
            children: utils.getNavItemsForLanguage(contributingNavItems, "pl"),
          },
          {
            title: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "pl"),
          },
          {
            title: "Blog (auf Englisch)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "Lista zmian (po angielsku)",
            children: [changelogNavItem],
          },
        ],
      },
      "/ar/": {
        selectText: "🌍",
        label: "العربية",
        editLinkText: "ساعدنا في تحسين هذه الصفحة!",
        algolia: {
          apiKey: "35f878f4b712d5ab6a659eb0a6c71576",
          indexName: "qownnotes",
        },
        nav: [
          {
            text: "بدء الاستخدام",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "ar"),
          },
          {
            text: "التثبيت",
            link: utils.getNavItemForLanguage(installationNavItem, "ar"),
          },
          {
            text: "تواصل",
            ariaLabel: "Contact Menu",
            items: contactNavItems,
          },
          {
            text: "تبرع",
            link: utils.getNavItemForLanguage(donateNavItem, "ar"),
          },
        ],
        sidebar: [
          {
            title: "بدء الاستخدام",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "ar",
            ),
          },
          {
            title: "التثبيت",
            children: utils.getNavItemsForLanguage(installationNavItems, "ar"),
          },
          {
            title: "المحرر",
            children: utils.getNavItemsForLanguage(editorNavItems, "ar"),
          },
          {
            title: "البرمجة",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "ar"),
          },
          {
            title: "مساهمة",
            children: utils.getNavItemsForLanguage(contributingNavItems, "ar"),
          },
          {
            title: "أسئلة شائعة",
            children: utils.getNavItemsForLanguage(faqNavItems, "ar"),
          },
          {
            title: "المدونة (بالإنجليزية)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            title: "سجل التغييرات (بالإنجليزية)",
            children: [changelogNavItem],
          },
        ],
      },
    },
  },
  /**
   * Apply plugins，ref：https://v1.vuepress.vuejs.org/plugin/
   */
  plugins: [
    "@vuepress/plugin-back-to-top",
    "@vuepress/plugin-medium-zoom",
    "@vuepress/last-updated", // https://vuepress.vuejs.org/plugin/official/plugin-last-updated.html#vuepress-plugin-last-updated
    "vuepress-plugin-mermaidjs", // https://github.com/eFrane/vuepress-plugin-mermaidjs
    [
      "vuepress-plugin-code-copy", // https://github.com/znicholasbrown/vuepress-plugin-code-copy
      {
        color: "#3eaf7c",
      },
    ],
    [
      "seo", // https://github.com/lorisleiva/vuepress-plugin-seo
      {
        description: ($page) => $page.frontmatter.description || description,
        image: ($page, $site) => {
          if ($page.frontmatter.image) {
            return $site.themeConfig.domain &&
              !$page.frontmatter.image.startsWith("http")
              ? $site.themeConfig.domain + $page.frontmatter.image
              : $page.frontmatter.image;
          }

          return $site.themeConfig.domain + "/screenshots/screenshot.png";
        },
      },
    ],
    [
      "sitemap", // https://github.com/ekoeryanto/vuepress-plugin-sitemap
      {
        hostname: "https://www.qownnotes.org",
        exclude: ["/404.html"],
      },
    ],
    [
      "vuepress-plugin-matomo", // https://github.com/qdot/vuepress-plugin-matomo
      {
        siteId: 7,
        trackerUrl: "https://p.bekerle.com/",
        trackerJsFile: "matomo.js",
      },
    ],
    [
      "feed", // https://github.com/webmasterish/vuepress-plugin-feed
      {
        feed_options: {
          title: "QOwnNotes Blog",
          description:
            "News about QOwnNotes, the open source markdown note taking application for Linux, macOS and Windows, that works together with Nextcloud Notes",
        },
        canonical_base: "https://www.qownnotes.org",
        sort: (entries) => _.reverse(_.sortBy(entries, "date")),
      },
    ],
    [
      "vuepress-plugin-canonical", // https://github.com/IOriens/vuepress-plugin-canonical
      {
        baseURL: "https://www.qownnotes.org", // base url for your canonical link, optional, default: ''
      },
    ],
  ],
  markdown: {
    lineNumbers: true,
  },
  chainWebpack: (config, isServer) => {
    config.module.rules.delete("images");
    config.module
      .rule("images")
      .test(/\.(png|jpe?g|gif|webp)(\?.*)?$/)
      .use("file-loader")
      .loader("file-loader")
      .options({
        name: `assets/img/[name].[ext]`, // we don't want a hash in the image filename, so we can use the image as og:image
      });
  },
};
