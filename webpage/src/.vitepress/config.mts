import { defineConfig } from 'vitepress'
import { description } from '../../package.json'
import { sitemap } from './plugins/sitemap'

// Shared contact items across all locales
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
]

const gettingStartedItems = [
  { text: "Overview", link: "/getting-started/overview" },
  { text: "Concept", link: "/getting-started/concept" },
  { text: "Demo", link: "/getting-started/demo" },
  { text: "Markdown", link: "/getting-started/markdown" },
  { text: "Importing Notes", link: "/getting-started/importing-notes" },
  { text: "CLI Parameters", link: "/getting-started/cli-parameters" },
  { text: "Browser Extension", link: "/getting-started/browser-extension" },
  { text: "Command Line Snippet Manager", link: "/getting-started/command-line-snippet-manager" },
  { text: "Web App", link: "/getting-started/web-app" },
  { text: "QOwnNotesAPI", link: "/getting-started/qownnotesapi" },
  { text: "Note Relations", link: "/getting-started/note-relations" },
  { text: "Searching", link: "/getting-started/searching" },
  { text: "Shortcuts", link: "/getting-started/shortcuts" },
  { text: "Git Versioning", link: "/getting-started/git-versioning" },
]

const installationItems = [
  { text: "Installation", link: "/installation/" },
  { text: "Ubuntu Linux", link: "/installation/ubuntu" },
  { text: "Windows", link: "/installation/windows" },
  { text: "macOS", link: "/installation/macos" },
  { text: "Debian Linux", link: "/installation/debian" },
  { text: "openSUSE Linux", link: "/installation/opensuse" },
  { text: "Fedora Linux", link: "/installation/fedora" },
  { text: "Snap", link: "/installation/snap" },
  { text: "Flatpak", link: "/installation/flatpak" },
  { text: "AppImage", link: "/installation/appimage" },
  { text: "Nix", link: "/installation/nix" },
  { text: "Arch Linux", link: "/installation/arch" },
  { text: "Solus", link: "/installation/solus" },
  { text: "KaOS", link: "/installation/kaos" },
  { text: "CentOS", link: "/installation/centos" },
  { text: "Raspberry Pi OS", link: "/installation/raspberry-pi-os" },
  { text: "Gentoo", link: "/installation/gentoo" },
  { text: "Funtoo", link: "/installation/funtoo" },
  { text: "Void Linux", link: "/installation/void" },
  { text: "Slackware", link: "/installation/slackware" },
  { text: "FreeBSD", link: "/installation/freebsd" },
  { text: "Building from Source", link: "/installation/building" },
]

const editorItems = [
  { text: "Spellchecking", link: "/editor/spellchecking" },
  { text: "AI", link: "/editor/ai" },
  { text: "Time Formats", link: "/editor/time-formats" },
]

const scriptingItems = [
  { text: "Scripting", link: "/scripting/" },
  { text: "Methods and Objects", link: "/scripting/methods-and-objects" },
  { text: "Hooks", link: "/scripting/hooks" },
  { text: "Classes", link: "/scripting/classes" },
]

const contributingItems = [
  { text: "Get Involved", link: "/contributing/get-involved" },
  { text: "Code of Conduct", link: "/contributing/code-of-conduct" },
  { text: "Maintainers", link: "/contributing/maintainers" },
  { text: "Translation", link: "/contributing/translation" },
  { text: "Donate", link: "/contributing/donate" },
  { text: "Sponsors", link: "/contributing/sponsors" },
]

const faqItems = [
  { text: "Preview without Edit", link: "/faq/preview-without-edit" },
  { text: "References", link: "/faq/references" },
  { text: "Metrics", link: "/faq/metrics" },
]

// Helper to create localized nav items
function createLocalizedItems(items: any[], locale: string) {
  return items.map(item => ({
    text: item.text,
    link: `/${locale}${item.link}`
  }))
}

