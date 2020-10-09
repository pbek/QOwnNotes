const { description } = require('../../package')

module.exports = {
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#title
   */
  title: 'QOwnNotes',
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#description
   */
  description: description,

  /**
   * Extra tags to be injected to the page HTML `<head>`
   *
   * ref：https://v1.vuepress.vuejs.org/config/#head
   */
  head: [
    ['meta', { name: 'theme-color', content: '#3eaf7c' }],
    ['meta', { name: 'apple-mobile-web-app-capable', content: 'yes' }],
    ['meta', { name: 'apple-mobile-web-app-status-bar-style', content: 'black' }],
    ['link', { rel: 'icon', type: 'image/png', sizes: '128x128', href: '/favicon.png' }],
    ['link', { rel: 'stylesheet', href: 'https://cdn.jsdelivr.net/npm/@mdi/font@5.x/css/materialdesignicons.min.css' }],
    // ['link', { rel: 'stylesheet', href: `https://cdn.jsdelivr.net/npm/vuetify/dist/vuetify.min.css` }],
    // ['script', { src: `https://cdn.jsdelivr.net/npm/vue/dist/vue.js` }],
    // ['script', { src: `https://cdn.jsdelivr.net/npm/vuetify/dist/vuetify.js` }],
  ],

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
    nav: [
      {
        text: 'Getting started',
        link: '/getting-started/overview',
      },
      {
        text: 'Installation',
        link: '/installation/',
      },
      {
        text: 'Changelog',
        link: '/changelog',
        collapsable: true,
      },
      {
        text: 'Contact',
        ariaLabel: 'Contact Menu',
        items: [
          { text: 'Ask question', link: 'https://github.com/pbek/QOwnNotes/issues#support' },
          { text: 'Ask for feature', link: 'https://github.com/pbek/QOwnNotes/issues#feature' },
          { text: 'Report bug', link: 'https://github.com/pbek/QOwnNotes/issues#bug' },
          { text: 'Telegram Channel', link: 'https://t.me/QOwnNotes' },
          { text: 'Matrix/Riot.im Chat', link: 'https://riot.im/app/#/room/!rUzrRvrnrOsLasDdbp:matrix.org?via=matrix.org' },
          { text: 'Gitter Chat', link: 'https://gitter.im/qownnotes/qownnotes' },
          { text: 'IRC Channel', link: 'https://kiwiirc.com/client/irc.freenode.net/#qownnotes' },
          { text: 'IRC Log', link: 'https://quodlibet.duckdns.org/irc/qownnotes/latest.log.html' },
        ]
      },
      {
        text: 'Donate',
        link: '/contributing/donate',
      }
    ],
    sidebar: [
      {
        title: 'Getting started',
        children: [
          'getting-started/overview',
          'getting-started/concept',
          'getting-started/cli-parameters',
          'getting-started/browser-extension',
          'getting-started/qownnotesapi',
          'getting-started/searching',
          'getting-started/shortcuts',
        ]
      },
      {
        title: 'Installation',
        children: [
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
        ]
      },
      {
        title: 'Editor',
        children: [
          'editor/spellchecking',
          'editor/time-formats',
        ]
      },
      {
        title: 'Scripting',
        children: [
          'scripting/',
          'scripting/methods-and-objects',
          'scripting/hooks',
          'scripting/classes',
        ]
      },
      {
        title: 'Contributing',
        children: [
          'contributing/get-involved',
          'contributing/maintainers',
          'contributing/translation',
          '/contributing/donate',
          'contributing/sponsors',
        ]
      },
      {
        title: 'FAQ',
        children: [
          'faq/preview-without-edit',
          'faq/references',
        ]
      }
    ]
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
      "vuepress-plugin-matomo", // https://github.com/qdot/vuepress-plugin-matomo
      {
        siteId: 7,
        trackerUrl: 'https://p.bekerle.com/',
        trackerJsFile: 'matomo.js'
      }
    ],
  ],

  markdown: {
    lineNumbers: true
  }
}
