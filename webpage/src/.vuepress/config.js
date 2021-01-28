const { description } = require('../../package')
const _ = require('lodash');
const utils = require('./utils');

// We don't want translation for these
const contactNavItems = [
  { text: 'Ask question', link: 'https://github.com/pbek/QOwnNotes/issues#support' },
  { text: 'Ask for feature', link: 'https://github.com/pbek/QOwnNotes/issues#feature' },
  { text: 'Report bug', link: 'https://github.com/pbek/QOwnNotes/issues#bug' },
  { text: 'Telegram Channel', link: 'https://t.me/QOwnNotes' },
  { text: 'Matrix/Element.io Room', link: 'https://app.element.io/#/room/#qownnotes:matrix.org' },
  { text: 'Gitter Chat', link: 'https://gitter.im/qownnotes/qownnotes' },
  { text: 'IRC Channel', link: 'https://kiwiirc.com/client/irc.freenode.net/#qownnotes' },
  { text: 'IRC Log', link: 'https://quodlibet.duckdns.org/irc/qownnotes/latest.log.html' },
];

const gettingStartedNavItems = [
  'getting-started/overview',
  'getting-started/concept',
  'getting-started/demo',
  'getting-started/markdown',
  'getting-started/importing-notes',
  'getting-started/cli-parameters',
  'getting-started/browser-extension',
  'getting-started/qownnotesapi',
  'getting-started/searching',
  'getting-started/shortcuts',
];

const installationNavItems = [
  'installation/',
  'installation/ubuntu',
  'installation/windows',
  'installation/macos',
  'installation/debian',
  'installation/opensuse',
  'installation/fedora',
  'installation/snap',
  'installation/flatpak',
  'installation/appimage',
  'installation/arch',
  'installation/solus',
  'installation/kaos',
  'installation/centos',
  'installation/raspberry-pi-os',
  'installation/gentoo',
  'installation/slackware',
  'installation/freebsd',
  'installation/building',
];

const editorNavItems = [
  'editor/spellchecking',
  'editor/time-formats',
];

const scriptingNavItems = [
  'scripting/',
  'scripting/methods-and-objects',
  'scripting/hooks',
  'scripting/classes',
];

const contributingNavItems = [
  'contributing/get-involved',
  'contributing/maintainers',
  'contributing/translation',
  'contributing/donate',
  'contributing/sponsors',
];

const faqNavItems = [
  'faq/preview-without-edit',
  'faq/references',
];

const gettingStartedNavItem = '/getting-started/overview';
const installationNavItem = '/installation/';
const changelogNavItem = '/changelog';
const donateNavItem = '/contributing/donate';

