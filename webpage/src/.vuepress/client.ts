import { defineClientConfig } from 'vuepress/client'
import { createVuetify } from 'vuetify'
import * as components from 'vuetify/components'
import * as directives from 'vuetify/directives'
import 'vuetify/styles'

// Import custom components
import Poll from './components/Poll.vue'
import BlogDate from './components/BlogDate.vue'
import BlogIndex from './components/BlogIndex.vue'
import ProfileCard from './components/ProfileCard.vue'

// Import Matomo analytics (replaces vuepress-plugin-matomo)
import { initMatomo } from './utils/matomo.js'

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
    initMatomo(router)
  },
  setup() {},
  rootComponents: [],
})