export default defineConfig({
  title: "QOwnNotes",
  description: description,

  head: [
    ["meta", { name: "theme-color", content: "#3eaf7c" }],
    ["meta", { name: "apple-mobile-web-app-capable", content: "yes" }],
    ["meta", { name: "apple-mobile-web-app-status-bar-style", content: "black" }],
    ["meta", { name: "viewport", content: "width=device-width, initial-scale=1.0" }],
    ["link", { rel: "icon", type: "image/png", sizes: "128x128", href: "/favicon.png" }],
    ["link", { rel: "stylesheet", href: "https://cdn.jsdelivr.net/npm/@mdi/font@5.x/css/materialdesignicons.min.css" }],
    ["link", { rel: "alternate", title: "QOwnNotes Releases RSS", type: "application/rss+xml", href: "https://feeds.feedburner.com/QOwnNotesReleases" }],
    ["link", { rel: "alternate", title: "QOwnNotes Blog RSS", type: "application/rss+xml", href: "https://feeds.feedburner.com/QOwnNotesBlog" }],
    ["link", { rel: "me", href: "https://social.qownnotes.org/@patrizio" }],
    ["link", { rel: "me", href: "https://social.qownnotes.org/@qownnotes" }],
  ],

  locales: {
    root: {
      label: "English",
      lang: "en-US",
      themeConfig: {
        nav: [
          { text: "Getting Started", link: "/getting-started/overview" },
          { text: "Installation", link: "/installation/" },
          {
            text: "Contact",
            items: contactNavItems
          },
          { text: "Donate", link: "/contributing/donate" },
        ],
        sidebar: {
          '/getting-started/': [
            {
              text: "Getting Started",
              items: gettingStartedItems,
            },
          ],
          '/installation/': [
            {
              text: "Installation",
              items: installationItems,
            },
          ],
          '/editor/': [
            {
              text: "Editor",
              items: editorItems,
            },
          ],
          '/scripting/': [
            {
              text: "Scripting",
              items: scriptingItems,
            },
          ],
          '/contributing/': [
            {
              text: "Contributing",
              items: contributingItems,
            },
          ],
          '/faq/': [
            {
              text: "FAQ",
              items: faqItems,
            },
          ],
          '/': [
            {
              text: "Getting Started",
              items: gettingStartedItems,
            },
            {
              text: "Installation",
              items: installationItems,
            },
            {
              text: "Editor",
              items: editorItems,
            },
            {
              text: "Scripting",
              items: scriptingItems,
            },
            {
              text: "Contributing",
              items: contributingItems,
            },
            {
              text: "FAQ",
              items: faqItems,
            },
            {
              text: "Changelog",
              items: [{ text: "Changelog", link: "/changelog" }],
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'Help us improve this page!'
        },
        search: {
          provider: 'algolia',
          options: {
            appId: 'LP8PS829KJ',
            apiKey: '52d0b99615b7586dcd5a360a5cfe3f07',
            indexName: 'qownnotes'
          }
        },
        socialLinks: [
          { icon: 'github', link: 'https://github.com/pbek/QOwnNotes' },
          { icon: 'mastodon', link: 'https://social.qownnotes.org/@qownnotes' },
          { icon: 'twitter', link: 'https://twitter.com/QOwnNotes' },
          { icon: 'facebook', link: 'https://www.facebook.com/QOwnNotes' },
        ],
      },
    },
    de: {
      label: "Deutsch",
      lang: "de-DE",
      themeConfig: {
        nav: [
          { text: "Loslegen", link: "/de/getting-started/overview" },
          { text: "Installation", link: "/de/installation/" },
          { text: "Kontakt", items: contactNavItems },
          { text: "Spenden", link: "/de/contributing/donate" },
        ],
        sidebar: {
          '/de/': [
            {
              text: "Loslegen",
              items: createLocalizedItems(gettingStartedItems, 'de'),
            },
            {
              text: "Installation",
              items: createLocalizedItems(installationItems, 'de'),
            },
            {
              text: "Editor",
              items: createLocalizedItems(editorItems, 'de'),
            },
            {
              text: "Skripten",
              items: createLocalizedItems(scriptingItems, 'de'),
            },
            {
              text: "Beitragen",
              items: createLocalizedItems(contributingItems, 'de'),
            },
            {
              text: "FAQ",
              items: createLocalizedItems(faqItems, 'de'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'Hilf uns diese Seite zu verbessern!'
        },
      },
    },
    es: {
      label: "Español",
      lang: "es-ES",
      themeConfig: {
        nav: [
          { text: "Empezando", link: "/es/getting-started/overview" },
          { text: "Instalación", link: "/es/installation/" },
          { text: "Contacto", items: contactNavItems },
          { text: "Donar", link: "/es/contributing/donate" },
        ],
        sidebar: {
          '/es/': [
            {
              text: "Empezando",
              items: createLocalizedItems(gettingStartedItems, 'es'),
            },
            {
              text: "Instalación",
              items: createLocalizedItems(installationItems, 'es'),
            },
            {
              text: "Editor",
              items: createLocalizedItems(editorItems, 'es'),
            },
            {
              text: "Guiones",
              items: createLocalizedItems(scriptingItems, 'es'),
            },
            {
              text: "Contribuir",
              items: createLocalizedItems(contributingItems, 'es'),
            },
            {
              text: "FAQ",
              items: createLocalizedItems(faqItems, 'es'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: '¡Ayúdanos a mejorar este sitio!'
        },
      },
    },
    fr: {
      label: "Français",
      lang: "fr-FR",
      themeConfig: {
        nav: [
          { text: "Commencer", link: "/fr/getting-started/overview" },
          { text: "Installation", link: "/fr/installation/" },
          { text: "Contact", items: contactNavItems },
          { text: "Faire un don", link: "/fr/contributing/donate" },
        ],
        sidebar: {
          '/fr/': [
            {
              text: "Commencer",
              items: createLocalizedItems(gettingStartedItems, 'fr'),
            },
            {
              text: "Installation",
              items: createLocalizedItems(installationItems, 'fr'),
            },
            {
              text: "Éditeur",
              items: createLocalizedItems(editorItems, 'fr'),
            },
            {
              text: "Les scripts",
              items: createLocalizedItems(scriptingItems, 'fr'),
            },
            {
              text: "Contribuer",
              items: createLocalizedItems(contributingItems, 'fr'),
            },
            {
              text: "FAQ",
              items: createLocalizedItems(faqItems, 'fr'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'Aidez-nous à améliorer ce site!'
        },
      },
    },
    it: {
      label: "Italiano",
      lang: "it-IT",
      themeConfig: {
        nav: [
          { text: "Iniziare", link: "/it/getting-started/overview" },
          { text: "Installazione", link: "/it/installation/" },
          { text: "Contatto", items: contactNavItems },
          { text: "Donare", link: "/it/contributing/donate" },
        ],
        sidebar: {
          '/it/': [
            {
              text: "Iniziare",
              items: createLocalizedItems(gettingStartedItems, 'it'),
            },
            {
              text: "Installazione",
              items: createLocalizedItems(installationItems, 'it'),
            },
            {
              text: "Editor",
              items: createLocalizedItems(editorItems, 'it'),
            },
            {
              text: "Script",
              items: createLocalizedItems(scriptingItems, 'it'),
            },
            {
              text: "Contribuire",
              items: createLocalizedItems(contributingItems, 'it'),
            },
            {
              text: "FAQ",
              items: createLocalizedItems(faqItems, 'it'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'Aiutaci a migliorare questo sito!'
        },
      },
    },
    fa: {
      label: "فارسی",
      lang: "fa-IR",
      dir: "rtl",
      themeConfig: {
        nav: [
          { text: "شروع", link: "/fa/getting-started/overview" },
          { text: "نصب", link: "/fa/installation/" },
          { text: "تماس", items: contactNavItems },
          { text: "کمک مالی", link: "/fa/contributing/donate" },
        ],
        sidebar: {
          '/fa/': [
            {
              text: "شروع",
              items: createLocalizedItems(gettingStartedItems, 'fa'),
            },
            {
              text: "نصب و راه اندازی",
              items: createLocalizedItems(installationItems, 'fa'),
            },
            {
              text: "ویرایشگر",
              items: createLocalizedItems(editorItems, 'fa'),
            },
            {
              text: "اسکریپت نویسی",
              items: createLocalizedItems(scriptingItems, 'fa'),
            },
            {
              text: "مشارکت",
              items: createLocalizedItems(contributingItems, 'fa'),
            },
            {
              text: "سؤالات متداول",
              items: createLocalizedItems(faqItems, 'fa'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'در اصلاح این صفحه ما را یاری کنید!'
        },
      },
    },
    nl: {
      label: "Nederlands",
      lang: "nl-NL",
      themeConfig: {
        nav: [
          { text: "Aan de slag", link: "/nl/getting-started/overview" },
          { text: "Installatie", link: "/nl/installation/" },
          { text: "Contact", items: contactNavItems },
          { text: "Doneren", link: "/nl/contributing/donate" },
        ],
        sidebar: {
          '/nl/': [
            {
              text: "Aan de slag",
              items: createLocalizedItems(gettingStartedItems, 'nl'),
            },
            {
              text: "Installatie",
              items: createLocalizedItems(installationItems, 'nl'),
            },
            {
              text: "Editor",
              items: createLocalizedItems(editorItems, 'nl'),
            },
            {
              text: "Skripten",
              items: createLocalizedItems(scriptingItems, 'nl'),
            },
            {
              text: "Bijdragen",
              items: createLocalizedItems(contributingItems, 'nl'),
            },
            {
              text: "FAQ",
              items: createLocalizedItems(faqItems, 'nl'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'Help ons deze pagina te verbeteren!'
        },
      },
    },
    hu: {
      label: "Magyar",
      lang: "hu-HU",
      themeConfig: {
        nav: [
          { text: "Az első lépések", link: "/hu/getting-started/overview" },
          { text: "Telepítés", link: "/hu/installation/" },
          { text: "Kapcsolat", items: contactNavItems },
          { text: "Adomány", link: "/hu/contributing/donate" },
        ],
        sidebar: {
          '/hu/': [
            {
              text: "Az első lépések",
              items: createLocalizedItems(gettingStartedItems, 'hu'),
            },
            {
              text: "Telepítés",
              items: createLocalizedItems(installationItems, 'hu'),
            },
            {
              text: "Szerkesztő",
              items: createLocalizedItems(editorItems, 'hu'),
            },
            {
              text: "Szkriptek",
              items: createLocalizedItems(scriptingItems, 'hu'),
            },
            {
              text: "Adomány",
              items: createLocalizedItems(contributingItems, 'hu'),
            },
            {
              text: "FAQ",
              items: createLocalizedItems(faqItems, 'hu'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'Segítsen javítani ezt az oldalt!'
        },
      },
    },
    ar: {
      label: "العربية",
      lang: "ar-AR",
      dir: "rtl",
      themeConfig: {
        nav: [
          { text: "بدء الاستخدام", link: "/ar/getting-started/overview" },
          { text: "التثبيت", link: "/ar/installation/" },
          { text: "تواصل", items: contactNavItems },
          { text: "تبرع", link: "/ar/contributing/donate" },
        ],
        sidebar: {
          '/ar/': [
            {
              text: "بدء الاستخدام",
              items: createLocalizedItems(gettingStartedItems, 'ar'),
            },
            {
              text: "التثبيت",
              items: createLocalizedItems(installationItems, 'ar'),
            },
            {
              text: "المحرر",
              items: createLocalizedItems(editorItems, 'ar'),
            },
            {
              text: "البرمجة",
              items: createLocalizedItems(scriptingItems, 'ar'),
            },
            {
              text: "مساهمة",
              items: createLocalizedItems(contributingItems, 'ar'),
            },
            {
              text: "أسئلة شائعة",
              items: createLocalizedItems(faqItems, 'ar'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'ساعدنا في تحسين هذه الصفحة!'
        },
      },
    },
    pl: {
      label: "Polski",
      lang: "pl-PL",
      themeConfig: {
        nav: [
          { text: "Pierwsze kroki", link: "/pl/getting-started/overview" },
          { text: "Instalacja", link: "/pl/installation/" },
          { text: "Kontakt", items: contactNavItems },
          { text: "Wpłać", link: "/pl/contributing/donate" },
        ],
        sidebar: {
          '/pl/': [
            {
              text: "Pierwsze kroki",
              items: createLocalizedItems(gettingStartedItems, 'pl'),
            },
            {
              text: "Instalacja",
              items: createLocalizedItems(installationItems, 'pl'),
            },
            {
              text: "Editor",
              items: createLocalizedItems(editorItems, 'pl'),
            },
            {
              text: "Skrypty",
              items: createLocalizedItems(scriptingItems, 'pl'),
            },
            {
              text: "Wkład",
              items: createLocalizedItems(contributingItems, 'pl'),
            },
            {
              text: "FAQ",
              items: createLocalizedItems(faqItems, 'pl'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: 'Pomóż nam ulepszyć tę stronę!'
        },
      },
    },
    ko: {
      label: "한국어",
      lang: "ko-KR",
      themeConfig: {
        nav: [
          { text: "시작하기", link: "/ko/getting-started/overview" },
          { text: "설치", link: "/ko/installation/" },
          { text: "연락하다", items: contactNavItems },
          { text: "기부하다", link: "/ko/contributing/donate" },
        ],
        sidebar: {
          '/ko/': [
            {
              text: "시작하기",
              items: createLocalizedItems(gettingStartedItems, 'ko'),
            },
            {
              text: "설치",
              items: createLocalizedItems(installationItems, 'ko'),
            },
            {
              text: "편집자",
              items: createLocalizedItems(editorItems, 'ko'),
            },
            {
              text: "스크립팅",
              items: createLocalizedItems(scriptingItems, 'ko'),
            },
            {
              text: "기여",
              items: createLocalizedItems(contributingItems, 'ko'),
            },
            {
              text: "자주하는 질문",
              items: createLocalizedItems(faqItems, 'ko'),
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/pbek/QOwnNotes/edit/main/webpage/src/:path',
          text: '이 페이지를 개선할 수 있도록 도와주세요!'
        },
      },
    },
  },

  themeConfig: {
    logo: "/favicon.png",
    siteTitle: "QOwnNotes",
  },

  // Markdown configuration
  markdown: {
    lineNumbers: true,
  },

  // Build output directory
  outDir: '.vitepress/dist',

  // Clean URLs (removes .html extension)
  cleanUrls: true,

  // Sitemap plugin
  transformPageData(pageData) {
    sitemap.transformPageData(pageData)
  },

  async buildEnd(config) {
    await sitemap.buildEnd(config)
  },

  // SEO - Add Open Graph and Twitter Card meta tags
  transformHead({ pageData }) {
    const head = []

    const ogUrl = `https://www.qownnotes.org/${pageData.relativePath.replace(/((^|\/)index)?\.md$/, '$2')}`
    const ogImage = pageData.frontmatter.image
      ? (pageData.frontmatter.image.startsWith('http')
          ? pageData.frontmatter.image
          : `https://www.qownnotes.org${pageData.frontmatter.image}`)
      : 'https://www.qownnotes.org/screenshots/screenshot.png'

    head.push(['meta', { property: 'og:url', content: ogUrl }])
    head.push(['meta', { property: 'og:type', content: 'website' }])
    head.push(['meta', { property: 'og:title', content: pageData.frontmatter.title || pageData.title }])
    head.push(['meta', { property: 'og:description', content: pageData.frontmatter.description || description }])
    head.push(['meta', { property: 'og:image', content: ogImage }])

    head.push(['meta', { name: 'twitter:card', content: 'summary_large_image' }])
    head.push(['meta', { name: 'twitter:site', content: '@QOwnNotes' }])
    head.push(['meta', { name: 'twitter:creator', content: '@PatrizioBekerle' }])

    head.push(['link', { rel: 'canonical', href: ogUrl }])

    return head
  },
})