module.exports = {
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#title
   */
  title: 'QOwnNotes',

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
    ['meta', { name: 'theme-color', content: '#3eaf7c' }],
    ['meta', { name: 'apple-mobile-web-app-capable', content: 'yes' }],
    ['meta', { name: 'apple-mobile-web-app-status-bar-style', content: 'black' }],
    ['meta', { name: 'viewport', content: 'width=device-width, initial-scale=1.0' }],
    ['link', { rel: 'icon', type: 'image/png', sizes: '128x128', href: '/favicon.png' }],
    ['link', { rel: 'stylesheet', href: 'https://cdn.jsdelivr.net/npm/@mdi/font@5.x/css/materialdesignicons.min.css' }],
    ['link', { rel: 'alternate', title: 'QOwnNotes Releases RSS', type: 'application/rss+xml', href: 'https://feeds.feedburner.com/QOwnNotesReleases' }],
    ['link', { rel: 'alternate', title: 'QOwnNotes Blog RSS', type: 'application/rss+xml', href: 'https://feeds.feedburner.com/QOwnNotesBlog' }],
    // ['link', { rel: 'stylesheet', href: `https://cdn.jsdelivr.net/npm/vuetify/dist/vuetify.min.css` }],
    // ['script', { src: `https://cdn.jsdelivr.net/npm/vue/dist/vue.js` }],
    // ['script', { src: `https://cdn.jsdelivr.net/npm/vuetify/dist/vuetify.js` }],
  ],

  locales: {
    // The key is the path for the locale to be nested under.
    // As a special case, the default locale can use '/' as its path.
    '/': {
      lang: 'en-US', // this will be set as the lang attribute on <html>
      title: 'QOwnNotes',
    },
    '/de/': {
      lang: 'de-DE',
      title: 'QOwnNotes',
    },
    '/nl/': {
      lang: 'nl-NL',
      title: 'QOwnNotes',
    },
    '/hu/': {
      lang: 'hu-HU',
      title: 'QOwnNotes',
    },
    '/fa/': {
      lang: 'fr-FR',
      title: 'QOwnNotes',
          },
    '/es/': {
      lang: 'es-ES',
      title: 'QOwnNotes',
          },
    '/fa/': {
      lang: 'fa-IR',
      title: 'QOwnNotes',
    },
  },

  /**
   * Theme configuration, here is the default theme configuration for VuePress.
   *
   * ref：https://v1.vuepress.vuejs.org/theme/default-theme-config.html
   */
  themeConfig: {
    repo: 'pbek/QOwnNotes',
    editLinks: true,
    domain: 'https://www.qownnotes.org',
    author: { name: 'Patrizio Bekerle', twitter: '@PatrizioBekerle' },
    docsBranch: 'develop',
    docsDir: 'webpage/src',
    editLinkText: 'Help us improve this page!',
    lastUpdated: false,
    logo: '/favicon.png',
    locales: {
      '/': {
        // text for the language dropdown
        selectText: 'Languages',
        // label for this locale in the language dropdown
        label: 'English',
        // Aria Label for locale in the dropdown
        ariaLabel: 'Languages',
        // text for the edit-on-github link
        editLinkText: 'Help us improve this page!',
        // algolia docsearch options for current locale
        algolia: {
          apiKey: '35f878f4b712d5ab6a659eb0a6c71576',
          indexName: 'qownnotes'
        },
        nav: [
          {
            text: 'Getting started',
            link: gettingStartedNavItem,
          },
          {
            text: 'Installation',
            link: installationNavItem,
          },
          {
            text: 'Blog',
            link: '/blog/',
          },
          {
            text: 'Changelog',
            link: changelogNavItem,
            collapsable: true,
          },
          {
            text: 'Contact',
            ariaLabel: 'Contact Menu',
            items: contactNavItems
          },
          {
            text: 'Donate',
            link: donateNavItem,
          }
        ],
        sidebar: [
          {
            title: 'Getting started',
            children: gettingStartedNavItems
          },
          {
            title: 'Installation',
            children: installationNavItems
          },
          {
            title: 'Editor',
            children: editorNavItems
          },
          {
            title: 'Scripting',
            children: scriptingNavItems
          },
          {
            title: 'Contributing',
            children: contributingNavItems
          },
          {
            title: 'FAQ',
            children: faqNavItems
          },
          {
            title: 'Blog',
            children: utils.getChildren('src', 'blog', true)
          }
        ]
      },
      '/de/': {
        selectText: 'Sprachen',
        label: 'Deutsch',
        editLinkText: 'Hilf uns diese Seite zu verbessern!',
        algolia: {},
        nav: [
          {
            text: 'Loslegen',
            link: utils.getNavItemForLanguage(gettingStartedNavItem, 'de'),
          },
          {
            text: 'Installation',
            link: utils.getNavItemForLanguage(installationNavItem, 'de'),
          },
          {
            text: 'Änderungsprotokoll',
            link: changelogNavItem,
            collapsable: true,
          },
          {
            text: 'Kontakt',
            ariaLabel: 'Contact Menu',
            items: contactNavItems
          },
          {
            text: 'Spenden',
            link: utils.getNavItemForLanguage(donateNavItem, 'de'),
          }
        ],
        sidebar: [
          {
            title: 'Loslegen',
            children: utils.getNavItemsForLanguage(gettingStartedNavItems, "de")
          },
          {
            title: 'Installation',
            children: utils.getNavItemsForLanguage(installationNavItems, "de")
          },
          {
            title: 'Editor',
            children: utils.getNavItemsForLanguage(editorNavItems, "de")
          },
          {
            title: 'Skripten',
            children: utils.getNavItemsForLanguage(scriptingNavItems, "de")
          },
          {
            title: 'Beitragen',
            children: utils.getNavItemsForLanguage(contributingNavItems, "de")
          },
          {
            title: 'FAQ',
            children: utils.getNavItemsForLanguage(faqNavItems, "de")
          }
        ]
      },
      '/es/': {
        selectText: 'Idiomas',
        label: 'Español',
        editLinkText: '¡Ayúdanos a mejorar este sitio!',
        algolia: {},
        nav: [
          {
            text: 'Empezando',
            link: utils.getNavItemForLanguage(gettingStartedNavItem, 'es'),
          },
          {
            text: 'Instalación',
            link: utils.getNavItemForLanguage(installationNavItem, 'es'),
          },
          {
            text: 'Registro de cambios',
            link: changelogNavItem,
            collapsable: true,
          },
          {
            text: 'Contacto',
            ariaLabel: 'Contact Menu',
            items: contactNavItems
          },
          {
            text: 'Donar',
            link: utils.getNavItemForLanguage(donateNavItem, 'es'),
          }
        ],
        sidebar: [
          {
            title: 'Empezando',
            children: utils.getNavItemsForLanguage(gettingStartedNavItems, "es")
          },
          {
            title: 'Instalación',
            children: utils.getNavItemsForLanguage(installationNavItems, "es")
          },
          {
            title: 'Editor',
            children: utils.getNavItemsForLanguage(editorNavItems, "es")
          },
          {
            title: 'Guiones',
            children: utils.getNavItemsForLanguage(scriptingNavItems, "es")
          },
          {
            title: 'Contribuir',
            children: utils.getNavItemsForLanguage(contributingNavItems, "es")
          },
          {
            title: 'FAQ',
            children: utils.getNavItemsForLanguage(faqNavItems, "es")
          }
        ]
      },
      '/hu/': {
        selectText: 'Nyelvek',
        label: 'Magyar',
        editLinkText: 'Segítsen javítani ezt az oldalt!',
        algolia: {},
        nav: [
          {
            text: 'Startolunk',
            link: utils.getNavItemForLanguage(gettingStartedNavItem, 'hu'),
          },
          {
            text: 'Telepítés',
            link: utils.getNavItemForLanguage(installationNavItem, 'hu'),
          },
          {
            text: 'Változások',
            link: changelogNavItem,
            collapsable: true,
          },
          {
            text: 'Kapcsolat',
            ariaLabel: 'Contact Menu',
            items: contactNavItems
          },
          {
            text: 'Adomány',
            link: utils.getNavItemForLanguage(donateNavItem, 'hu'),
          }
        ],
        sidebar: [
          {
            title: 'Startol',
            children: utils.getNavItemsForLanguage(gettingStartedNavItems, "hu")
          },
          {
            title: 'Telepítés',
            children: utils.getNavItemsForLanguage(installationNavItems, "hu")
          },
          {
            title: 'Szerkesztő',
            children: utils.getNavItemsForLanguage(editorNavItems, "hu")
          },
          {
            title: 'Szkriptek',
            children: utils.getNavItemsForLanguage(scriptingNavItems, "hu")
          },
          {
            title: 'Adomány',
            children: utils.getNavItemsForLanguage(contributingNavItems, "hu")
          },
          {
            title: 'FAQ',
            children: utils.getNavItemsForLanguage(faqNavItems, "hu")
          }
        ]
      },
       '/fr/': {
        selectText: 'Langues',
        label: 'français',
        editLinkText: 'Aidez-nous à améliorer ce site!',
        algolia: {},
        nav: [
          {
            text: 'Commencer',
            link: utils.getNavItemForLanguage(gettingStartedNavItem, 'fr'),
          },
          {
            text: 'Installation',
            link: utils.getNavItemForLanguage(installationNavItem, 'fr'),
          },
          {
            text: 'Changelog',
            link: changelogNavItem,
            collapsable: true,
          },
          {
            text: 'Contact',
            ariaLabel: 'Contact Menu',
            items: contactNavItems
          },
          {
            text: 'Faire un don',
            link: utils.getNavItemForLanguage(donateNavItem, 'fr'),
          }
        ],
        sidebar: [
          {
            title: 'Commencer',
            children: utils.getNavItemsForLanguage(gettingStartedNavItems, "fr")
          },
          {
            title: 'Installation',
            children: utils.getNavItemsForLanguage(installationNavItems, "fr")
          },
          {
            title: 'Éditeur',
            children: utils.getNavItemsForLanguage(editorNavItems, "fr")
          },
          {
            title: 'Les scripts',
            children: utils.getNavItemsForLanguage(scriptingNavItems, "fr")
          },
          {
            title: 'Contribuer',
            children: utils.getNavItemsForLanguage(contributingNavItems, "fr")
          },
          {
            title: 'FAQ',
            children: utils.getNavItemsForLanguage(faqNavItems, "fr")
          }
        ]
      },
      '/fa/': {
        selectText: 'زبان ها',
        label: 'فارسی',
        editLinkText: 'برای بهبود این سایت به ما کمک کنید!',
        algolia: {},
        nav: [
          {
            text: 'شروع شدن',
            link: utils.getNavItemForLanguage(gettingStartedNavItem, 'fa'),
          },
          {
            text: 'نصب و راه اندازی',
            link: utils.getNavItemForLanguage(installationNavItem, 'fa'),
          },
          {
            text: 'تغییرات',
            link: changelogNavItem,
            collapsable: true,
          },
          {
            text: 'مخاطب',
            ariaLabel: 'Contact Menu',
            items: contactNavItems
          },
          {
            text: 'اهدا کردن',
            link: utils.getNavItemForLanguage(donateNavItem, 'fa'),
          }
        ],
        sidebar: [
          {
            title: 'شروع شدن',
            children: utils.getNavItemsForLanguage(gettingStartedNavItems, 'fa')
          },
          {
            title: 'نصب و راه اندازی',
            children: utils.getNavItemsForLanguage(installationNavItems, 'fa')
          },
          {
            title: 'ویرایشگر',
            children: utils.getNavItemsForLanguage(editorNavItems, 'fa')
          },
          {
            title: 'اسکریپت ها',
            children: utils.getNavItemsForLanguage(scriptingNavItems, 'fa')
          },
          {
            title: 'مشارکت',
            children: utils.getNavItemsForLanguage(contributingNavItems, 'fa')
          },
          {
            title: 'سالات متداول',
            children: utils.getNavItemsForLanguage(faqNavItems, 'fa')
          }
        ]
      },
      '/nl/': {
        selectText: 'Talen',
        label: 'Nederlands',
        editLinkText: 'Help ons deze pagina te verbeteren!',
        algolia: {},
        nav: [
          {
            text: 'Aan de slag',
            link: utils.getNavItemForLanguage(gettingStartedNavItem, 'nl'),
          },
          {
            text: 'Installatie',
            link: utils.getNavItemForLanguage(installationNavItem, 'nl'),
          },
          {
            text: 'Changelog',
            link: changelogNavItem,
            collapsable: true,
          },
          {
            text: 'Contact',
            ariaLabel: 'Contact Menu',
            items: contactNavItems
          },
          {
            text: 'Doneren',
            link: utils.getNavItemForLanguage(donateNavItem, 'nl'),
          }
        ],
        sidebar: [
          {
            title: 'Aan de slag',
            children: utils.getNavItemsForLanguage(gettingStartedNavItems, "nl")
          },
          {
            title: 'Installatie',
            children: utils.getNavItemsForLanguage(installationNavItems, "nl")
          },
          {
            title: 'Editor',
            children: utils.getNavItemsForLanguage(editorNavItems, "nl")
          },
          {
            title: 'Skripten',
            children: utils.getNavItemsForLanguage(scriptingNavItems, "nl")
          },
          {
            title: 'Bijdragen',
            children: utils.getNavItemsForLanguage(contributingNavItems, "nl")
          },
          {
            title: 'FAQ',
            children: utils.getNavItemsForLanguage(faqNavItems, "nl")
          }
        ]
      }
    }
  },
  /**
   * Apply plugins，ref：https://v1.vuepress.vuejs.org/plugin/
   */
  plugins: [
    '@vuepress/plugin-back-to-top',
    '@vuepress/plugin-medium-zoom',
    '@vuepress/last-updated', // https://vuepress.vuejs.org/plugin/official/plugin-last-updated.html#vuepress-plugin-last-updated
    'vuepress-plugin-mermaidjs', // https://github.com/eFrane/vuepress-plugin-mermaidjs
    [
      'vuepress-plugin-code-copy', // https://github.com/znicholasbrown/vuepress-plugin-code-copy
      {
        color: '#3eaf7c'
      }
    ],
    [
      'seo', // https://github.com/lorisleiva/vuepress-plugin-seo
      {
        description: $page => ($page.frontmatter.description || description),
        image: ($page, $site) => {
          if ($page.frontmatter.image) {
            return $site.themeConfig.domain &&
                !$page.frontmatter.image.startsWith('http')
                ? $site.themeConfig.domain + $page.frontmatter.image
                : $page.frontmatter.image;
          }

          return $site.themeConfig.domain + '/screenshots/screenshot.png'
        },
      },
    ],
    [
      'sitemap', // https://github.com/ekoeryanto/vuepress-plugin-sitemap
      {
        hostname: 'https://www.qownnotes.org',
        exclude: ['/404.html']
      },
    ],
    [
      'vuepress-plugin-matomo', // https://github.com/qdot/vuepress-plugin-matomo
      {
        siteId: 7,
        trackerUrl: 'https://p.bekerle.com/',
        trackerJsFile: 'matomo.js'
      }
    ],
    [
      'feed', // https://github.com/webmasterish/vuepress-plugin-feed
      {
        feed_options: {
          title: 'QOwnNotes Blog',
          description: "News about QOwnNotes, the open source markdown note taking application for Linux, Mac OS X and Windows, that works together with Nextcloud Notes",
        },
        canonical_base: 'https://www.qownnotes.org',
        sort: entries => _.reverse( _.sortBy( entries, 'date' ) ),
      }
    ],
  ],
  markdown: {
    lineNumbers: true
  }
}
