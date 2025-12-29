import { defineUserConfig } from "vuepress";
import { viteBundler } from "@vuepress/bundler-vite";
import customTheme from "./theme/index.js";
import { createRequire } from "module";
import * as utils from "./utils.js";
import { docsearchPlugin } from "@vuepress/plugin-docsearch";
import { markdownChartPlugin } from "@vuepress/plugin-markdown-chart";
import { markdownExtPlugin } from "@vuepress/plugin-markdown-ext";
import { seoPlugin } from "@vuepress/plugin-seo";
import { sitemapPlugin } from "@vuepress/plugin-sitemap";
import { feedPlugin } from "@vuepress/plugin-feed";

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
    // DocSearch plugin - https://ecosystem.vuejs.press/plugins/search/docsearch.html
    docsearchPlugin({
      appId: "LP8PS829KJ",
      apiKey: "52d0b99615b7586dcd5a360a5cfe3f07",
      indexName: "qownnotes",
      locales: {
        "/": {
          placeholder: "Search Documentation",
          translations: {
            button: {
              buttonText: "Search",
              buttonAriaLabel: "Search",
            },
          },
        },
        "/de/": {
          placeholder: "Dokumentation durchsuchen",
          translations: {
            button: {
              buttonText: "Suchen",
              buttonAriaLabel: "Suchen",
            },
          },
        },
        "/it/": {
          placeholder: "Cerca documentazione",
          translations: {
            button: {
              buttonText: "Cerca",
              buttonAriaLabel: "Cerca",
            },
          },
        },
        "/fr/": {
          placeholder: "Rechercher dans la documentation",
          translations: {
            button: {
              buttonText: "Rechercher",
              buttonAriaLabel: "Rechercher",
            },
          },
        },
        "/es/": {
          placeholder: "Buscar documentación",
          translations: {
            button: {
              buttonText: "Buscar",
              buttonAriaLabel: "Buscar",
            },
          },
        },
        "/fa/": {
          placeholder: "جستجوی مستندات",
          translations: {
            button: {
              buttonText: "جستجو",
              buttonAriaLabel: "جستجو",
            },
          },
        },
        "/nl/": {
          placeholder: "Zoek documentatie",
          translations: {
            button: {
              buttonText: "Zoeken",
              buttonAriaLabel: "Zoeken",
            },
          },
        },
        "/hu/": {
          placeholder: "Dokumentáció keresése",
          translations: {
            button: {
              buttonText: "Keresés",
              buttonAriaLabel: "Keresés",
            },
          },
        },
        "/ar/": {
          placeholder: "البحث في الوثائق",
          translations: {
            button: {
              buttonText: "بحث",
              buttonAriaLabel: "بحث",
            },
          },
        },
        "/pl/": {
          placeholder: "Szukaj w dokumentacji",
          translations: {
            button: {
              buttonText: "Szukaj",
              buttonAriaLabel: "Szukaj",
            },
          },
        },
        "/ko/": {
          placeholder: "문서 검색",
          translations: {
            button: {
              buttonText: "검색",
              buttonAriaLabel: "검색",
            },
          },
        },
      },
    }),
    // Mermaid diagrams support (via chart plugin)
    markdownChartPlugin({
      // Enable Mermaid
      mermaid: true,
    }),
    // Task lists and other extensions
    markdownExtPlugin({
      tasklist: true,
    }),
    // SEO plugin - https://ecosystem.vuejs.press/plugins/seo/seo/
    // Doc: https://github.com/vuepress/ecosystem/blob/main/docs/plugins/seo/seo/config.md
    seoPlugin({
      hostname: "https://www.qownnotes.org",
      canonical: "https://www.qownnotes.org",
      author: {
        name: "Patrizio Bekerle",
        twitter: "PatrizioBekerle",
      },
      ogp: (ogp, page) => {
        // Helper function to get the image URL
        const getImageUrl = () => {
          return page.frontmatter.image
            ? page.frontmatter.image.startsWith("http")
              ? page.frontmatter.image
              : `https://www.qownnotes.org${page.frontmatter.image}`
            : "https://www.qownnotes.org/screenshots/screenshot.png";
        };

        const imageUrl = getImageUrl();

        return {
          ...ogp,
          "og:image": imageUrl,
          "twitter:card": "summary_large_image",
          "twitter:title": page.title,
          "twitter:description": page.frontmatter.description || "",
          "twitter:image": imageUrl,
          "twitter:creator": "@PatrizioBekerle",
        };
      },
    }),
    // Sitemap plugin - https://ecosystem.vuejs.press/plugins/seo/sitemap/
    sitemapPlugin({
      hostname: "https://www.qownnotes.org",
      excludeUrls: ["/404.html"],
    }),
    // Feed plugin - https://ecosystem.vuejs.press/plugins/blog/feed/
    // Doc: https://github.com/vuepress/ecosystem/blob/main/docs/plugins/blog/feed/config.md
    feedPlugin({
      hostname: "https://www.qownnotes.org",
      rss: true,
      atom: true,
      json: true,
      rssOutputFilename: "rss.xml",
      atomOutputFilename: "feed.atom",
      jsonOutputFilename: "feed.json",
      channel: {
        title: "QOwnNotes Blog",
        description:
          "News about QOwnNotes, the open source markdown note taking application for Linux, macOS and Windows, that works together with Nextcloud Notes",
      },
      filter: ({ frontmatter, filePathRelative }) => {
        // Only include blog posts in the feed
        return filePathRelative && filePathRelative.startsWith("blog/");
      },
      sorter: (a, b) => {
        // Sort by date, newest first
        return (
          new Date(b.data.date).getTime() - new Date(a.data.date).getTime()
        );
      },
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
          {
            text: "Getting started",
            collapsible: true,
            children: gettingStartedNavItems,
          },
          {
            text: "Installation",
            collapsible: true,
            children: installationNavItems,
          },
          { text: "Editor", collapsible: true, children: editorNavItems },
          { text: "Scripting", collapsible: true, children: scriptingNavItems },
          {
            text: "Contributing",
            collapsible: true,
            children: contributingNavItems,
          },
          { text: "FAQ", collapsible: true, children: faqNavItems },
          {
            text: "Blog",
            collapsible: true,
            children: utils.getChildren("src", "blog", true),
          },
          {
            text: "Changelog",
            collapsible: true,
            children: [changelogNavItem],
          },
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "de",
            ),
          },
          {
            text: "Installation",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "de"),
          },
          {
            text: "Editor",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "de"),
          },
          {
            text: "Skripten",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "de"),
          },
          {
            text: "Beitragen",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "de"),
          },
          {
            text: "FAQ",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "de"),
          },
          {
            text: "Blog (auf Englisch)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "Änderungsprotokoll (auf Englisch)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "it",
            ),
          },
          {
            text: "Installazione",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "it"),
          },
          {
            text: "Editor",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "it"),
          },
          {
            text: "Scripting",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "it"),
          },
          {
            text: "Contribuire",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "it"),
          },
          {
            text: "FAQ",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "it"),
          },
          {
            text: "Blog (in inglese)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "Changelog (in inglese)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "fr",
            ),
          },
          {
            text: "Installation",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "fr"),
          },
          {
            text: "Éditeur",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "fr"),
          },
          {
            text: "Scripting",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "fr"),
          },
          {
            text: "Contribuer",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "fr"),
          },
          {
            text: "FAQ",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "fr"),
          },
          {
            text: "Blog (en anglais)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "Changelog (en anglais)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "es",
            ),
          },
          {
            text: "Instalación",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "es"),
          },
          {
            text: "Editor",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "es"),
          },
          {
            text: "Scripting",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "es"),
          },
          {
            text: "Contribuir",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "es"),
          },
          {
            text: "FAQ",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "es"),
          },
          {
            text: "Blog (en inglés)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "Changelog (en inglés)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "fa",
            ),
          },
          {
            text: "نصب",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "fa"),
          },
          {
            text: "ویرایشگر",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "fa"),
          },
          {
            text: "اسکریپت نویسی",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "fa"),
          },
          {
            text: "مشارکت",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "fa"),
          },
          {
            text: "سوالات متداول",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "fa"),
          },
          {
            text: "وبلاگ (به انگلیسی)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "تغییرات (به انگلیسی)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "nl",
            ),
          },
          {
            text: "Installatie",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "nl"),
          },
          {
            text: "Editor",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "nl"),
          },
          {
            text: "Scripting",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "nl"),
          },
          {
            text: "Bijdragen",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "nl"),
          },
          {
            text: "FAQ",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "nl"),
          },
          {
            text: "Blog (in het Engels)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "Changelog (in het Engels)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "hu",
            ),
          },
          {
            text: "Telepítés",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "hu"),
          },
          {
            text: "Szerkesztő",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "hu"),
          },
          {
            text: "Szkriptelés",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "hu"),
          },
          {
            text: "Közreműködés",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "hu"),
          },
          {
            text: "GYIK",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "hu"),
          },
          {
            text: "Blog (angolul)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "Változásnapló (angolul)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "ar",
            ),
          },
          {
            text: "التثبيت",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "ar"),
          },
          {
            text: "المحرر",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "ar"),
          },
          {
            text: "البرمجة",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "ar"),
          },
          {
            text: "المساهمة",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "ar"),
          },
          {
            text: "الأسئلة الشائعة",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "ar"),
          },
          {
            text: "المدونة (بالإنجليزية)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "سجل التغييرات (بالإنجليزية)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "pl",
            ),
          },
          {
            text: "Instalacja",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "pl"),
          },
          {
            text: "Edytor",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "pl"),
          },
          {
            text: "Skrypty",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "pl"),
          },
          {
            text: "Współpraca",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "pl"),
          },
          {
            text: "FAQ",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "pl"),
          },
          {
            text: "Blog (po angielsku)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "Dziennik zmian (po angielsku)",
            collapsible: true,
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
            collapsible: true,
            children: utils.getNavItemsForLanguage(
              gettingStartedNavItems,
              "ko",
            ),
          },
          {
            text: "설치",
            collapsible: true,
            children: utils.getNavItemsForLanguage(installationNavItems, "ko"),
          },
          {
            text: "편집기",
            collapsible: true,
            children: utils.getNavItemsForLanguage(editorNavItems, "ko"),
          },
          {
            text: "스크립팅",
            collapsible: true,
            children: utils.getNavItemsForLanguage(scriptingNavItems, "ko"),
          },
          {
            text: "기여하기",
            collapsible: true,
            children: utils.getNavItemsForLanguage(contributingNavItems, "ko"),
          },
          {
            text: "자주 묻는 질문",
            collapsible: true,
            children: utils.getNavItemsForLanguage(faqNavItems, "ko"),
          },
          {
            text: "블로그 (영어)",
            collapsible: true,
            children: utils.getBlogChildrenWithTitles("src", "blog", true),
          },
          {
            text: "변경 로그 (영어)",
            collapsible: true,
            children: [changelogNavItem],
          },
        ],
      },
    },
  }),
});
