import { defineUserConfig } from "vuepress";
import { viteBundler } from "@vuepress/bundler-vite";
import customTheme from "./theme/index.js";
import { createRequire } from "module";
import * as utils from "./utils.js";

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

  bundler: viteBundler(),

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
      // Add other locales similarly...
    },
  }),

  markdown: {
    code: {
      lineNumbers: true,
    },
  },
});
