import { defineUserConfig } from "vuepress";
import { viteBundler } from "@vuepress/bundler-vite";
import customTheme from "./theme/index.js";
import { createRequire } from "module";
import * as utils from "./utils.js";
import { markdownChartPlugin } from "@vuepress/plugin-markdown-chart";
import { markdownExtPlugin } from "@vuepress/plugin-markdown-ext";

const require = createRequire(import.meta.url);
const { description } = require("../../package.json");

// Navigation items
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

export default defineUserConfig({
  lang: "en-US",
  title: "QOwnNotes",
  description: description,

  // Disable SSR preloading to avoid CSS import issues
  shouldPreload: false,
  shouldPrefetch: false,

  // Configure bundler - handle Vuetify for SSR
  bundler: viteBundler({
    viteOptions: {
      ssr: {
        noExternal: ["vuetify"],
      },
    },
  }),

  // Plugins
  plugins: [
    // Mermaid diagrams support (via chart plugin)
    markdownChartPlugin({
      // Enable Mermaid
      mermaid: true,
    }),
    // Task lists and other extensions
    markdownExtPlugin({
      tasklist: true,
    }),
  ],

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
        href: "https://cdn.jsdelivr.net/npm/@mdi/font@7.x/css/materialdesignicons.min.css",
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
  ],

  locales: {
    "/": { lang: "en-US", title: "QOwnNotes" },
    "/de/": { lang: "de-DE", title: "QOwnNotes" },
    "/it/": { lang: "it-IT", title: "QOwnNotes" },
    "/fr/": { lang: "fr-FR", title: "QOwnNotes" },
    "/es/": { lang: "es-ES", title: "QOwnNotes" },
    "/fa/": { lang: "fa-IR", title: "QOwnNotes" },
    "/nl/": { lang: "nl-NL", title: "QOwnNotes" },
    "/hu/": { lang: "hu-HU", title: "QOwnNotes" },
    "/ar/": { lang: "ar-AR", title: "QOwnNotes" },
    "/pl/": { lang: "pl-PL", title: "QOwnNotes" },
    "/ko/": { lang: "ko-KR", title: "QOwnNotes" },
  },

  theme: customTheme({
    repo: "pbek/QOwnNotes",
    editLink: true,
    docsBranch: "main",
    docsDir: "webpage/src",
    editLinkText: "Help us improve this page!",
    lastUpdated: false,
    logo: "/favicon.png",

    locales: {
      "/": {
        selectLanguageName: "English",
        selectLanguageText: "🌍",
        navbar: [
          { text: "Getting started", link: gettingStartedNavItem },
          { text: "Installation", link: installationNavItem },
          { text: "Contact", children: contactNavItems },
          { text: "Donate", link: donateNavItem },
        ],
        sidebar: [
          { text: "Getting started", children: gettingStartedNavItems },
          { text: "Installation", children: installationNavItems },
          { text: "Editor", children: editorNavItems },
          { text: "Scripting", children: scriptingNavItems },
          { text: "Contributing", children: contributingNavItems },
          { text: "FAQ", children: faqNavItems },
          { text: "Blog", children: utils.getChildren("src", "blog", true) },
          { text: "Changelog", children: [changelogNavItem] },
        ],
      },
      "/de/": {
        selectLanguageName: "Deutsch",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "Loslegen",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "de"),
          },
          {
            text: "Installation",
            link: utils.getNavItemForLanguage(installationNavItem, "de"),
          },
          { text: "Kontakt", children: contactNavItems },
          {
            text: "Spenden",
            link: utils.getNavItemForLanguage(donateNavItem, "de"),
          },
        ],
        sidebar: [
          {
            text: "Loslegen",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "de",
            ),
          },
          {
            text: "Installation",
            children: utils.getNavItemsForLanguage(installationNavItems, "de"),
          },
          {
            text: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "de"),
          },
          {
            text: "Skripten",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "de"),
          },
          {
            text: "Beitragen",
            children: utils.getNavItemsForLanguage(contributingNavItems, "de"),
          },
          {
            text: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "de"),
          },
          {
            text: "Blog (auf Englisch)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Änderungsprotokoll (auf Englisch)",
            children: [changelogNavItem],
          },
        ],
      },
      "/it/": {
        selectLanguageName: "Italiano",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "Iniziare",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "it"),
          },
          {
            text: "Installazione",
            link: utils.getNavItemForLanguage(installationNavItem, "it"),
          },
          { text: "Contatto", children: contactNavItems },
          {
            text: "Donare",
            link: utils.getNavItemForLanguage(donateNavItem, "it"),
          },
        ],
        sidebar: [
          {
            text: "Iniziare",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "it",
            ),
          },
          {
            text: "Installazione",
            children: utils.getNavItemsForLanguage(installationNavItems, "it"),
          },
          {
            text: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "it"),
          },
          {
            text: "Scripting",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "it"),
          },
          {
            text: "Contribuire",
            children: utils.getNavItemsForLanguage(contributingNavItems, "it"),
          },
          {
            text: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "it"),
          },
          {
            text: "Blog (in inglese)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Changelog (in inglese)",
            children: [changelogNavItem],
          },
        ],
      },
      "/fr/": {
        selectLanguageName: "Français",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "Commencer",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "fr"),
          },
          {
            text: "Installation",
            link: utils.getNavItemForLanguage(installationNavItem, "fr"),
          },
          { text: "Contact", children: contactNavItems },
          {
            text: "Faire un don",
            link: utils.getNavItemForLanguage(donateNavItem, "fr"),
          },
        ],
        sidebar: [
          {
            text: "Commencer",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "fr",
            ),
          },
          {
            text: "Installation",
            children: utils.getNavItemsForLanguage(installationNavItems, "fr"),
          },
          {
            text: "Éditeur",
            children: utils.getNavItemsForLanguage(editorNavItems, "fr"),
          },
          {
            text: "Scripting",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "fr"),
          },
          {
            text: "Contribuer",
            children: utils.getNavItemsForLanguage(contributingNavItems, "fr"),
          },
          {
            text: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "fr"),
          },
          {
            text: "Blog (en anglais)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Changelog (en anglais)",
            children: [changelogNavItem],
          },
        ],
      },
      "/es/": {
        selectLanguageName: "Español",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "Empezar",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "es"),
          },
          {
            text: "Instalación",
            link: utils.getNavItemForLanguage(installationNavItem, "es"),
          },
          { text: "Contacto", children: contactNavItems },
          {
            text: "Donar",
            link: utils.getNavItemForLanguage(donateNavItem, "es"),
          },
        ],
        sidebar: [
          {
            text: "Empezar",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "es",
            ),
          },
          {
            text: "Instalación",
            children: utils.getNavItemsForLanguage(installationNavItems, "es"),
          },
          {
            text: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "es"),
          },
          {
            text: "Scripting",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "es"),
          },
          {
            text: "Contribuir",
            children: utils.getNavItemsForLanguage(contributingNavItems, "es"),
          },
          {
            text: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "es"),
          },
          {
            text: "Blog (en inglés)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Changelog (en inglés)",
            children: [changelogNavItem],
          },
        ],
      },
      "/fa/": {
        selectLanguageName: "فارسی",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "شروع",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "fa"),
          },
          {
            text: "نصب",
            link: utils.getNavItemForLanguage(installationNavItem, "fa"),
          },
          { text: "تماس", children: contactNavItems },
          {
            text: "حمایت مالی",
            link: utils.getNavItemForLanguage(donateNavItem, "fa"),
          },
        ],
        sidebar: [
          {
            text: "شروع",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "fa",
            ),
          },
          {
            text: "نصب",
            children: utils.getNavItemsForLanguage(installationNavItems, "fa"),
          },
          {
            text: "ویرایشگر",
            children: utils.getNavItemsForLanguage(editorNavItems, "fa"),
          },
          {
            text: "اسکریپت نویسی",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "fa"),
          },
          {
            text: "مشارکت",
            children: utils.getNavItemsForLanguage(contributingNavItems, "fa"),
          },
          {
            text: "سوالات متداول",
            children: utils.getNavItemsForLanguage(faqNavItems, "fa"),
          },
          {
            text: "وبلاگ (به انگلیسی)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "تغییرات (به انگلیسی)",
            children: [changelogNavItem],
          },
        ],
      },
      "/nl/": {
        selectLanguageName: "Nederlands",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "Aan de slag",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "nl"),
          },
          {
            text: "Installatie",
            link: utils.getNavItemForLanguage(installationNavItem, "nl"),
          },
          { text: "Contact", children: contactNavItems },
          {
            text: "Doneren",
            link: utils.getNavItemForLanguage(donateNavItem, "nl"),
          },
        ],
        sidebar: [
          {
            text: "Aan de slag",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "nl",
            ),
          },
          {
            text: "Installatie",
            children: utils.getNavItemsForLanguage(installationNavItems, "nl"),
          },
          {
            text: "Editor",
            children: utils.getNavItemsForLanguage(editorNavItems, "nl"),
          },
          {
            text: "Scripting",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "nl"),
          },
          {
            text: "Bijdragen",
            children: utils.getNavItemsForLanguage(contributingNavItems, "nl"),
          },
          {
            text: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "nl"),
          },
          {
            text: "Blog (in het Engels)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Changelog (in het Engels)",
            children: [changelogNavItem],
          },
        ],
      },
      "/hu/": {
        selectLanguageName: "Magyar",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "Kezdő lépések",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "hu"),
          },
          {
            text: "Telepítés",
            link: utils.getNavItemForLanguage(installationNavItem, "hu"),
          },
          { text: "Kapcsolat", children: contactNavItems },
          {
            text: "Adományozás",
            link: utils.getNavItemForLanguage(donateNavItem, "hu"),
          },
        ],
        sidebar: [
          {
            text: "Kezdő lépések",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "hu",
            ),
          },
          {
            text: "Telepítés",
            children: utils.getNavItemsForLanguage(installationNavItems, "hu"),
          },
          {
            text: "Szerkesztő",
            children: utils.getNavItemsForLanguage(editorNavItems, "hu"),
          },
          {
            text: "Szkriptelés",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "hu"),
          },
          {
            text: "Közreműködés",
            children: utils.getNavItemsForLanguage(contributingNavItems, "hu"),
          },
          {
            text: "GYIK",
            children: utils.getNavItemsForLanguage(faqNavItems, "hu"),
          },
          {
            text: "Blog (angolul)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Változásnapló (angolul)",
            children: [changelogNavItem],
          },
        ],
      },
      "/ar/": {
        selectLanguageName: "العربية",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "البدء",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "ar"),
          },
          {
            text: "التثبيت",
            link: utils.getNavItemForLanguage(installationNavItem, "ar"),
          },
          { text: "اتصل", children: contactNavItems },
          {
            text: "تبرع",
            link: utils.getNavItemForLanguage(donateNavItem, "ar"),
          },
        ],
        sidebar: [
          {
            text: "البدء",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "ar",
            ),
          },
          {
            text: "التثبيت",
            children: utils.getNavItemsForLanguage(installationNavItems, "ar"),
          },
          {
            text: "المحرر",
            children: utils.getNavItemsForLanguage(editorNavItems, "ar"),
          },
          {
            text: "البرمجة",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "ar"),
          },
          {
            text: "المساهمة",
            children: utils.getNavItemsForLanguage(contributingNavItems, "ar"),
          },
          {
            text: "الأسئلة الشائعة",
            children: utils.getNavItemsForLanguage(faqNavItems, "ar"),
          },
          {
            text: "المدونة (بالإنجليزية)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "سجل التغييرات (بالإنجليزية)",
            children: [changelogNavItem],
          },
        ],
      },
      "/pl/": {
        selectLanguageName: "Polski",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "Rozpoczęcie",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "pl"),
          },
          {
            text: "Instalacja",
            link: utils.getNavItemForLanguage(installationNavItem, "pl"),
          },
          { text: "Kontakt", children: contactNavItems },
          {
            text: "Wesprzyj",
            link: utils.getNavItemForLanguage(donateNavItem, "pl"),
          },
        ],
        sidebar: [
          {
            text: "Rozpoczęcie",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "pl",
            ),
          },
          {
            text: "Instalacja",
            children: utils.getNavItemsForLanguage(installationNavItems, "pl"),
          },
          {
            text: "Edytor",
            children: utils.getNavItemsForLanguage(editorNavItems, "pl"),
          },
          {
            text: "Skrypty",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "pl"),
          },
          {
            text: "Współpraca",
            children: utils.getNavItemsForLanguage(contributingNavItems, "pl"),
          },
          {
            text: "FAQ",
            children: utils.getNavItemsForLanguage(faqNavItems, "pl"),
          },
          {
            text: "Blog (po angielsku)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Dziennik zmian (po angielsku)",
            children: [changelogNavItem],
          },
        ],
      },
      "/ko/": {
        selectLanguageName: "한국어",
        selectLanguageText: "🌍",
        navbar: [
          {
            text: "시작하기",
            link: utils.getNavItemForLanguage(gettingStartedNavItem, "ko"),
          },
          {
            text: "설치",
            link: utils.getNavItemForLanguage(installationNavItem, "ko"),
          },
          { text: "연락처", children: contactNavItems },
          {
            text: "후원",
            link: utils.getNavItemForLanguage(donateNavItem, "ko"),
          },
        ],
        sidebar: [
          {
            text: "시작하기",
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "ko",
            ),
          },
          {
            text: "설치",
            children: utils.getNavItemsForLanguage(installationNavItems, "ko"),
          },
          {
            text: "편집기",
            children: utils.getNavItemsForLanguage(editorNavItems, "ko"),
          },
          {
            text: "스크립팅",
            children: utils.getNavItemsForLanguage(scriptingNavItems, "ko"),
          },
          {
            text: "기여하기",
            children: utils.getNavItemsForLanguage(contributingNavItems, "ko"),
          },
          {
            text: "자주 묻는 질문",
            children: utils.getNavItemsForLanguage(faqNavItems, "ko"),
          },
          {
            text: "블로그 (영어)",
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "변경 로그 (영어)",
            children: [changelogNavItem],
          },
        ],
      },
    },
  }),
});
