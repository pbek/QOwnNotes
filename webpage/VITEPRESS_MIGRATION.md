# VitePress Migration

This documentation site has been migrated from VuePress v1 to VitePress.

## What Changed

### Configuration

- `config.js` → `config.mts` (TypeScript)
- Configuration structure updated to VitePress API
- Locales configuration simplified
- Sidebar and navigation updated to VitePress format

### Removed Dependencies

- Removed VuePress plugins (most functionality is now built-in or handled differently):
  - `@vuepress/plugin-back-to-top` - Built into VitePress
  - `@vuepress/plugin-medium-zoom` - Built into VitePress
  - `vuepress-plugin-code-copy` - Built into VitePress
  - `vuepress-plugin-mermaidjs` - Can be added via markdown-it plugin if needed
  - `vuepress-plugin-seo` - Implemented via transformHead
  - `vuepress-plugin-sitemap` - Implemented as custom plugin
  - `vuepress-plugin-canonical` - Implemented via transformHead
  - `vuepress-plugin-matomo` - Can be added to theme if needed
  - `vuepress-plugin-feed` - Can be implemented separately if needed
  - Vuetify - Not needed, VitePress has its own component system

### Custom Components

Vue components (Poll.vue, etc.) that used Vuetify will need to be rewritten using plain Vue 3 or removed if not critical.

### Benefits of VitePress

- ⚡ Much faster development and build times (uses Vite instead of Webpack)
- 🎯 Better performance with static site generation
- 📦 Smaller bundle size
- 🔧 Built-in features (code copy, image zoom, back to top)
- 🌐 Better i18n support
- 🎨 Modern Vue 3 and TypeScript support

## Setup

```bash
# Install dependencies
npm install

# Development
npm run dev

# Build
npm run build

# Preview build
npm run preview
```

## Migration Notes

### Home Page

- Updated frontmatter format from VuePress to VitePress layout: home
- Removed Vuetify components (v-chip, v-icon)
- Simplified RSS links

### Scripts

- Removed Node.js legacy OpenSSL workarounds (not needed with Vite)
- Simplified build process

### RTL Support

- Arabic and Persian locales still have RTL support
- Applied via CSS in custom theme

## TODO

- [ ] Test all pages and links
- [ ] Verify search functionality with Algolia
- [ ] Re-implement Matomo analytics if needed
- [ ] Re-implement RSS feed generation if needed
- [ ] Convert or remove custom Vue components (Poll, BlogIndex, etc.)
- [ ] Test all locale versions
- [ ] Verify image paths and assets
- [ ] Test build and deployment
