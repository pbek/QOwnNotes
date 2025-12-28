import { defineClientConfig } from 'vuepress/client'
import { createVuetify } from 'vuetify'
import * as components from 'vuetify/components'
import * as directives from 'vuetify/directives'
import 'vuetify/styles'

// Matomo type declaration
declare global {
  interface Window {
    _paq: any[]
  }
}

// Import custom components
import Poll from './components/Poll.vue'
import BlogDate from './components/BlogDate.vue'
import BlogIndex from './components/BlogIndex.vue'
import ProfileCard from './components/ProfileCard.vue'

export default defineClientConfig({
  enhance({ app, router, siteData }) {
    // Create Vuetify instance
    const vuetify = createVuetify({
      components,
      directives,
      theme: {
        defaultTheme: 'light',
      },
    })

    // Use Vuetify
    app.use(vuetify)

    // Register custom components globally
    app.component('Poll', Poll)
    app.component('BlogDate', BlogDate)
    app.component('BlogIndex', BlogIndex)
    app.component('ProfileCard', ProfileCard)

    // Initialize Matomo Analytics (replaces vuepress-plugin-matomo)
    // Only run in browser environment
    if (typeof window !== 'undefined') {
      // Configuration
      const matomoConfig = {
        siteId: 7,
        trackerUrl: 'https://p.bekerle.com/',
        trackerJsFile: 'matomo.js',
      }

      // Initialize Matomo
      window._paq = window._paq || []
      window._paq.push(['trackPageView'])
      window._paq.push(['enableLinkTracking'])

      // Load Matomo script
      ;(function () {
        const u = matomoConfig.trackerUrl
        window._paq.push(['setTrackerUrl', u + 'matomo.php'])
        window._paq.push(['setSiteId', matomoConfig.siteId.toString()])

        const d = document
        const g = d.createElement('script')
        const s = d.getElementsByTagName('script')[0]
        g.async = true
        g.src = u + matomoConfig.trackerJsFile
        s.parentNode.insertBefore(g, s)
      })()

      // Track page changes on route navigation (SPA support)
      router.afterEach((to) => {
        if (window._paq) {
          window._paq.push(['setCustomUrl', window.location.href])
          window._paq.push(['setDocumentTitle', document.title])
          window._paq.push(['trackPageView'])
        }
      })
    }
  },
  setup() {},
  rootComponents: [Poll],
})

